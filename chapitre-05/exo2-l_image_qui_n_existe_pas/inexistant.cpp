// Exercice 2 : echec de chargement image et texture, capture de la salle.
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

#include <cstdlib>
#include <iostream>
#include "NKImage/Core/NkImage.h"
#include "NKRenderer/Materials/NkMaterial.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "ImageAbsente";
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

int nkmain(const NkEntryState &) {
    // Partie NkImage : quatre lignes, sans acces aux pixels apres un echec.
    NkImage imageAbsente;
    const bool charge = imageAbsente.Load("murs/nexistepas.png");
    std::cout << "charge : " << charge << ", valide : " << imageAbsente.IsValid()
              << ", largeur : " << imageAbsente.Width() << ", hauteur : " << imageAbsente.Height() << std::endl;

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle - image absente";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[ImageAbsente] Creation fenetre KO");
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
        renderer = nullptr;
        NkDeviceFactory::Destroy(device);
        device = nullptr;
    }

    if (renderer == nullptr) {
        logger.Error("[ImageAbsente] Aucune interface graphique ne demarre le renderer");
        window.Close();
        return 2;
    }
    logger.Info("[ImageAbsente] Interface graphique retenue : {0}", NkGraphicsApiName(device->GetApi()));

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle cube = renderer->GetMeshSystem()->GetCube();

    const auto texture = renderer->GetTextures()->Load("murs/nexistepas.png");
    NkMaterial* mat = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    if (mat == nullptr) {
        logger.Error("[ImageAbsente] Creation du materiau impossible");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 4;
    }
    mat->SetAlbedoMap(texture);


    // On laisse passer quelques images (ombres et ressources en place), on
    // redirige les dernieres vers une cible hors ecran, puis on enregistre.
    const char* numero = std::getenv("MASALLE_SHOT");
    const int demande = numero ? std::atoi(numero) : 120;
    const int imageCapturee = demande >= 4 ? demande : 120;
    const int imageArmee = imageCapturee - 3;
    const char* cheminCapture = std::getenv("MASALLE_SHOT_PATH");
    if (!cheminCapture || !*cheminCapture) cheminCapture = "mur-absent.png";
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
                logger.Error("[ImageAbsente] Cible hors ecran KO");
                break;
            }
        }

        if (image == imageCapturee) {
            if (cibleArmee) {
                device->WaitIdle();
                captureOk = cible.Capture(cheminCapture);
                logger.Info("[ImageAbsente] {0} : {1}", cheminCapture, captureOk ? "OK" : "ECHEC");
                renderer->SetFinalColorTarget(NkTextureHandle{});
                cible.Shutdown();
                cibleArmee = false;
            }
            break;
        }

        if (!renderer->BeginFrame()) {
            continue;
        }

        // Camera fixe orientee vers le mur du fond.
        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)largeur / (float32)hauteur;
        camData.nearPlane = 0.05f;
        camData.farPlane = 50.f;
        NkCamera3D cam(camData);
        cam.SetPosition({-2.2f, 1.7f, 1.7f});
        cam.SetTarget({0.5f, 1.4f, -2.05f});

        NkSceneContext sctx;
        sctx.camera = cam;

        NkLightDesc lumiere;
        lumiere.type = NkLightType::NK_DIRECTIONAL;
        lumiere.direction = NkVec3f{-0.4f, -1.f, -0.3f};
        lumiere.color = {1.f, 0.96f, 0.9f};
        lumiere.intensity = 3.f;
        lumiere.castShadow = true;
        sctx.lights.PushBack(lumiere);

        r3d->BeginScene(sctx);
        for (unsigned i = 0; i < sizeof(PIECES) / sizeof(PIECES[0]); ++i) {
            const Piece &piece = PIECES[i];
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
            if (i == 1) {
                // Seul le mur du fond recoit le materiau teste.
                dc.material = mat->GetInstHandle();
                dc.tint = {1.f, 1.f, 1.f};
            }
            r3d->Submit(dc);
        }

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    if (cibleArmee && !captureOk) {
        renderer->SetFinalColorTarget(NkTextureHandle{});
        cible.Shutdown();
    }
    NkMaterial::Destroy(mat);
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    if (!captureOk) {
        return 3;
    }
    return 0;
}
