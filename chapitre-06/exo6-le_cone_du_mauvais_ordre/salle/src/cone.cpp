// cone.cpp — chapitre 06, exercice 6 : le cone du mauvais ordre.
//
// La camera de la salle suit un quaternion, regard ET haut. L'orientation
// est construite dans les deux ordres de composition :
//   qLacet * qTangage  (l'ordre sans defaut : le tangage est applique en premier,
//                       dans le repere de la tete, puis la tete tourne)
//   qTangage * qLacet  (l'ordre fautif : le lacet d'abord, puis un tangage
//                       autour de l'axe x du MONDE)
// La souris ne pilote que le lacet ; le tangage est fixe a 30 degres.
//
// Touches : Espace passe d'un ordre a l'autre ; Echap ferme la salle.
//
// Variables d'environnement :
//   MASALLE_LACET=<degres>   pose figee : donne le lacet et coupe la souris
//   MASALLE_ORDRE=mauvais    demarre dans l'ordre fautif (sinon : l'ordre sans defaut)
//   MASALLE_SHOT=<n>         capture l'image n puis ferme la salle
//   MASALLE_SHOT_PATH=<nom>  nom du fichier PNG de la capture
//
// Au demarrage, avant d'ouvrir la fenetre, le programme imprime la hauteur
// du regard (angle au-dessus de l'horizon) aux lacets 0, 45, 90, 135 et 180
// dans les deux ordres, et ecrit les dix lignes dans hauteurs_sortie.txt.

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
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Offscreen/NkOffscreenTarget.h"
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
    d.appName = "MaSalleCone";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

// Sensibilite en radians par compte du capteur.
static const float32 SENSIBILITE = 0.002f;
static const float32 DEGRES_PAR_RADIAN = 57.2957795f;

// Le tangage est fixe : la souris ne pilote que le lacet.
static const float32 TANGAGE_DEG = 30.f;

// ── L'orientation, dans les deux ordres ─────────────────────────────────
// Le moteur compose comme les matrices : dans q1 * q2, q2 est applique en
// premier (verifie a l'exercice 5).
//   qLacet * qTangage : on leve d'abord les yeux, puis la tete tourne autour
//     de la verticale du monde. Le regard garde sa hauteur de 30 degres.
//   qTangage * qLacet : la tete tourne d'abord, puis tout bascule de 30
//     degres autour de l'axe x du monde, qui ne suit pas la tete.
static NkQuatf Orientation(float32 lacetDeg, bool ordreFautif) {
    const NkQuatf qLacet = NkQuatf::RotateY(NkAngle(lacetDeg));
    const NkQuatf qTangage = NkQuatf::RotateX(NkAngle(TANGAGE_DEG));
    if (ordreFautif) {
        return qTangage * qLacet;
    }
    return qLacet * qTangage;
}

// Hauteur du regard : angle au-dessus de l'horizon, en degres.
static float32 HauteurRegard(const NkQuatf &q) {
    const NkVec3f regard = q * NkVec3f(0.f, 0.f, -1.f);
    float32 y = regard.y;
    if (y > 1.f) {
        y = 1.f;
    }
    if (y < -1.f) {
        y = -1.f;
    }
    return std::asin(y) * DEGRES_PAR_RADIAN;
}

// Imprime les dix hauteurs (cinq lacets, deux ordres), avant la fenetre.
static void ImprimerHauteurs() {
    const float32 lacets[5] = {0.f, 45.f, 90.f, 135.f, 180.f};
    const char *noms[2] = {"lacet*tangage", "tangage*lacet"};

    // printf, et la meme sortie dans un fichier : une application a fenetre
    // n'a pas de console en Release.
    std::FILE *sortie = std::fopen("hauteurs_sortie.txt", "w");
    for (int ordre = 0; ordre < 2; ++ordre) {
        for (int i = 0; i < 5; ++i) {
            const float32 h = HauteurRegard(Orientation(lacets[i], ordre == 1));
            // Evite d'imprimer "-0.0" quand la hauteur est nulle.
            const double affichee = std::fabs(h) < 0.05f ? 0.0 : (double)h;
            char ligne[96];
            std::snprintf(ligne, sizeof(ligne), "%s, lacet %d : hauteur %.1f deg",
                          noms[ordre], (int)lacets[i], affichee);
            std::printf("%s\n", ligne);
            if (sortie != nullptr) {
                std::fprintf(sortie, "%s\n", ligne);
            }
        }
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

// Positions en metres. Un plafond est ajoute pour voir ou monte le regard.
static const Piece PIECES[] = {
    {{0.f, -0.05f, 0.f}, {6.f, 0.1f, 4.f}, {0.35f, 0.33f, 0.30f}},          // sol
    {{0.f, 2.85f, 0.f}, {6.2f, 0.1f, 4.2f}, {0.95f, 0.95f, 0.92f}},         // plafond
    {{0.f, 1.4f, -2.05f}, {6.2f, 2.8f, 0.1f}, {0.85f, 0.83f, 0.78f}},       // mur du fond
    {{0.f, 1.4f, 2.05f}, {6.2f, 2.8f, 0.1f}, {0.85f, 0.83f, 0.78f}},        // mur de l'entree
    {{-3.05f, 1.4f, 0.f}, {0.1f, 2.8f, 4.f}, {0.85f, 0.83f, 0.78f}},        // mur gauche
    {{3.05f, 1.4f, 0.f}, {0.1f, 2.8f, 4.f}, {0.85f, 0.83f, 0.78f}},         // mur droit
    {{-1.5f, 1.f, 1.98f}, {0.9f, 2.f, 0.04f}, {0.45f, 0.28f, 0.15f}},       // porte
    {{-1.5f, 1.5f, -1.98f}, {1.5f, 1.2f, 0.04f}, {0.55f, 0.75f, 0.90f}},    // fenetre du fond
    {{2.98f, 1.5f, 0.5f}, {0.04f, 1.2f, 1.5f}, {0.55f, 0.75f, 0.90f}},      // fenetre de droite
    {{-2.98f, 1.4f, 0.f}, {0.04f, 0.9f, 1.4f}, {0.30f, 0.50f, 0.35f}},      // tableau, mur gauche
    {{0.5f, 0.78f, 0.f}, {1.2f, 0.04f, 0.8f}, {0.60f, 0.42f, 0.25f}},       // plateau
    {{-0.04f, 0.38f, -0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}}, // pied
    {{1.04f, 0.38f, -0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}},  // pied
    {{-0.04f, 0.38f, 0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}},  // pied
    {{1.04f, 0.38f, 0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}},   // pied
    {{0.4f, 0.81f, 0.f}, {0.34f, 0.02f, 0.24f}, {0.15f, 0.15f, 0.17f}},     // ordinateur
    {{0.9f, 1.025f, -0.2f}, {0.15f, 0.45f, 0.15f}, {0.95f, 0.85f, 0.40f}},  // lampe
};

static NkString TitreFenetre(bool ordreFautif) {
    return ordreFautif
        ? NkString("Ma salle - tangage*lacet (ordre fautif) - Espace : changer, Echap : quitter")
        : NkString("Ma salle - lacet*tangage (sans defaut) - Espace : changer, Echap : quitter");
}

int nkmain(const NkEntryState &) {
    ImprimerHauteurs();

    // ── Reglages par l'environnement ──
    const char *envLacet = std::getenv("MASALLE_LACET");
    const char *envOrdre = std::getenv("MASALLE_ORDRE");
    const char *envShot = std::getenv("MASALLE_SHOT");
    const char *envShotPath = std::getenv("MASALLE_SHOT_PATH");

    const bool poseFigee = (envLacet != nullptr && envLacet[0] != '\0');
    float32 lacetDeg = poseFigee ? (float32)std::atof(envLacet) : 0.f;
    bool ordreFautif = (envOrdre != nullptr && std::strcmp(envOrdre, "mauvais") == 0);
    const int imageCapture = (envShot != nullptr) ? std::atoi(envShot) : -1;
    const char *fichierCapture = (envShotPath != nullptr && envShotPath[0] != '\0') ? envShotPath : "capture.png";

    NkWindowConfig winCfg;
    winCfg.title = TitreFenetre(ordreFautif);
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[MaSalleCone] Creation fenetre KO");
        return 1;
    }

    // Souris active : pointeur cache et retenu dans la fenetre.
    if (!poseFigee) {
        window.ShowMouse(false);
        window.CaptureMouse(true);
        window.ClipMouseToClient(true);
    }

    uint32 largeur = (uint32)window.GetSize().width;
    uint32 hauteur = (uint32)window.GetSize().height;

    // Sur cette machine, DirectX 12 ne prepare pas le nuanceur PBR : on essaie
    // les interfaces une a une et on garde la premiere qui demarre vraiment.
    const NkGraphicsApi ordreApi[4] = {
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
        devInfo.api = ordreApi[i];

        device = NkDeviceFactory::CreateWithFallback(devInfo, {ordreApi[i]});
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
        logger.Error("[MaSalleCone] Aucune interface graphique ne demarre le renderer");
        window.ClipMouseToClient(false);
        window.CaptureMouse(false);
        window.ShowMouse(true);
        window.Close();
        return 2;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle cube = renderer->GetMeshSystem()->GetCube();

    bool enMarche = true;
    bool changerOrdre = false;
    NkEventSystem &evenements = NkEvents();
    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) {
        enMarche = false;
    });
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) {
            enMarche = false;
        } else if (e->GetKey() == NkKey::NK_SPACE) {
            changerOrdre = true;
        }
    });

    // Les mouvements bruts sont ACCUMULES ici ; la boucle lit le total une
    // fois par image, puis le vide (exercice 3).
    int32 accumX = 0;
    evenements.AddEventCallback<NkMouseRawEvent>([&](NkMouseRawEvent *e) {
        accumX += e->GetDeltaX();
    });
    evenements.AddEventCallback<NkWindowFocusGainedEvent>([&](NkWindowFocusGainedEvent *) {
        // Windows relache le confinement quand le premier plan change.
        if (!poseFigee) {
            window.ClipMouseToClient(true);
        }
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

    // La capture : quelques images avant, on redirige le rendu vers une cible
    // hors ecran lisible ; a l'image demandee, on l'enregistre puis on ferme.
    NkOffscreenTarget cible;
    bool cibleArmee = false;
    bool captureOk = false;
    const int imageArmee = imageCapture - 3;

    const NkVec3f positionTete = {-0.6f, 1.6f, 0.6f};

    for (int image = 0; enMarche && window.IsOpen(); ++image) {
        evenements.PollEvents();
        if (!enMarche) {
            break;
        }

        if (changerOrdre) {
            changerOrdre = false;
            ordreFautif = !ordreFautif;
            window.SetTitle(TitreFenetre(ordreFautif));
        }

        // On LIT le total de l'image, PUIS on le vide. La pose figee coupe la souris.
        if (!poseFigee) {
            // Souris vers la droite -> la tete tourne a droite (lacet negatif).
            lacetDeg -= (float32)accumX * SENSIBILITE * DEGRES_PAR_RADIAN;
        }
        accumX = 0;

        if (imageCapture > 0 && image == imageArmee) {
            NkOffscreenDesc od;
            od.width = largeur;
            od.height = hauteur;
            od.hasDepth = false;
            od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
            od.readback = true;
            od.name = "ma_salle_capture";
            if (cible.Init(device, renderer->GetTextures(), od)) {
                renderer->SetFinalColorTarget(renderer->GetTextures()->GetRHIHandle(cible.GetColorResult()));
                cibleArmee = true;
            } else {
                logger.Error("[MaSalleCone] Cible hors ecran KO");
                break;
            }
        }

        if (imageCapture > 0 && image == imageCapture) {
            if (cibleArmee) {
                device->WaitIdle();
                captureOk = cible.Capture(fichierCapture);
                logger.Info("[MaSalleCone] {0} : {1}", fichierCapture, captureOk ? "OK" : "ECHEC");
                renderer->SetFinalColorTarget(NkTextureHandle{});
                cible.Shutdown();
            }
            break;
        }

        if (!renderer->BeginFrame()) {
            continue;
        }

        // ── La camera suit le quaternion : regard ET haut viennent de lui ──
        const NkQuatf q = Orientation(lacetDeg, ordreFautif);
        NkCamera3DData cd;
        cd.position = positionTete;
        cd.target = cd.position + q * NkVec3f(0.f, 0.f, -1.f);
        cd.up = q * NkVec3f(0.f, 1.f, 0.f);
        cd.fovY = 70.f;
        cd.aspect = (float32)largeur / (float32)hauteur;
        cd.nearPlane = 0.05f;
        cd.farPlane = 50.f;
        NkCamera3D cam(cd);

        NkSceneContext sctx;
        sctx.camera = cam;

        // Pas d'ombre : le plafond plongerait toute la salle dans la sienne.
        NkLightDesc lumiere;
        lumiere.type = NkLightType::NK_DIRECTIONAL;
        lumiere.direction = {-0.3f, -1.f, -0.4f};
        lumiere.color = {1.f, 0.96f, 0.9f};
        lumiere.intensity = 3.f;
        lumiere.castShadow = false;
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

    if (imageCapture > 0 && !captureOk) {
        return 3;
    }
    return 0;
}
