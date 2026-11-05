// emprunts-salle.cpp — chapitre 08, exercice 9 : compter les emprunts.
//
// La boucle de rendu de la salle (celle de l'exercice 10 : une vraie
// NkXrSwapchain de 3 images, une cible hors ecran et un renderer par image)
// instrumentee avec trois compteurs :
//   reussies    : AcquireImage a rendu true
//   refusees    : AcquireImage a rendu false
//   liberations : ReleaseImage a rendu true
// Relation attendue a chaque fin de trame : reussies - liberations vaut 0
// (ou 1 tant qu'une image est empruntee, au milieu d'une trame).
//
// La boucle tourne MASALLE_DUREE secondes (60 par defaut). Si MASALLE_FUITE
// vaut k > 0, la trame k prend le CHEMIN QUI FUIT : une sortie anticipee
// entre AcquireImage et ReleaseImage (voir « CHEMIN QUI FUIT » plus bas).
// Le bilan est ajoute a compteurs_sortie.txt.

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
    d.appName = "MaSalleEmprunts";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static const uint32 LARGEUR = 640;
static const uint32 HAUTEUR = 720;
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
    const uint32 nbImages = 3;
    // MASALLE_DUREE : duree de la boucle en secondes (defaut 60).
    double duree = 60.0;
    if (const char *v = std::getenv("MASALLE_DUREE")) {
        const double d = std::atof(v);
        if (d > 0.0) {
            duree = d;
        }
    }
    // MASALLE_FUITE : numero de la trame qui prend le chemin de sortie
    // anticipee (0 = aucune, defaut).
    long long trameFuite = 0;
    if (const char *v = std::getenv("MASALLE_FUITE")) {
        trameFuite = std::atoll(v);
    }

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle - compter les emprunts";
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
        logger.Error("[MaSalleEmprunts] Aucune interface graphique ne demarre");
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
        logger.Error("[MaSalleEmprunts] Chaine refusee");
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

    long long reussies = 0;
    long long refusees = 0;
    long long liberations = 0;
    long long premiereFuite = 0;
    long long premiereBloquee = 0;
    long long dessinees = 0;
    long long trames = 0;

    const auto debut = std::chrono::steady_clock::now();
    for (long long u = 1; enMarche && window.IsOpen(); ++u) {
        const double ecoule = std::chrono::duration<double>(std::chrono::steady_clock::now() - debut).count();
        if (ecoule >= duree) {
            break;
        }
        evenements.PollEvents();
        if (!ecran->BeginFrame()) {
            --u;
            continue;
        }
        trames = u;
        NkICommandBuffer *cmd = ecran->GetCmd();

        // ── Emprunter ──
        uint32 acquise = 0;
        if (!chaine.AcquireImage(acquise)) {
            ++refusees;                     // rien emprunte : rien a dessiner, rien a rendre
            if (premiereBloquee == 0) {
                premiereBloquee = u;
            }
        } else {
            ++reussies;
            if (trameFuite > 0 && u == trameFuite) {
                // ── CHEMIN QUI FUIT ──
                // Une sortie anticipee entre AcquireImage et ReleaseImage :
                // par exemple « l'oeil n'est pas pret, on saute le dessin ».
                // On quitte la trame sans ReleaseImage : l'image reste
                // empruntee pour toujours, et tous les AcquireImage suivants
                // sont refuses.
                if (premiereFuite == 0) {
                    premiereFuite = u;
                }
            } else {
                // ── Chemin normal : dessiner, puis rendre ──
                Dessiner(rendus[acquise], cmd, 2.f * (float32)(u % 180));
                ++dessinees;
                if (chaine.ReleaseImage()) {
                    ++liberations;
                }
            }
        }

        // ── Soumettre : le compositeur lit la derniere image relachee ──
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
    }
    const double total = std::chrono::duration<double>(std::chrono::steady_clock::now() - debut).count();

    std::FILE *sortie = std::fopen("compteurs_sortie.txt", "a");
    if (sortie != nullptr) {
        std::fprintf(sortie, "boucle %s : %.1f s, %lld trames, %lld dessinees\n",
                     trameFuite > 0 ? "avec le chemin qui fuit" : "normale", total, trames, dessinees);
        std::fprintf(sortie, "REUSSIES %lld\nREFUSEES %lld\nLIBERATIONS %lld\nECART %lld\n",
                     reussies, refusees, liberations, reussies - liberations);
        if (premiereFuite == 0) {
            std::fprintf(sortie, "PREMIERE FUITE AUCUNE\n");
        } else {
            std::fprintf(sortie, "PREMIERE FUITE %lld\n", premiereFuite);
        }
        if (premiereBloquee == 0) {
            std::fprintf(sortie, "PREMIERE BLOQUEE AUCUNE\n\n");
        } else {
            std::fprintf(sortie, "PREMIERE BLOQUEE %lld\n\n", premiereBloquee);
        }
        std::fclose(sortie);
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
