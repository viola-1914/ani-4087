// cout.cpp — chapitre 06, exercice 7 : le cout mesure.
//
// Au demarrage, avant d'ouvrir la fenetre, on chronometre sur un million
// d'iterations les deux facons de construire l'orientation de la tete :
//   - matrices : NkMat4f::RotationY * NkMat4f::RotationX, puis conversion
//     en quaternion (le chemin du simulateur) ;
//   - quaternions : NkQuatf::RotateY * NkQuatf::RotateX, le produit direct.
// Chaque version est mesuree trois fois. Les resultats sont imprimes avec
// std::printf et ecrits dans cout_sortie.txt (une application a fenetre n'a
// pas de console en Release). Puis la salle s'ouvre ; Echap ferme.
//
// Variables d'environnement (posees par mesurer.ps1) :
//   MASALLE_CONFIG=Debug|Release  nom de la configuration, en tete des lignes
//   MASALLE_MESURE_SEULE=1        mesure puis quitte, sans ouvrir la salle

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkMouseEvent.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKLogger/NkLog.h"
#include "NKTime/NkChrono.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "NKMath/NkAngle.h"
#include "NKMath/NkMat.h"
#include "NKMath/NkQuat.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "MaSalle";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

// Sensibilite en radians par compte du capteur.
static const float32 SENSIBILITE = 0.002f;

static const float32 DEGRES_PAR_RADIAN = 57.2957795f;

static const int ITERATIONS = 1000000;

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

// ── Les deux constructions ──────────────────────────────────────────────
// Celle du simulateur : deux matrices, un produit, une conversion.
static NkQuatf ParMatrices(float32 lacetDeg, float32 tangageDeg) {
    const NkMat4f rotation = NkMat4f::RotationY(NkAngle(lacetDeg)) *
                             NkMat4f::RotationX(NkAngle(tangageDeg));
    const NkQuatf q = NkQuatf(rotation).Normalized();
    return q;
}

// La directe : le produit des deux quaternions, dans le meme ordre que les
// matrices (lacet * tangage : le tangage est applique en premier, exercice 5).
static NkQuatf ParQuaternions(float32 lacetDeg, float32 tangageDeg) {
    const NkQuatf q = (NkQuatf::RotateY(NkAngle(lacetDeg)) *
                       NkQuatf::RotateX(NkAngle(tangageDeg))).Normalized();
    return q;
}

// Les angles changent a chaque tour, pour que rien ne se calcule une fois
// pour toutes hors de la boucle.
static float32 LacetDe(int i) {
    return (float32)(i % 3600) * 0.1f;
}
static float32 TangageDe(int i) {
    return (float32)(i % 1700) * 0.1f - 85.f;
}

// Une boucle d'un million d'iterations. Les quatre composantes de chaque
// resultat s'ajoutent a la somme, imprimee a la fin : sans cette somme lue,
// le compilateur pourrait supprimer la boucle entiere.
static float64 BoucleMatrices(float64 &somme) {
    NkChrono chrono;
    for (int i = 0; i < ITERATIONS; ++i) {
        const NkQuatf q = ParMatrices(LacetDe(i), TangageDe(i));
        somme += (float64)q.x + (float64)q.y + (float64)q.z + (float64)q.w;
    }
    return chrono.Elapsed().ToMilliseconds();
}

static float64 BoucleQuaternions(float64 &somme) {
    NkChrono chrono;
    for (int i = 0; i < ITERATIONS; ++i) {
        const NkQuatf q = ParQuaternions(LacetDe(i), TangageDe(i));
        somme += (float64)q.x + (float64)q.y + (float64)q.z + (float64)q.w;
    }
    return chrono.Elapsed().ToMilliseconds();
}

static void Ecrire(std::FILE *sortie, const char *ligne) {
    std::printf("%s\n", ligne);
    if (sortie != nullptr) {
        std::fprintf(sortie, "%s\n", ligne);
    }
}

static void MesurerCout() {
    const char *envConfig = std::getenv("MASALLE_CONFIG");
    const char *config = (envConfig != nullptr && envConfig[0] != '\0') ? envConfig : "?";

    std::FILE *sortie = std::fopen("cout_sortie.txt", "w");
    char ligne[160];

    // 1. Verification sur un angle : lacet 45, tangage 30.
    //    q et -q donnent la meme rotation : si la conversion rend l'oppose,
    //    on le retourne avant de comparer.
    const NkQuatf qm = ParMatrices(45.f, 30.f);
    NkQuatf qq = ParQuaternions(45.f, 30.f);
    if (qm.Dot(qq) < 0.f) {
        qq = qq * -1.f;
    }
    const float32 ecart = std::fabs(qm.x - qq.x) + std::fabs(qm.y - qq.y) +
                          std::fabs(qm.z - qq.z) + std::fabs(qm.w - qq.w);
    std::snprintf(ligne, sizeof(ligne), "verification, lacet 45, tangage 30 :");
    Ecrire(sortie, ligne);
    std::snprintf(ligne, sizeof(ligne), "  matrices    : (x %.4f, y %.4f, z %.4f, w %.4f)",
                  (double)qm.x, (double)qm.y, (double)qm.z, (double)qm.w);
    Ecrire(sortie, ligne);
    std::snprintf(ligne, sizeof(ligne), "  quaternions : (x %.4f, y %.4f, z %.4f, w %.4f)",
                  (double)qq.x, (double)qq.y, (double)qq.z, (double)qq.w);
    Ecrire(sortie, ligne);
    std::snprintf(ligne, sizeof(ligne), "  ecart = %.6f -> %s", (double)ecart,
                  ecart < 1e-4f ? "meme quaternion" : "DIFFERENTS");
    Ecrire(sortie, ligne);

    // 2. Trois mesures de chaque version, en alternant.
    float64 somme = 0.0;
    for (int essai = 0; essai < 3; ++essai) {
        const float64 tm = BoucleMatrices(somme);
        std::snprintf(ligne, sizeof(ligne), "%s, matrices : %.2f ms", config, tm);
        Ecrire(sortie, ligne);
        const float64 tq = BoucleQuaternions(somme);
        std::snprintf(ligne, sizeof(ligne), "%s, quaternions : %.2f ms", config, tq);
        Ecrire(sortie, ligne);
    }

    // 3. La somme est lue : les boucles ne peuvent pas disparaitre.
    std::snprintf(ligne, sizeof(ligne), "somme de controle : %.3f", somme);
    Ecrire(sortie, ligne);

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
    MesurerCout();

    const char *envSeule = std::getenv("MASALLE_MESURE_SEULE");
    if (envSeule != nullptr && std::strcmp(envSeule, "1") == 0) {
        return 0;
    }

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle - le cout mesure (Echap pour quitter)";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[MaSalle] Creation fenetre KO");
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
        logger.Error("[MaSalle] Aucune interface graphique ne demarre le renderer");
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
    tete.PoserTete(-1.009f, -0.274f, {-2.2f, 1.7f, 1.7f});

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
