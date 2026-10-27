// budget.cpp — chapitre 07, exercice 1 : votre budget calcule.
//
// Au demarrage, avant d'ouvrir la fenetre : le budget d'une image a 72, 90 et
// 120 hertz, en millisecondes (1000 / frequence), imprime avec std::printf.
// Puis la salle tourne : au DEBUT de chaque image, on note l'instant present
// (NkChrono::Now().nanoseconds, horloge qui ne recule jamais). Le temps d'une
// image est l'intervalle entre deux debuts successifs. On en garde mille, en
// ecartant les trente premieres images (elles paient le demarrage), puis on
// imprime la moyenne, la mediane et le maximum.
//
// Variables d'environnement :
//   MASALLE_EXIT=1040  arrete la salle seule apres 1040 images
//   NK_VK_MAILBOX=1    la salle n'attend plus l'ecran ; la ligne s'appelle alors
//                      « sans attente » au lieu d'« ecran ». Sous Vulkan, la variable
//                      choisit le mode MAILBOX ; si la salle tourne sous OpenGL, le
//                      programme pose en plus un intervalle d'echange nul
//                      (wglSwapIntervalEXT(0)), l'equivalent OpenGL.
// Sortie : std::printf et budget_sortie.txt (pas de console en Release).

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
    d.appName = "MaSalleBudget";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

// Sensibilite en radians par compte du capteur.
static const float32 SENSIBILITE = 0.002f;

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

// ── Le budget d'une image ───────────────────────────────────────────────
// Dans un casque, la cadence est celle de l'ecran : le budget d'une image en
// decoule par une division, 1000 ms / frequence.
static void ImprimerBudgets(std::FILE *sortie) {
    const int frequences[3] = {72, 90, 120};
    for (int i = 0; i < 3; ++i) {
        const double budgetMs = 1000.0 / (double)frequences[i];
        char ligne[64];
        std::snprintf(ligne, sizeof(ligne), "budget %d Hz : %.3f ms", frequences[i], budgetMs);
        std::printf("%s\n", ligne);
        if (sortie != nullptr) {
            std::fprintf(sortie, "%s\n", ligne);
        }
    }
    std::fflush(stdout);
}

// Nombre d'images ecartees au demarrage, et nombre d'intervalles mesures.
static const int IMAGES_ECARTEES = 30;
static const int INTERVALLES_MESURES = 1000;

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
    // Les budgets sont calcules AVANT l'ouverture de la fenetre.
    std::FILE *sortie = std::fopen("budget_sortie.txt", "w");
    ImprimerBudgets(sortie);

    // Quelle mesure : avec l'attente de l'ecran (par defaut) ou sans (NK_VK_MAILBOX).
    const char *envMailbox = std::getenv("NK_VK_MAILBOX");
    const bool sansAttente = (envMailbox != nullptr && envMailbox[0] != '\0' && envMailbox[0] != '0');
    const char *nomMesure = sansAttente ? "sans attente" : "ecran";

    // MASALLE_EXIT : la salle s'arrete seule apres ce nombre d'images.
    const char *envExit = std::getenv("MASALLE_EXIT");
    const long long imagesMax = (envExit != nullptr) ? std::atoll(envExit) : 0;

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle - le budget mesure (Echap pour quitter)";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[MaSalleBudget] Creation fenetre KO");
        return 1;
    }

    // Pointeur cache et retenu dans la fenetre : la souris pilote la tete.
    window.ShowMouse(false);
    window.CaptureMouse(true);
    window.ClipMouseToClient(true);

    uint32 largeur = (uint32)window.GetSize().width;
    uint32 hauteur = (uint32)window.GetSize().height;

    // Vulkan d'abord : NK_VK_MAILBOX n'agit que sous Vulkan. Si Vulkan ne
    // demarre pas, on essaie les autres une a une (DirectX 12 ne prepare pas le
    // nuanceur PBR sur cette machine).
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
        // Sous OpenGL, NK_VK_MAILBOX n'agit pas : l'equivalent est un
        // intervalle d'echange nul (wglSwapIntervalEXT(0)), qui ne cale plus
        // la presentation sur le balayage de l'ecran.
        if (sansAttente) {
            devInfo.context.opengl.swapInterval = NkGLSwapInterval::NK_GL_SWAP_IMMEDIATE;
        }

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
        logger.Error("[MaSalleBudget] Aucune interface graphique ne demarre le renderer");
        window.ClipMouseToClient(false);
        window.CaptureMouse(false);
        window.ShowMouse(true);
        window.Close();
        return 2;
    }

    const char *nomApi = NkGraphicsApiName(device->GetApi());
    logger.Info("[MaSalleBudget] api={0}", nomApi);

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

    // Les debuts d'image, en nanosecondes. Il en faut 1001 pour 1000 intervalles.
    std::vector<float64> debuts;
    debuts.reserve(INTERVALLES_MESURES + 1);
    long long image = 0;

    while (enMarche && window.IsOpen()) {
        // ── DEBUT DE L'IMAGE : on note l'instant present ──
        const float64 maintenant = NkChrono::Now().nanoseconds;
        if (image >= IMAGES_ECARTEES && (int)debuts.size() < INTERVALLES_MESURES + 1) {
            debuts.push_back(maintenant);
        }
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


    // ── Les mille intervalles : moyenne, mediane, maximum ──
    std::vector<double> intervalles;
    for (size_t i = 1; i < debuts.size(); ++i) {
        intervalles.push_back((debuts[i] - debuts[i - 1]) / 1.0e6);  // ns -> ms
    }
    char ligne[160];
    if (!intervalles.empty()) {
        double somme = 0.0;
        double maximum = 0.0;
        for (double t : intervalles) {
            somme += t;
            if (t > maximum) {
                maximum = t;
            }
        }
        const double moyenne = somme / (double)intervalles.size();
        std::vector<double> tries = intervalles;
        std::sort(tries.begin(), tries.end());
        const size_t n = tries.size();
        const double mediane = (n % 2 == 1) ? tries[n / 2] : 0.5 * (tries[n / 2 - 1] + tries[n / 2]);

        std::snprintf(ligne, sizeof(ligne), "%s : moyenne %.3f ms, mediane %.3f ms, max %.3f ms", nomMesure,
                      moyenne, mediane, maximum);
        std::printf("%s\n", ligne);
        if (sortie != nullptr) {
            std::fprintf(sortie, "%s\n", ligne);
        }
        // Pour la reponse : combien d'images depassent chaque budget, et la cadence.
        int au72 = 0, au90 = 0, au120 = 0;
        for (double t : intervalles) {
            au72 += (t > 1000.0 / 72.0) ? 1 : 0;
            au90 += (t > 1000.0 / 90.0) ? 1 : 0;
            au120 += (t > 1000.0 / 120.0) ? 1 : 0;
        }
        std::snprintf(ligne, sizeof(ligne),
                      "(info) api=%s, echecs avant : %s, intervalles=%d, cadence mediane %.1f images/s, au-dessus du budget : 72 Hz %d, 90 Hz %d, 120 Hz %d",
                      nomApi, echecs[0] ? echecs : "aucun", (int)n, 1000.0 / mediane, au72, au90, au120);
        std::printf("%s\n", ligne);
        if (sortie != nullptr) {
            std::fprintf(sortie, "%s\n", ligne);
        }
    } else {
        std::snprintf(ligne, sizeof(ligne), "%s : pas assez d'images mesurees", nomMesure);
        std::printf("%s\n", ligne);
        if (sortie != nullptr) {
            std::fprintf(sortie, "%s\n", ligne);
        }
    }
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