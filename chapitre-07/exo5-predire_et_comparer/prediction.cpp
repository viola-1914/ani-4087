// prediction.cpp — chapitre 07, exercice 5 : predire et comparer.
//
// La tete tourne a la souris (accumulation des mouvements bruts, lus une fois
// par image : lacet -= total x * 0,0022 rad, tangage -= total y * 0,0022 rad,
// accumulateur vide). L'orientation vient du chemin des matrices, comme le
// simulateur ; la camera la suit.
// A chaque image :
//   1. la vitesse angulaire entre l'image d'avant et celle-ci (calcul du moteur) ;
//   2. l'orientation PREDITE pour l'instant ou l'image sera vue : l'orientation
//      actuelle, tournee a cette vitesse pendant une periode de l'ecran
//      (MASALLE_HZ, la frequence lue dans les parametres de Windows) ;
//   3. a l'image suivante, on compare cette prediction a l'orientation mesuree
//      (erreur avec prediction), et l'orientation d'avant, prise telle quelle,
//      a celle de maintenant (erreur sans prediction) — sur les memes images.
// Apres mille images comparees (trente ecartees au demarrage), on imprime les
// deux moyennes et les deux maximums, et le nombre d'images ou la tete etait
// immobile. La salle se ferme seule.
// Sortie : std::printf et prediction_sortie.txt (pas de console en Release).

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkMouseEvent.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKLogger/NkLog.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "NKMath/NkAngle.h"
#include "NKMath/NkMat.h"
#include "NKMath/NkQuat.h"
#include "NKTime/NkChrono.h"
#include "NKRHI/Core/NkGraphicsApi.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "MaSallePrediction";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

// Sensibilite en radians par compte du capteur, celle de l'enonce.
static const float32 SENSIBILITE = 0.0022f;

static const float32 DEGRES_PAR_RADIAN = 57.2957795f;

// ── La tete ──────────────────────────────────────────────────────────────
struct Tete {
    float32 lacet = 0.f;    ///< radians
    float32 tangage = 0.f;  ///< radians
    NkVec3f position = {0.f, 1.7f, 0.f};  ///< metres

    // Direction du regard. Convention de la salle : avant = -Z, haut = +Y,
    // repere de la main droite.
    NkVec3f Regard() const {
        const float32 l = lacet;
        return {-std::sin(l) * std::cos(tangage),
                std::sin(tangage),
                -std::cos(l) * std::cos(tangage)};
    }

    // Crochet de test : pose la tete sans la souris.
    void PoserTete(float32 lacetRad, float32 tangageRad, const NkVec3f &positionM) {
        lacet = lacetRad;
        tangage = tangageRad;
        position = positionM;
    }
};

// ── Lecture des variables d'environnement ──────────────────────────────
// Comme EnvU64 lit MASALLE_EXIT : la valeur entiere, ou la valeur par defaut.
static uint64 EnvU64(const char *nom, uint64 parDefaut) {
    const char *v = std::getenv(nom);
    if (v == nullptr || v[0] == '\0') {
        return parDefaut;
    }
    return (uint64)std::strtoull(v, nullptr, 10);
}

// Images ecartees au demarrage, images comparees.
static const uint64 IMAGES_ECARTEES = 30;
static const uint64 IMAGES_COMPAREES = 1000;

// ── La vitesse angulaire entre deux poses horodatees ────────────────────
// Comme le module de realite virtuelle du moteur. Le vecteur rendu a pour
// direction l'axe de la rotation, pour longueur la vitesse en rad/s.
static NkVec3f VitesseAngulaire(const NkQuatf &from, const NkQuatf &to, float32 dtSeconds) {
    if (dtSeconds <= 0.f) {
        return NkVec3f(0.f, 0.f, 0.f);
    }
    NkQuatf delta = (to * from.Conjugate()).Normalized();
    // Les deux hemispheres du quaternion codent la meme rotation ; on
    // force le chemin court, sinon un delta minuscule peut se lire
    // comme un tour presque complet dans l'autre sens.
    if (delta.w < 0.f) {
        delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);
    }
    const float32 angleRad = delta.Angle().Rad();
    if (angleRad <= 1e-6f) {
        return NkVec3f(0.f, 0.f, 0.f);
    }
    return delta.Axis() * (angleRad / dtSeconds);
}

// L'orientation de la tete, comme le simulateur : deux matrices, une conversion.
static NkQuatf Orientation(float32 lacetRad, float32 tangageRad) {
    const NkMat4f rotation = NkMat4f::RotationY(NkAngle::FromRad(lacetRad)) *
                             NkMat4f::RotationX(NkAngle::FromRad(tangageRad));
    const NkQuatf orientation = NkQuatf(rotation).Normalized();
    return orientation;
}

static void Ecrire(std::FILE *sortie, const char *ligne) {
    std::printf("%s\n", ligne);
    if (sortie != nullptr) {
        std::fprintf(sortie, "%s\n", ligne);
    }
}

// ── Predire : faire tourner l'orientation a cette vitesse ───────────────
// Comme le module de realite virtuelle du moteur, pour l'orientation seule.
static NkQuatf Extrapoler(const NkQuatf &orientation, const NkVec3f &vitesse, float32 dtSeconds) {
    const float32 speedRad = vitesse.Len();
    if (speedRad > 1e-6f) {
        const NkVec3f axis = vitesse * (1.f / speedRad);
        // Delta en espace de BASE : il se compose a GAUCHE de
        // l'orientation courante (meme convention que la mesure).
        const NkQuatf delta(NkAngle::FromRad(speedRad * dtSeconds), axis);
        return (delta * orientation).Normalized();
    }
    return orientation;
}

// L'angle qui separe deux orientations, en degres.
static float32 AngleEntre(const NkQuatf &a, const NkQuatf &b) {
    return (b * a.Conjugate()).Normalized().Angle().Deg();
}

// Une piece de la salle : un cube unite (-0,5 a +0,5) mis a l'echelle de sa
// taille puis translate a son centre. Ordre : Translate * Scale.
struct Piece {
    NkVec3f centre;
    NkVec3f taille;
    NkVec3f couleur;
};

// Positions en metres, reprises de salle_apres.cpp (exercice 8).
static const Piece PIECES[] = {
    {{0.f, -0.05f, 0.f}, {6.f, 0.1f, 4.f}, {0.35f, 0.33f, 0.30f}},          // sol
    {{0.f, 1.4f, -2.05f}, {6.2f, 2.8f, 0.1f}, {0.85f, 0.83f, 0.78f}},       // mur du fond
    {{0.f, 1.4f, 2.05f}, {6.2f, 2.8f, 0.1f}, {0.85f, 0.83f, 0.78f}},        // mur de l'entree
    {{-3.05f, 1.4f, 0.f}, {0.1f, 2.8f, 4.f}, {0.85f, 0.83f, 0.78f}},        // mur gauche
    {{3.05f, 1.4f, 0.f}, {0.1f, 2.8f, 4.f}, {0.85f, 0.83f, 0.78f}},         // mur droit
    {{-1.5f, 1.f, 1.98f}, {0.9f, 2.f, 0.04f}, {0.45f, 0.28f, 0.15f}},       // porte
    {{-1.5f, 1.5f, -1.98f}, {1.5f, 1.2f, 0.04f}, {0.55f, 0.75f, 0.90f}},    // fenetre du fond
    {{2.98f, 1.5f, 0.5f}, {0.04f, 1.2f, 1.5f}, {0.55f, 0.75f, 0.90f}},      // fenetre de droite
    {{0.5f, 0.78f, 0.f}, {1.2f, 0.04f, 0.8f}, {0.60f, 0.42f, 0.25f}},       // plateau
    {{-0.04f, 0.38f, -0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}}, // pied
    {{1.04f, 0.38f, -0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}},  // pied
    {{-0.04f, 0.38f, 0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}},  // pied
    {{1.04f, 0.38f, 0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}},   // pied
    {{0.4f, 0.81f, 0.f}, {0.34f, 0.02f, 0.24f}, {0.15f, 0.15f, 0.17f}},     // ordinateur
    {{0.9f, 1.025f, -0.2f}, {0.15f, 0.45f, 0.15f}, {0.95f, 0.85f, 0.40f}},  // lampe
};

int nkmain(const NkEntryState &) {
    // L'origine des instants : le lancement du programme.
    const float64 lancement = NkChrono::Now().nanoseconds;
    std::FILE *sortie = std::fopen("prediction_sortie.txt", "w");

    // MASALLE_HZ : la frequence de l'ecran (parametres d'affichage de Windows).
    // Le temps qui reste avant l'affichage est une periode de cet ecran.
    const uint64 hzLu = EnvU64("MASALLE_HZ", 60);
    const float32 hz = (hzLu > 1) ? (float32)hzLu : 60.f;
    const float32 periode = 1.f / hz;  // secondes

    // MASALLE_EXIT : la salle s'arrete seule apres ce nombre d'images (garde-fou).
    const uint64 imagesMax = EnvU64("MASALLE_EXIT", 0);

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle - regardez autour de vous a la souris (environ 20 secondes)";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[MaSallePrediction] Creation fenetre KO");
        return 1;
    }

    // Pointeur cache et retenu dans la fenetre : la souris pilote la tete.
    window.ShowMouse(false);
    window.CaptureMouse(true);
    window.ClipMouseToClient(true);

    uint32 largeur = (uint32)window.GetSize().width;
    uint32 hauteur = (uint32)window.GetSize().height;

    // On essaie les interfaces une a une et on garde la premiere qui demarre
    // (sur cette machine : Vulkan echoue, DirectX 12 ne prepare pas le PBR).
    const NkGraphicsApi ordre[4] = {
        NkGraphicsApi::NK_GFX_API_VULKAN,
        NkGraphicsApi::NK_GFX_API_OPENGL,
        NkGraphicsApi::NK_GFX_API_DX11,
        NkGraphicsApi::NK_GFX_API_DX12,
    };

    NkIDevice *device = nullptr;
    NkRenderer *renderer = nullptr;
    char echecs[160] = "";  // les interfaces essayees en vain, pour le rapport
    for (int i = 0; i < 4 && renderer == nullptr; ++i) {
        NkDeviceInitInfo devInfo{};
        devInfo.surface = window.GetSurfaceDesc();
        devInfo.width = largeur;
        devInfo.height = hauteur;
        devInfo.api = ordre[i];

        device = NkDeviceFactory::CreateWithFallback(devInfo, {ordre[i]});
        if (device == nullptr || !device->IsValid()) {
            NkDeviceFactory::Destroy(device);
            std::strncat(echecs, NkGraphicsApiName(ordre[i]), sizeof(echecs) - std::strlen(echecs) - 1);
            std::strncat(echecs, " (device) ", sizeof(echecs) - std::strlen(echecs) - 1);
            continue;
        }

        NkRendererConfig cfg;
        cfg.api = device->GetApi();
        cfg.width = largeur;
        cfg.height = hauteur;
        cfg.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;

        renderer = NkRenderer::Create(device, cfg);
        if (renderer != nullptr && renderer->Initialize()) {
            break;
        }
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        std::strncat(echecs, NkGraphicsApiName(ordre[i]), sizeof(echecs) - std::strlen(echecs) - 1);
        std::strncat(echecs, " (renderer) ", sizeof(echecs) - std::strlen(echecs) - 1);
    }

    if (renderer == nullptr) {
        logger.Error("[MaSallePrediction] Aucune interface graphique ne demarre le renderer");
        window.ClipMouseToClient(false);
        window.CaptureMouse(false);
        window.ShowMouse(true);
        window.Close();
        return 2;
    }

    const char *nomApi = NkGraphicsApiName(device->GetApi());
    logger.Info("[MaSallePrediction] api={0}", nomApi);

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle cube = renderer->GetMeshSystem()->GetCube();

    bool enMarche = true;
    NkEventSystem &evenements = NkEvents();
    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) {
        enMarche = false;
    });
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) {
            enMarche = false;
        }
    });
    // Symptôme : avec NkInput.MouseRawDeltaX() lu a chaque image, la tete
    //   tourne toute seule, a vitesse constante, des que la souris s'arrete.
    // Cause : le module d'entrees garde le deplacement du DERNIER evenement
    //   brut et ne le remet jamais a zero ; le relire a chaque image rejoue ce
    //   dernier deplacement soixante fois par seconde.
    // Preuve : dans NKEvent/NkEventState.h, OnRaw() ecrase rawDeltaX/Y a
    //   chaque evenement et rien ne les consomme ; mesure faite sur cette salle :
    //   souris immobile, brut reste a -1 pendant 12 s et la tete derive de
    //   6,9 deg/s (60 images/s x 1 compte x 0,002 rad).
    // Correction : on s'abonne aux evenements bruts et on additionne leurs
    //   deplacements ici ; la boucle de rendu lit ce total UNE fois par
    //   image, PUIS le remet a zero. Sans la remise a zero, on reproduit le
    //   meme defaut : le total deja applique est rejoue a chaque image.
    int32 accumVideX = 0;
    int32 accumVideY = 0;
    evenements.AddEventCallback<NkMouseRawEvent>([&](NkMouseRawEvent *e) {
        // A chaque evenement : on AJOUTE, on ne lit ni ne vide.
        accumVideX += e->GetDeltaX();
        accumVideY += e->GetDeltaY();
    });
    evenements.AddEventCallback<NkWindowFocusGainedEvent>([&](NkWindowFocusGainedEvent *) {
        // Windows relache le confinement quand le premier plan change.
        window.ClipMouseToClient(true);
    });
    evenements.AddEventCallback<NkWindowResizeEvent>([&](NkWindowResizeEvent *e) {
        const uint32 w = (uint32)e->GetWidth();
        const uint32 h = (uint32)e->GetHeight();
        if (w > 0 && h > 0 && (w != largeur || h != hauteur)) {
            largeur = w;
            hauteur = h;
            renderer->OnResize(w, h);
        }
    });

    // La tete, posee au depart vers la table par le meme crochet que le test.
    Tete tete;
    tete.PoserTete(-1.009f, -0.274f, {-2.2f, 1.7f, 1.7f});

    uint64 image = 0;
    bool aOrientationPrecedente = false;
    NkQuatf orientationPrecedente = NkQuatf::Identity();
    NkQuatf orientationPredite = NkQuatf::Identity();  // predite a l'image d'avant
    float64 instantPrecedent = 0.0;   // secondes

    uint64 nbCompares = 0;
    uint64 nbImmobiles = 0;           // images ou la souris n'a rien envoye
    float64 sommeAvec = 0.0, sommeSans = 0.0;
    float32 maxAvec = 0.f, maxSans = 0.f;
    char ligne[220];

    while (enMarche && window.IsOpen()) {
        // ── DEBUT DE L'IMAGE : son instant, en secondes (float64) ──
        const float64 instant = (NkChrono::Now().nanoseconds - lancement) / 1.0e9;

        ++image;
        if (imagesMax > 0 && image > imagesMax) {
            break;
        }

        // Les evenements de cette image remplissent les accumulateurs.
        evenements.PollEvents();
        if (!enMarche) {
            break;
        }

        // On LIT le total de l'image, PUIS on le vide.
        const bool immobile = (accumVideX == 0 && accumVideY == 0);
        tete.lacet -= (float32)accumVideX * SENSIBILITE;
        tete.tangage -= (float32)accumVideY * SENSIBILITE;
        accumVideX = 0;
        accumVideY = 0;

        // Tangage borne a +/-89 degres : a 90, le lacet devient indetermine.
        const float32 limite = 89.f / DEGRES_PAR_RADIAN;
        if (tete.tangage > limite) {
            tete.tangage = limite;
        }
        if (tete.tangage < -limite) {
            tete.tangage = -limite;
        }

        // L'orientation MESUREE de cette image, par le chemin des matrices.
        const NkQuatf orientation = Orientation(tete.lacet, tete.tangage);

        if (aOrientationPrecedente) {
            // ── La prediction faite a l'image d'avant, jugee maintenant ──
            if (image > IMAGES_ECARTEES + 1 && nbCompares < IMAGES_COMPAREES) {
                const float32 errAvec = AngleEntre(orientationPredite, orientation);
                const float32 errSans = AngleEntre(orientationPrecedente, orientation);
                sommeAvec += errAvec;
                sommeSans += errSans;
                maxAvec = (errAvec > maxAvec) ? errAvec : maxAvec;
                maxSans = (errSans > maxSans) ? errSans : maxSans;
                nbImmobiles += immobile ? 1 : 0;
                ++nbCompares;
            }

            // ── La prediction pour l'instant ou CETTE image sera vue ──
            const float32 dt = (float32)(instant - instantPrecedent);
            const NkVec3f vitesse = VitesseAngulaire(orientationPrecedente, orientation, dt);
            orientationPredite = Extrapoler(orientation, vitesse, periode);
        } else {
            orientationPredite = orientation;
        }
        orientationPrecedente = orientation;
        instantPrecedent = instant;
        aOrientationPrecedente = true;

        // Mille images comparees : la salle se ferme seule.
        if (nbCompares >= IMAGES_COMPAREES) {
            break;
        }

        if (!renderer->BeginFrame()) {
            continue;
        }

        // La camera suit l'orientation : cible = position + orientation * avant.
        NkCamera3DData cd;
        cd.position = tete.position;
        cd.target = cd.position + orientation * NkVec3f(0.f, 0.f, -1.f);
        cd.up = {0.f, 1.f, 0.f};
        cd.fovY = 70.f;
        cd.aspect = (float32)largeur / (float32)hauteur;
        cd.nearPlane = 0.05f;
        cd.farPlane = 50.f;
        NkCamera3D cam(cd);

        NkSceneContext sctx;
        sctx.camera = cam;

        NkLightDesc lumiere;
        lumiere.type = NkLightType::NK_DIRECTIONAL;
        lumiere.direction = {-0.3f, -1.f, -0.4f};
        lumiere.color = {1.f, 0.96f, 0.9f};
        lumiere.intensity = 3.f;
        lumiere.castShadow = true;
        sctx.lights.PushBack(lumiere);

        r3d->BeginScene(sctx);
        for (const Piece &piece : PIECES) {
            NkDrawCall3D dc;
            dc.mesh = cube;
            dc.transform = NkMat4f::Translate(piece.centre) * NkMat4f::Scale(piece.taille);
            dc.aabb = {{piece.centre.x - piece.taille.x * 0.5f,
                        piece.centre.y - piece.taille.y * 0.5f,
                        piece.centre.z - piece.taille.z * 0.5f},
                       {piece.centre.x + piece.taille.x * 0.5f,
                        piece.centre.y + piece.taille.y * 0.5f,
                        piece.centre.z + piece.taille.z * 0.5f}};
            dc.tint = piece.couleur;
            dc.roughness = 0.8f;
            r3d->Submit(dc);
        }

        renderer->Present();
        renderer->EndFrame();
    }


    if (nbCompares > 0) {
        std::snprintf(ligne, sizeof(ligne), "avec prediction : moyenne %.4f deg, max %.4f deg, sur %d images",
                      sommeAvec / (float64)nbCompares, (double)maxAvec, (int)nbCompares);
        Ecrire(sortie, ligne);
        std::snprintf(ligne, sizeof(ligne), "sans prediction : moyenne %.4f deg, max %.4f deg, sur %d images",
                      sommeSans / (float64)nbCompares, (double)maxSans, (int)nbCompares);
        Ecrire(sortie, ligne);
    }
    std::snprintf(ligne, sizeof(ligne), "(info) api=%s, hz=%d, periode=%.3f ms, images immobiles = %d sur %d", nomApi,
                  (int)hz, (double)periode * 1000.0, (int)nbImmobiles, (int)nbCompares);
    Ecrire(sortie, ligne);
    std::fflush(stdout);
    if (sortie != nullptr) {
        std::fclose(sortie);
    }

    // Le pointeur est toujours rendu avant de finir.
    window.ClipMouseToClient(false);
    window.CaptureMouse(false);
    window.ShowMouse(true);

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}
