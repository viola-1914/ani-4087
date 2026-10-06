// MaSalle — chapitre 04, exercice 9 : le budget des sous-systemes.
//
// Mesure UN demarrage de la salle, puis se ferme tout seul :
//   - debut du chronometre : entree dans nkmain ;
//   - fin du chronometre   : premiere image de la salle presentee a l'ecran.
//
// Argument de lancement :
//   ALL   (defaut) : cfg.subsystems = NK_SS_ALL, la valeur par defaut ;
//   SALLE          : cfg.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW,
//                    ce dont la salle a besoin.
//   API=OPENGL     : (facultatif) impose l'interface graphique ; sinon le
//                    programme essaie Vulkan, OpenGL, DX11 puis DX12.
//
// Le renderer cherche ses shaders dans Resources/NKRenderer, a partir du
// dossier courant : mesurer.ps1 relie ce dossier au code source de Nkentseu.
//
// Le temps est AJOUTE a la fin de mesures_brutes.txt, une ligne par
// lancement : "ALL 812". Le script mesurer.ps1 relance le programme dix fois
// par configuration et met les vingt lignes au format demande.
//
// La salle est celle du plan du chapitre 1 (6 m x 4 m x 2,80 m), avec les
// positions calculees par PoserAuSol / PoserSurTable a l'exercice 8.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKLogger/NkLog.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "NKRenderer/Core/NkRendererResult.h"

#include <chrono>
#include <cstdio>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "MaSalleBudget";
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

// Vrai si l'un des arguments vaut exactement "SALLE".
static bool DemandeSalle(const NkEntryState &state) {
    for (const NkString &argument : state.args) {
        if (std::strcmp(argument.CStr(), "SALLE") == 0) {
            return true;
        }
    }
    return false;
}

// Trace un echec dans diagnostic.txt : une application a fenetre n'a pas de
// console en Release, et sans cette trace un echec serait muet.
static void Diagnostic(const char *config, const char *message) {
    std::FILE *fichier = std::fopen("diagnostic.txt", "a");
    if (fichier == nullptr) {
        return;
    }
    std::fprintf(fichier, "%s : %s\n", config, message);
    std::fclose(fichier);
}

static void EnregistrerMesure(const char *config, long long ms) {
    std::FILE *fichier = std::fopen("mesures_brutes.txt", "a");
    if (fichier == nullptr) {
        logger.Error("[MaSalleBudget] Impossible d'ouvrir mesures_brutes.txt");
        return;
    }
    std::fprintf(fichier, "%s %lld\n", config, ms);
    std::fclose(fichier);
}

// Lit un argument "API=VULKAN", "API=OPENGL", "API=DX11" ou "API=DX12".
static bool ApiDemandee(const NkEntryState &state, NkGraphicsApi &api) {
    for (const NkString &argument : state.args) {
        const char *texte = argument.CStr();
        if (std::strcmp(texte, "API=VULKAN") == 0) {
            api = NkGraphicsApi::NK_GFX_API_VULKAN;
            return true;
        }
        if (std::strcmp(texte, "API=OPENGL") == 0) {
            api = NkGraphicsApi::NK_GFX_API_OPENGL;
            return true;
        }
        if (std::strcmp(texte, "API=DX11") == 0) {
            api = NkGraphicsApi::NK_GFX_API_DX11;
            return true;
        }
        if (std::strcmp(texte, "API=DX12") == 0) {
            api = NkGraphicsApi::NK_GFX_API_DX12;
            return true;
        }
    }
    return false;
}

static void AjouterEchec(char *echecs, usize taille, const char *api, const char *raison) {
    const usize longueur = std::strlen(echecs);
    if (longueur + 1 >= taille) {
        return;
    }
    std::snprintf(echecs + longueur, taille - longueur, " [%s : %s]", api, raison);
}

// Ecrit le mot-cle de l'interface qui a marche, pour que mesurer.ps1
// l'impose aux vingt mesures.
static void EcrireApiRetenue(NkGraphicsApi api) {
    const char *motCle = "VULKAN";
    if (api == NkGraphicsApi::NK_GFX_API_OPENGL) {
        motCle = "OPENGL";
    } else if (api == NkGraphicsApi::NK_GFX_API_DX11) {
        motCle = "DX11";
    } else if (api == NkGraphicsApi::NK_GFX_API_DX12) {
        motCle = "DX12";
    }
    std::FILE *fichier = std::fopen("api_retenue.txt", "w");
    if (fichier == nullptr) {
        return;
    }
    std::fprintf(fichier, "%s\n", motCle);
    std::fclose(fichier);
}

int nkmain(const NkEntryState &state) {
    // Le chronometre part avant la fenetre : tout le demarrage est compte.
    const auto debut = std::chrono::steady_clock::now();

    const bool salle = DemandeSalle(state);
    const char *nomConfig = "ALL";
    if (salle) {
        nomConfig = "SALLE";
    }

    NkWindowConfig winCfg;
    winCfg.title = "MaSalle - budget des sous-systemes";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[MaSalleBudget] Creation fenetre KO");
        Diagnostic(nomConfig, "echec creation fenetre (code 1)");
        return 1;
    }

    // Interfaces essayees dans l'ordre. La detection automatique a retenu
    // DirectX 12 sur cette machine, mais le renderer n'y compile pas son
    // nuanceur PBR : on essaie donc les interfaces une a une et on garde la
    // premiere avec laquelle le renderer demarre vraiment.
    NkGraphicsApi ordre[4] = {
        NkGraphicsApi::NK_GFX_API_VULKAN,
        NkGraphicsApi::NK_GFX_API_OPENGL,
        NkGraphicsApi::NK_GFX_API_DX11,
        NkGraphicsApi::NK_GFX_API_DX12,
    };
    int nombreApis = 4;

    // "API=OPENGL" (passe par mesurer.ps1) impose une seule interface :
    // les vingt mesures ne paient alors aucun essai rate.
    NkGraphicsApi imposee = NkGraphicsApi::NK_GFX_API_NONE;
    if (ApiDemandee(state, imposee)) {
        ordre[0] = imposee;
        nombreApis = 1;
    }

    const uint32 largeur = (uint32)window.GetSize().width;
    const uint32 hauteur = (uint32)window.GetSize().height;

    NkIDevice *device = nullptr;
    NkRenderer *renderer = nullptr;
    char echecs[2048] = "";

    for (int i = 0; i < nombreApis && renderer == nullptr; ++i) {
        NkDeviceInitInfo devInfo{};
        devInfo.surface = window.GetSurfaceDesc();
        devInfo.width = largeur;
        devInfo.height = hauteur;
        devInfo.api = ordre[i];

        device = NkDeviceFactory::CreateWithFallback(devInfo, {ordre[i]});
        if (device == nullptr || !device->IsValid()) {
            AjouterEchec(echecs, sizeof(echecs), NkGraphicsApiName(ordre[i]), "device refuse");
            NkDeviceFactory::Destroy(device);
            continue;
        }

        // La seule ligne qui change entre les deux configurations.
        NkRendererConfig cfg;
        cfg.api = device->GetApi();
        cfg.width = largeur;
        cfg.height = hauteur;
        if (salle) {
            cfg.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;
        } else {
            cfg.subsystems = NK_SS_ALL;
        }

        renderer = NkRenderer::Create(device, cfg);
        if (renderer != nullptr && renderer->Initialize()) {
            break;
        }

        AjouterEchec(echecs, sizeof(echecs), NkGraphicsApiName(ordre[i]), NkRGetLastErrorMessage());
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
    }

    if (renderer == nullptr) {
        logger.Error("[MaSalleBudget] Init renderer KO");
        char message[2200];
        std::snprintf(message, sizeof(message), "echec initialisation renderer (code 3) :%s", echecs);
        Diagnostic(nomConfig, message);
        window.Close();
        return 3;
    }

    // Le cours le demande : afficher l'interface retenue au demarrage.
    logger.Info("[MaSalleBudget] Interface graphique retenue : {0}", NkGraphicsApiName(device->GetApi()));
    EcrireApiRetenue(device->GetApi());

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle cube = renderer->GetMeshSystem()->GetCube();

    // On essaie jusqu'a obtenir UNE image presentee : BeginFrame peut
    // refuser les toutes premieres si la fenetre n'est pas encore prete.
    bool imagePresentee = false;
    for (int essai = 0; essai < 120 && !imagePresentee; ++essai) {
        NkEvents().PollEvents();
        if (!renderer->BeginFrame()) {
            continue;
        }

        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)largeur / (float32)hauteur;
        // Plan rapproche a cinq centimetres : on est DANS la salle.
        camData.nearPlane = 0.05f;
        camData.farPlane = 50.f;
        NkCamera3D cam(camData);
        cam.SetPosition({-1.8f, 1.6f, 1.6f});
        cam.SetTarget({0.5f, 0.8f, 0.f});

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
        imagePresentee = true;
    }

    // Le GPU doit avoir fini l'image pour qu'elle soit vraiment a l'ecran.
    device->WaitIdle();
    const auto fin = std::chrono::steady_clock::now();
    const long long ms = (long long)std::chrono::duration_cast<std::chrono::milliseconds>(fin - debut).count();

    if (imagePresentee) {
        EnregistrerMesure(nomConfig, ms);
        logger.Info("[MaSalleBudget] {0} : premiere image en {1} ms", nomConfig, ms);
    } else {
        logger.Error("[MaSalleBudget] Aucune image presentee : mesure non enregistree");
        Diagnostic(nomConfig, "aucune image presentee en 120 essais (code 4)");
    }

    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    if (!imagePresentee) {
        return 4;
    }
    return 0;
}