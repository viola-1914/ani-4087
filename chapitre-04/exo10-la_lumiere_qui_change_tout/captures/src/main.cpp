// MaSalleLumiere — chapitre 04, exercice 10 : la lumiere qui change tout.
//
// Rend la salle avec UN soleil et enregistre l'image en PNG, puis se ferme.
// Argument de lancement : le reglage a montrer.
//   REFERENCE : direction (-0,4 ; -1 ; -0,3), intensite 3, ombres actives.
//               C'est le soleil "salle" de lumiere.cpp.
//   DIRECTION : seule la direction change -> (-0,9 ; -0,3 ; -0,2),
//               le soleil "soir" de lumiere.cpp.
//   INTENSITE : seule l'intensite change -> 1 au lieu de 3.
//   OMBRE     : seul castShadow change -> false.
// Chaque capture ne change donc qu'UN reglage par rapport a la reference.
//
// L'image est ecrite dans <reglage>.png, en minuscules, dans le dossier
// courant (ex. direction.png). capturer.ps1 lance les quatre.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKLogger/NkLog.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Offscreen/NkOffscreenTarget.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include <cstring>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "MaSalleLumiere";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

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

// Le soleil d'une capture.
struct Soleil {
    NkVec3f direction;
    float32 intensite;
    bool ombre;
    const char *fichier;
};

// Lit le reglage demande. Par defaut : la reference.
static Soleil SoleilDemande(const NkEntryState &state) {
    Soleil soleil = {{-0.4f, -1.f, -0.3f}, 3.f, true, "reference.png"};
    for (const NkString &argument : state.args) {
        const char *texte = argument.CStr();
        if (std::strcmp(texte, "DIRECTION") == 0) {
            soleil.direction = {-0.9f, -0.3f, -0.2f};
            soleil.fichier = "direction.png";
        } else if (std::strcmp(texte, "INTENSITE") == 0) {
            soleil.intensite = 1.f;
            soleil.fichier = "intensite.png";
        } else if (std::strcmp(texte, "OMBRE") == 0) {
            soleil.ombre = false;
            soleil.fichier = "ombre.png";
        }
    }
    return soleil;
}

int nkmain(const NkEntryState &state) {
    const Soleil soleil = SoleilDemande(state);

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle - capture de la lumiere";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[MaSalleLumiere] Creation fenetre KO");
        return 1;
    }

    const uint32 largeur = (uint32)window.GetSize().width;
    const uint32 hauteur = (uint32)window.GetSize().height;

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
        logger.Error("[MaSalleLumiere] Aucune interface graphique ne demarre le renderer");
        window.Close();
        return 2;
    }
    logger.Info("[MaSalleLumiere] Interface graphique retenue : {0}", NkGraphicsApiName(device->GetApi()));

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle cube = renderer->GetMeshSystem()->GetCube();

    // On laisse passer quelques images (ombres et ressources en place), on
    // redirige les dernieres vers une cible hors ecran, puis on enregistre.
    const int imageArmee = 27;
    const int imageCapturee = 30;
    NkOffscreenTarget cible;
    bool cibleArmee = false;
    bool captureOk = false;

    for (int image = 0; image <= imageCapturee; ++image) {
        NkEvents().PollEvents();

        if (image == imageArmee) {
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
                logger.Error("[MaSalleLumiere] Cible hors ecran KO");
                break;
            }
        }

        if (image == imageCapturee) {
            if (cibleArmee) {
                device->WaitIdle();
                captureOk = cible.Capture(soleil.fichier);
                logger.Info("[MaSalleLumiere] {0} : {1}", soleil.fichier, captureOk ? "OK" : "ECHEC");
                renderer->SetFinalColorTarget(NkTextureHandle{});
                cible.Shutdown();
            }
            break;
        }

        if (!renderer->BeginFrame()) {
            continue;
        }

        // Meme camera fixe pour les quatre captures : seule la lumiere change.
        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)largeur / (float32)hauteur;
        camData.nearPlane = 0.05f;
        camData.farPlane = 50.f;
        NkCamera3D cam(camData);
        cam.SetPosition({-2.2f, 1.7f, 1.7f});
        cam.SetTarget({0.5f, 0.8f, 0.f});

        NkSceneContext sctx;
        sctx.camera = cam;

        NkLightDesc lumiere;
        lumiere.type = NkLightType::NK_DIRECTIONAL;
        lumiere.direction = soleil.direction;
        lumiere.color = {1.f, 0.96f, 0.9f};
        lumiere.intensity = soleil.intensite;
        lumiere.castShadow = soleil.ombre;
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

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    if (!captureOk) {
        return 3;
    }
    return 0;
}
