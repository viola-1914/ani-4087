// chaine-trois.cpp — chapitre 08, exercice 10 : la chaine a trois images.
//
// La salle est rendue dans une VRAIE chaine d'echange NKXR
// (nkentseu::xr::NkXrSwapchain), avec la discipline du chapitre :
//   AcquireImage -> dessiner dans l'image empruntee -> ReleaseImage
//   -> le compositeur (ici le renderer de l'ecran) lit
//      GetLastReleasedIndex() et la pose dans la fenetre.
//
// Chaque image de la chaine est une cible hors ecran de 640 x 720, avec son
// propre renderer de salle (patron du chapitre 6, exercice 9). La tete tourne
// de 2 degres par trame pour que les images se distinguent.
//
// Variable d'environnement MASALLE_IMAGES : nombre d'images de la chaine
// (1 a 4, defaut 3). Le programme :
//   - ecrit les 30 premieres trames dans chaine_sortie.txt :
//       trame u : image acquise i, image soumise j
//   - mesure ensuite 300 trames (duree moyenne et max d'une trame, en ms)
//     et ajoute cette ligne a mesure_sortie.txt ;
//   - se ferme seul.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKLogger/NkLog.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Core/NkRenderGraph.h"
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Materials/NkMaterialCollection.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Offscreen/NkOffscreenTarget.h"
#include "NKRenderer/Tools/Render2D/NkRender2D.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "NKMath/NkMat.h"
#include "NKMath/NkQuat.h"

// La chaine d'echange du moteur (copiee depuis NKXR par mesurer.ps1).
#include "NKXR/NkXrSwapchain.h"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "MaSalleTrois";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static const uint32 LARGEUR = 640;
static const uint32 HAUTEUR = 720;
static const int TRAMES_JOURNAL = 30;
static const int TRAMES_MESURE = 300;
static const NkVec3f POSITION_TETE = {0.30f, 1.70f, 1.20f};

struct Piece {
    NkVec3f centre;
    NkVec3f taille;
    NkVec3f couleur;
};

static const Piece PIECES[] = {
    {{0.f, -0.05f, 0.f}, {6.f, 0.1f, 4.f}, {0.35f, 0.33f, 0.30f}},
    {{0.f, 1.4f, -2.05f}, {6.2f, 2.8f, 0.1f}, {0.85f, 0.83f, 0.78f}},
    {{0.f, 1.4f, 2.05f}, {6.2f, 2.8f, 0.1f}, {0.85f, 0.83f, 0.78f}},
    {{-3.05f, 1.4f, 0.f}, {0.1f, 2.8f, 4.f}, {0.85f, 0.83f, 0.78f}},
    {{3.05f, 1.4f, 0.f}, {0.1f, 2.8f, 4.f}, {0.85f, 0.83f, 0.78f}},
    {{-1.5f, 1.f, 1.98f}, {0.9f, 2.f, 0.04f}, {0.45f, 0.28f, 0.15f}},
    {{-1.5f, 1.5f, -1.98f}, {1.5f, 1.2f, 0.04f}, {0.55f, 0.75f, 0.90f}},
    {{2.98f, 1.5f, 0.5f}, {0.04f, 1.2f, 1.5f}, {0.55f, 0.75f, 0.90f}},
    {{0.5f, 0.78f, 0.f}, {1.2f, 0.04f, 0.8f}, {0.60f, 0.42f, 0.25f}},
    {{-0.04f, 0.38f, -0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}},
    {{1.04f, 0.38f, -0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}},
    {{-0.04f, 0.38f, 0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}},
    {{1.04f, 0.38f, 0.34f}, {0.06f, 0.76f, 0.06f}, {0.30f, 0.20f, 0.12f}},
    {{0.4f, 0.81f, 0.f}, {0.34f, 0.02f, 0.24f}, {0.15f, 0.15f, 0.17f}},
    {{0.9f, 1.025f, -0.2f}, {0.15f, 0.45f, 0.15f}, {0.95f, 0.85f, 0.40f}},
};

static void SoumettreSalle(NkRender3D *r3d, NkMeshHandle cube, float32 lacetDeg) {
    const float32 a = lacetDeg * 3.14159265f / 180.f;
    NkCamera3DData cd;
    cd.position = POSITION_TETE;
    cd.target = POSITION_TETE + NkVec3f(-std::sin(a), 0.f, -std::cos(a));
    cd.up = {0.f, 1.f, 0.f};
    cd.fovY = 70.f;
    cd.aspect = (float32)LARGEUR / (float32)HAUTEUR;
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
    for (const Piece &p : PIECES) {
        NkDrawCall3D dc;
        dc.mesh = cube;
        dc.transform = NkMat4f::Translate(p.centre) * NkMat4f::Scale(p.taille);
        dc.aabb = {{p.centre.x - p.taille.x * 0.5f, p.centre.y - p.taille.y * 0.5f, p.centre.z - p.taille.z * 0.5f},
                   {p.centre.x + p.taille.x * 0.5f, p.centre.y + p.taille.y * 0.5f, p.centre.z + p.taille.z * 0.5f}};
        dc.tint = p.couleur;
        dc.roughness = 0.8f;
        r3d->Submit(dc);
    }
}

static void Dessiner(NkRenderer *r, NkICommandBuffer *cmd, float32 lacetDeg) {
    r->FlushGraphRebuilds();
    r->GetRender3D()->ResetFrame();
    if (r->GetMaterialCollection() != nullptr) {
        r->GetMaterialCollection()->Upload();
    }
    SoumettreSalle(r->GetRender3D(), r->GetMeshSystem()->GetCube(), lacetDeg);
    r->GetRenderGraph()->Execute(cmd);
}

int nkmain(const NkEntryState &) {
    uint32 nbImages = 3;
    if (const char *v = std::getenv("MASALLE_IMAGES")) {
        const int n = std::atoi(v);
        if (n >= 1 && n <= 4) {
            nbImages = (uint32)n;
        }
    }

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle - chaine d'echange";
    winCfg.width = LARGEUR;
    winCfg.height = HAUTEUR;
    winCfg.centered = true;
    winCfg.resizable = false;
    NkWindow window(winCfg);
    if (!window.IsValid()) {
        return 1;
    }
    const uint32 lw = (uint32)window.GetSize().width;
    const uint32 lh = (uint32)window.GetSize().height;

    const NkGraphicsApi ordreApi[4] = {
        NkGraphicsApi::NK_GFX_API_OPENGL, NkGraphicsApi::NK_GFX_API_VULKAN,
        NkGraphicsApi::NK_GFX_API_DX11, NkGraphicsApi::NK_GFX_API_DX12,
    };
    NkIDevice *device = nullptr;
    NkRenderer *ecran = nullptr;
    for (int i = 0; i < 4 && ecran == nullptr; ++i) {
        NkDeviceInitInfo devInfo{};
        devInfo.surface = window.GetSurfaceDesc();
        devInfo.width = lw;
        devInfo.height = lh;
        devInfo.api = ordreApi[i];
        device = NkDeviceFactory::CreateWithFallback(devInfo, {ordreApi[i]});
        if (device == nullptr || !device->IsValid()) {
            NkDeviceFactory::Destroy(device);
            continue;
        }
        NkRendererConfig cfg;
        cfg.api = device->GetApi();
        cfg.width = lw;
        cfg.height = lh;
        cfg.subsystems = NK_SS_RENDER2D;  // le compositeur : pose l'image soumise
        ecran = NkRenderer::Create(device, cfg);
        if (ecran != nullptr && ecran->Initialize()) {
            break;
        }
        NkRenderer::Destroy(ecran);
        ecran = nullptr;
        NkDeviceFactory::Destroy(device);
    }
    if (ecran == nullptr) {
        logger.Error("[MaSalleTrois] Aucune interface graphique ne demarre");
        return 2;
    }

    // ── La chaine d'echange NKXR ──
    xr::NkXrSwapchainDesc scDesc;
    scDesc.width = LARGEUR;
    scDesc.height = HAUTEUR;
    scDesc.imageCount = nbImages;
    scDesc.eye = xr::NkXrEye::NK_XR_EYE_LEFT;
    scDesc.name = "salle";
    xr::NkXrSwapchain chaine;
    if (!chaine.Init(scDesc)) {
        logger.Error("[MaSalleTrois] Chaine refusee");
        return 3;
    }

    // ── Une cible + un renderer de salle par image de la chaine ──
    NkOffscreenTarget *cibles[xr::NkXrSwapchain::kMaxImages] = {};
    NkRenderer *rendus[xr::NkXrSwapchain::kMaxImages] = {};
    for (uint32 i = 0; i < nbImages; ++i) {
        NkOffscreenDesc od;
        od.width = LARGEUR;
        od.height = HAUTEUR;
        od.hasDepth = false;
        od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
        od.readable = true;
        od.readback = false;
        od.name = "image_chaine";
        cibles[i] = ecran->CreateOffscreen(od);
        if (cibles[i] == nullptr) {
            return 4;
        }
        const auto rhi = ecran->GetTextures()->GetRHIHandle(cibles[i]->GetColorResult());
        chaine.SetImageNative(i, (uint64)rhi.id);

        NkRendererConfig cfg;
        cfg.api = device->GetApi();
        cfg.width = LARGEUR;
        cfg.height = HAUTEUR;
        cfg.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;
        rendus[i] = NkRenderer::Create(device, cfg);
        if (rendus[i] == nullptr || !rendus[i]->Initialize()) {
            return 5;
        }
        rendus[i]->SetFinalColorTarget(rhi);
        rendus[i]->SetRenderSizeOverride(LARGEUR, HAUTEUR);
    }

    NkRender2D *r2d = ecran->GetRender2D();
    const bool cibleRetournee = NkOffscreenStoredIsBottomUp(device->GetApi());

    bool enMarche = true;
    NkEventSystem &evenements = NkEvents();
    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { enMarche = false; });
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) {
            enMarche = false;
        }
    });

    std::FILE *journal = std::fopen("chaine_sortie.txt", "w");
    int refus = 0;
    double somme = 0.0;
    double maxi = 0.0;
    int mesurees = 0;
    auto avant = std::chrono::steady_clock::now();

    const int total = TRAMES_JOURNAL + TRAMES_MESURE;
    for (int u = 1; u <= total && enMarche && window.IsOpen(); ++u) {
        evenements.PollEvents();
        if (!ecran->BeginFrame()) {
            --u;
            continue;
        }
        NkICommandBuffer *cmd = ecran->GetCmd();

        // 1) Emprunter
        uint32 acquise = 0;
        if (!chaine.AcquireImage(acquise)) {
            ++refus;
        }
        // 2) Dessiner dans l'image empruntee
        Dessiner(rendus[acquise], cmd, 2.f * (float32)u);
        // 3) Rendre
        if (!chaine.ReleaseImage()) {
            ++refus;
        }
        // 4) Soumettre : le compositeur lit la derniere image relachee
        const uint32 soumise = chaine.GetLastReleasedIndex();
        r2d->Begin(cmd, lw, lh);
        if (cibleRetournee) {
            r2d->DrawOffscreen({0.f, 0.f, (float32)lw, (float32)lh}, cibles[soumise]->GetColorResult());
        } else {
            r2d->DrawImage(cibles[soumise]->GetColorResult(), {0.f, 0.f, (float32)lw, (float32)lh});
        }
        r2d->End();
        ecran->Present();
        ecran->EndFrame();

        if (u <= TRAMES_JOURNAL) {
            if (journal != nullptr) {
                std::fprintf(journal, "trame %d : image acquise %u, image soumise %u\n", u, acquise, soumise);
            }
        }
        const auto maintenant = std::chrono::steady_clock::now();
        const double ms = std::chrono::duration<double, std::milli>(maintenant - avant).count();
        avant = maintenant;
        if (u > TRAMES_JOURNAL) {
            somme += ms;
            if (ms > maxi) {
                maxi = ms;
            }
            ++mesurees;
        }
    }
    if (journal != nullptr) {
        std::fclose(journal);
    }

    std::FILE *mesure = std::fopen("mesure_sortie.txt", "a");
    if (mesure != nullptr && mesurees > 0) {
        std::fprintf(mesure, "%u image(s) : %d trames, moyenne %.3f ms, max %.3f ms, refus %d\n",
                     nbImages, mesurees, somme / mesurees, maxi, refus);
        std::fclose(mesure);
    }

    device->WaitIdle();
    for (uint32 i = 0; i < nbImages; ++i) {
        NkRenderer::Destroy(rendus[i]);
        ecran->DestroyOffscreen(cibles[i]);
    }
    NkRenderer::Destroy(ecran);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}
