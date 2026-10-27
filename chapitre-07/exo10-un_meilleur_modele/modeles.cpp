// modeles.cpp — chapitre 07, exercice 10 : un meilleur modele.
//
// Au demarrage, avant d'ouvrir la fenetre, sur le lacet seul, en degres, sans
// quaternion : quatre enregistrements de tete echantillonnes a 72 hertz.
//   lent      : 30 deg en 1000 ms (depart et arrivee doux)
//   rapide    : 90 deg en 400 ms (le quart de tour de l'enonce)
//   demi-tour : 180 deg en 250 ms (depart et arrivee doux)
//   arret     : 200 deg/s constants pendant 300 ms, puis arret net
// Deux modeles predisent la pose une periode plus loin, depuis les memes poses :
//   vitesse      : lacet + vitesse * periode (vitesse tiree des 2 dernieres poses)
//   acceleration : la meme chose + acceleration * periode^2 / 2 (acceleration
//                  tiree des 3 dernieres poses)
// Erreur moyenne et maximale, en degres, pendant le geste et les 200 ms qui
// suivent. Sortie : std::printf et modeles_sortie.txt. Puis la salle s'ouvre.
// Variable d'environnement : MASALLE_CALCUL_SEUL=1 calcule puis quitte.

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
#include "NKMath/NkFunctions.h"
#include "NKMath/NkMat.h"
#include "NKMath/NkQuat.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "MaSalleModeles";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

// Sensibilite en radians par compte du capteur.
static const float32 SENSIBILITE = 0.002f;

static const float32 DEGRES_PAR_RADIAN = 57.2957795f;
static const float32 RADIANS_PAR_DEGRE = 0.0174532925f;

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

    void PoserTete(float32 lacetRad, float32 tangageRad, const NkVec3f &positionM) {
        lacet = lacetRad;
        tangage = tangageRad;
        position = positionM;
    }
};

// ── Les gestes ───────────────────────────────────────────────────────────
static const double kPi = 3.14159265358979323846;

// La forme commune : depart et arrivee doux. Avec amplitude 90 et duree
// 0,400, c'est le quart de tour en 400 ms de l'enonce.
static double LacetDoux(double t, double amplitude, double duree) {  // t en s, lacet en deg
    if (t <= 0.0) {
        return 0.0;
    }
    if (t >= duree) {
        return amplitude;
    }
    return amplitude * 0.5 * (1.0 - std::cos(kPi * t / duree));
}

// L'arret brusque : vitesse constante, puis arret NET (la vitesse tombe a zero
// d'un coup, entre deux echantillons).
static double LacetArret(double t, double vitesse, double duree) {
    if (t <= 0.0) {
        return 0.0;
    }
    if (t >= duree) {
        return vitesse * duree;
    }
    return vitesse * t;
}

struct Enregistrement {
    const char *nom;
    bool arret;        // false : forme douce ; true : vitesse constante puis arret net
    double a;          // amplitude (deg) pour la forme douce, vitesse (deg/s) pour l'arret
    double duree;      // secondes
    double amplitude;  // degres parcourus, pour l'etiquette
};

static double Lacet(const Enregistrement &e, double t) {
    return e.arret ? LacetArret(t, e.a, e.duree) : LacetDoux(t, e.a, e.duree);
}

static void Ecrire(std::FILE *f, const char *ligne) {
    std::printf("%s\n", ligne);
    if (f != nullptr) {
        std::fprintf(f, "%s\n", ligne);
    }
}

static void CalculerModeles() {
    const double hz = 72.0;
    const double periode = 1.0 / hz;  // secondes
    const double apres = 0.200;       // les 200 ms qui suivent le geste

    const Enregistrement enregistrements[4] = {
        {"rapide", false, 90.0, 0.400, 90.0},
        {"lent", false, 30.0, 1.000, 30.0},
        {"demi-tour", false, 180.0, 0.250, 180.0},
        {"arret", true, 200.0, 0.300, 60.0},  // 200 deg/s pendant 300 ms : 60 deg
    };

    std::FILE *sortie = std::fopen("modeles_sortie.txt", "w");
    char ligne[220];
    for (const Enregistrement &e : enregistrements) {
        double sommeV = 0.0, sommeA = 0.0, maxV = 0.0, maxA = 0.0;
        int n = 0, gagneA = 0, gagneV = 0;
        // Une pose par periode ; on juge chaque prediction contre la vraie pose
        // une periode plus loin, du debut du geste a 200 ms apres sa fin.
        for (int k = 0; k * periode <= e.duree + apres + 1e-9; ++k) {
            const double t = k * periode;
            // Les trois dernieres poses (avant le debut, la tete est immobile a 0).
            const double l0 = Lacet(e, t);
            const double l1 = Lacet(e, t - periode);
            const double l2 = Lacet(e, t - 2.0 * periode);

            // Vitesse : des deux dernieres poses (deg/s).
            const double vitesse = (l0 - l1) / periode;
            // Acceleration : des trois dernieres poses, difference seconde divisee
            // par le CARRE de la periode (deg/s^2).
            const double acceleration = (l0 - 2.0 * l1 + l2) / (periode * periode);

            // Les deux modeles, meme horizon (une periode), memes poses.
            const double predVitesse = l0 + vitesse * periode;
            const double predAcceleration = l0 + vitesse * periode + acceleration * periode * periode / 2.0;

            const double vrai = Lacet(e, t + periode);
            const double errV = std::fabs(predVitesse - vrai);
            const double errA = std::fabs(predAcceleration - vrai);
            sommeV += errV;
            sommeA += errA;
            maxV = (errV > maxV) ? errV : maxV;
            maxA = (errA > maxA) ? errA : maxA;
            gagneA += (errA < errV - 1e-9) ? 1 : 0;
            gagneV += (errV < errA - 1e-9) ? 1 : 0;
            ++n;
        }
        std::snprintf(ligne, sizeof(ligne), "%s (%.0f deg en %.0f ms) : vitesse %.3f deg (max %.3f), acceleration %.3f deg (max %.3f)",
                      e.nom, e.amplitude, e.duree * 1000.0, sommeV / n, maxV, sommeA / n, maxA);
        Ecrire(sortie, ligne);
        std::snprintf(ligne, sizeof(ligne), "(info) %s : %d poses jugees, acceleration meilleure sur %d, vitesse meilleure sur %d",
                      e.nom, n, gagneA, gagneV);
        Ecrire(sortie, ligne);
    }
    std::fflush(stdout);
    if (sortie != nullptr) {
        std::fclose(sortie);
    }
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
    // Le calcul tourne AVANT l'ouverture de la fenetre.
    CalculerModeles();

    const char *envSeule = std::getenv("MASALLE_CALCUL_SEUL");
    if (envSeule != nullptr && std::strcmp(envSeule, "1") == 0) {
        return 0;
    }

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle - un meilleur modele (Echap pour quitter)";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[MaSalleModeles] Creation fenetre KO");
        return 1;
    }

    // Pointeur cache et retenu dans la fenetre : la souris pilote la tete.
    window.ShowMouse(false);
    window.CaptureMouse(true);
    window.ClipMouseToClient(true);

    uint32 largeur = (uint32)window.GetSize().width;
    uint32 hauteur = (uint32)window.GetSize().height;

    // Sur cette machine, DirectX 12 ne prepare pas le nuanceur PBR : on essaie
    // les interfaces une a une et on garde la premiere qui demarre vraiment.
    const NkGraphicsApi ordre[4] = {
        NkGraphicsApi::NK_GFX_API_OPENGL,
        NkGraphicsApi::NK_GFX_API_VULKAN,
        NkGraphicsApi::NK_GFX_API_DX11,
        NkGraphicsApi::NK_GFX_API_DX12,
    };

    NkIDevice *device = nullptr;
    NkRenderer *renderer = nullptr;
    for (int i = 0; i < 4 && renderer == nullptr; ++i) {
        NkDeviceInitInfo devInfo{};
        devInfo.surface = window.GetSurfaceDesc();
        devInfo.width = largeur;
        devInfo.height = hauteur;
        devInfo.api = ordre[i];

        device = NkDeviceFactory::CreateWithFallback(devInfo, {ordre[i]});
        if (device == nullptr || !device->IsValid()) {
            NkDeviceFactory::Destroy(device);
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
    }

    if (renderer == nullptr) {
        logger.Error("[MaSalleModeles] Aucune interface graphique ne demarre le renderer");
        window.ClipMouseToClient(false);
        window.CaptureMouse(false);
        window.ShowMouse(true);
        window.Close();
        return 2;
    }

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
    tete.PoserTete(0.f, 0.f, {0.30f, 1.70f, 1.20f});

    while (enMarche && window.IsOpen()) {
        // Les evenements de cette image remplissent les accumulateurs.
        evenements.PollEvents();
        if (!enMarche) {
            break;
        }

        // On LIT le total de l'image, PUIS on le vide.
        tete.lacet += (float32)accumVideX * SENSIBILITE;
        tete.tangage += (float32)accumVideY * SENSIBILITE;
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

        if (!renderer->BeginFrame()) {
            continue;
        }

        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)largeur / (float32)hauteur;
        camData.nearPlane = 0.05f;
        camData.farPlane = 50.f;
        NkCamera3D cam(camData);
        // La camera suit la tete : cible = position + Regard().
        const NkVec3f regard = tete.Regard();
        cam.SetPosition(tete.position);
        cam.SetTarget({tete.position.x + regard.x, tete.position.y + regard.y, tete.position.z + regard.z});

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
