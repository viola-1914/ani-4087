// deux-yeux.cpp — chapitre 06, exercice 9 : les deux images et l'image noire.
//
// La salle est dessinee deux fois par image, une fois par oeil, chacune dans
// sa cible hors ecran de 640 x 720, puis les deux cibles sont posees cote a
// cote dans la fenetre (1280 x 720).
//
//   Images  0 a 30 : les deux yeux sont decales de 31,5 mm de part et d'autre
//                    de la tete, le long de sa droite. Capture a l'image 30 :
//                    oeil-gauche.png et oeil-droit.png.
//   Images 31 a 60 : on « oublie » de decaler les yeux : les deux cameras sont
//                    a la place de la tete. Capture a l'image 60.
//
// Apres la boucle, les captures sont rechargees (NkImage) et soustraites
// pixel par pixel : difference.png, difference-vue-oubliee.png et
// difference.txt. La fenetre se ferme seule apres la seconde capture.
//
// Organisation des renderers :
//   - un renderer « ecran » (Render2D seul) possede la frame : BeginFrame,
//     Present, EndFrame, et pose les deux cibles dans la fenetre ;
//   - deux renderers « oeil » (Render3D + ombres) ne font jamais BeginFrame :
//     a chaque image, on rejoue pour eux ce que BeginFrame ferait
//     (FlushGraphRebuilds, ResetFrame, Upload), on leur soumet la salle, puis
//     on execute leur graphe sur le tampon de commandes de l'ecran.

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
#include "NKImage/Core/NkImage.h"
#include "NKMath/NkColor.h"
#include "NKMath/NkMat.h"
#include "NKMath/NkQuat.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "MaSalleDeuxYeux";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

// ── Tailles ──────────────────────────────────────────────────────────────
static const uint32 LARGEUR_FENETRE = 1280;
static const uint32 HAUTEUR_FENETRE = 720;
static const uint32 LARGEUR_OEIL = 640;  // la moitie de la fenetre
static const uint32 HAUTEUR_OEIL = 720;

// ── Les images cles ──────────────────────────────────────────────────────
static const int IMAGE_CAPTURE = 30;          // yeux decales
static const int IMAGE_CAPTURE_OUBLIEE = 60;  // yeux a la place de la tete

// ── La tete et les yeux ──────────────────────────────────────────────────
// Tete droite, regard vers -Z (avant de la salle), haut +Y.
static const NkVec3f POSITION_TETE = {0.30f, 1.70f, 1.20f};
static const float32 IPD = 0.063f;  // ecart pupillaire, metres

// La droite de la tete : l'image de +X (la droite de la salle) par son
// orientation. NkQuatf::Right() n'est pas utilise : il tourne (-1, 0, 0)
// parce que le moteur prend l'avant en +Z (voir exercice 8).
static NkVec3f DroiteTete(const NkQuatf &orientation) {
    return orientation * NkVec3f(1.f, 0.f, 0.f);
}

// Une piece de la salle : un cube unite (-0,5 a +0,5) mis a l'echelle de sa
// taille puis translate a son centre. Ordre : Translate * Scale.
struct Piece {
    NkVec3f centre;
    NkVec3f taille;
    NkVec3f couleur;
};

// Positions en metres, reprises de salle_apres.cpp (exercice 8 du chapitre 4).
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

// La MEME salle, soumise telle quelle a chaque oeil : seule la camera change.
static void SoumettreSalle(NkRender3D *r3d, NkMeshHandle cube, const NkVec3f &positionOeil) {
    NkCamera3DData cd;
    cd.position = positionOeil;
    cd.target = positionOeil + NkVec3f(0.f, 0.f, -1.f);  // regard vers -Z
    cd.up = {0.f, 1.f, 0.f};
    cd.fovY = 70.f;
    cd.aspect = (float32)LARGEUR_OEIL / (float32)HAUTEUR_OEIL;
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
}

// Rejoue pour un renderer d'oeil ce que BeginFrame ferait, lui soumet la
// salle, puis execute son graphe sur le tampon de commandes de l'ecran.
static void DessinerOeil(NkRenderer *oeil, NkICommandBuffer *cmdEcran, NkMeshHandle cube,
                         const NkVec3f &positionOeil) {
    oeil->FlushGraphRebuilds();
    oeil->GetRender3D()->ResetFrame();  // sinon il herite des UBO de l'image precedente
    if (oeil->GetMaterialCollection() != nullptr) {
        oeil->GetMaterialCollection()->Upload();
    }
    SoumettreSalle(oeil->GetRender3D(), cube, positionOeil);
    oeil->GetRenderGraph()->Execute(cmdEcran);
}

// ── La soustraction ──────────────────────────────────────────────────────
struct Difference {
    bool ok = false;
    int32 pixelsDifferents = 0;  // ecart > 1
    int32 total = 0;
    int32 ecartMax = 0;
    float64 moyenneGauche = 0.0;  // pour reperer une capture uniforme (blanche)
    int32 minGauche = 255;
    int32 maxGauche = 0;
};

static int32 Absolu(int32 v) {
    return v < 0 ? -v : v;
}

// Recharge les deux captures, les compare pixel par pixel et ecrit l'image de
// la difference : chaque pixel vaut quatre fois l'ecart, borne a 255.
// Le noir veut donc dire « pareil ».
static Difference Soustraire(const char *fichierGauche, const char *fichierDroit, const char *fichierDiff) {
    Difference d;
    NkImage gauche;
    NkImage droite;
    if (!gauche.Load(fichierGauche) || !droite.Load(fichierDroit) || !gauche.IsValid() || !droite.IsValid()) {
        logger.Error("[MaSalleDeuxYeux] Rechargement impossible : {0} / {1}", fichierGauche, fichierDroit);
        return d;
    }
    const int32 w = gauche.Width();
    const int32 h = gauche.Height();
    if (droite.Width() != w || droite.Height() != h) {
        logger.Error("[MaSalleDeuxYeux] Tailles differentes");
        return d;
    }

    NkImage diff = NkImage::Create((uint32)w, (uint32)h, 4);
    float64 somme = 0.0;
    for (int32 y = 0; y < h; ++y) {
        for (int32 x = 0; x < w; ++x) {
            const math::NkColor a = gauche.GetPixel(x, y);
            const math::NkColor b = droite.GetPixel(x, y);
            // L'ecart d'un pixel : la plus grande difference sur ses trois canaux.
            int32 ecart = Absolu((int32)a.r - (int32)b.r);
            const int32 eg = Absolu((int32)a.g - (int32)b.g);
            const int32 eb = Absolu((int32)a.b - (int32)b.b);
            if (eg > ecart) {
                ecart = eg;
            }
            if (eb > ecart) {
                ecart = eb;
            }
            if (ecart > 1) {
                ++d.pixelsDifferents;
            }
            if (ecart > d.ecartMax) {
                d.ecartMax = ecart;
            }
            int32 v = ecart * 4;
            if (v > 255) {
                v = 255;
            }
            diff.SetPixel(x, y, math::NkColor((uint8)v, (uint8)v, (uint8)v, (uint8)255));

            // Temoin : une capture blanche ou noire partout donnerait aussi zero.
            const int32 lum = ((int32)a.r + (int32)a.g + (int32)a.b) / 3;
            somme += lum;
            if (lum < d.minGauche) {
                d.minGauche = lum;
            }
            if (lum > d.maxGauche) {
                d.maxGauche = lum;
            }
        }
    }
    d.total = w * h;
    d.moyenneGauche = somme / (float64)d.total;
    d.ok = diff.SavePNG(fichierDiff);
    return d;
}

int nkmain(const NkEntryState &) {
    NkWindowConfig winCfg;
    winCfg.title = "Ma salle - les deux yeux (oeil gauche | oeil droit)";
    winCfg.width = LARGEUR_FENETRE;
    winCfg.height = HAUTEUR_FENETRE;
    winCfg.centered = true;
    winCfg.resizable = false;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[MaSalleDeuxYeux] Creation fenetre KO");
        return 1;
    }

    const uint32 largeur = (uint32)window.GetSize().width;
    const uint32 hauteur = (uint32)window.GetSize().height;

    // ── Le device et le renderer de l'ecran ──
    // Sur cette machine, DirectX 12 ne prepare pas le nuanceur PBR : on essaie
    // les interfaces une a une et on garde la premiere qui demarre vraiment.
    const NkGraphicsApi ordreApi[4] = {
        NkGraphicsApi::NK_GFX_API_OPENGL,
        NkGraphicsApi::NK_GFX_API_VULKAN,
        NkGraphicsApi::NK_GFX_API_DX11,
        NkGraphicsApi::NK_GFX_API_DX12,
    };

    NkIDevice *device = nullptr;
    NkRenderer *ecran = nullptr;
    for (int i = 0; i < 4 && ecran == nullptr; ++i) {
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
        cfg.subsystems = NK_SS_RENDER2D;  // l'ecran ne fait que poser les deux images

        ecran = NkRenderer::Create(device, cfg);
        if (ecran != nullptr && ecran->Initialize()) {
            break;
        }
        NkRenderer::Destroy(ecran);
        NkDeviceFactory::Destroy(device);
    }
    if (ecran == nullptr) {
        logger.Error("[MaSalleDeuxYeux] Aucune interface graphique ne demarre le renderer");
        window.Close();
        return 2;
    }

    // ── Deux cibles hors ecran, une par oeil, 640 x 720 ──
    // Creees dans la bibliotheque de textures de l'ecran : c'est lui qui les
    // echantillonne pour les poser dans la fenetre.
    NkOffscreenDesc od;
    od.width = LARGEUR_OEIL;
    od.height = HAUTEUR_OEIL;
    od.hasDepth = false;  // chaque renderer d'oeil a sa propre profondeur
    od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
    od.readable = true;   // echantillonnee par l'ecran
    od.readback = true;   // relue pour la capture
    od.name = "oeil_gauche";
    NkOffscreenTarget *cibleGauche = ecran->CreateOffscreen(od);
    od.name = "oeil_droit";
    NkOffscreenTarget *cibleDroite = ecran->CreateOffscreen(od);
    if (cibleGauche == nullptr || cibleDroite == nullptr) {
        logger.Error("[MaSalleDeuxYeux] Cibles hors ecran KO");
        return 3;
    }

    // ── Deux renderers de salle, un par oeil, crees comme celui de la salle ──
    NkRenderer *oeilGauche = nullptr;
    NkRenderer *oeilDroit = nullptr;
    NkOffscreenTarget *cibles[2] = {cibleGauche, cibleDroite};
    NkRenderer **oeils[2] = {&oeilGauche, &oeilDroit};
    for (int i = 0; i < 2; ++i) {
        NkRendererConfig cfg;
        cfg.api = device->GetApi();
        cfg.width = LARGEUR_OEIL;
        cfg.height = HAUTEUR_OEIL;
        cfg.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;

        NkRenderer *r = NkRenderer::Create(device, cfg);
        if (r == nullptr || !r->Initialize()) {
            logger.Error("[MaSalleDeuxYeux] Renderer d'oeil {0} KO", i);
            NkRenderer::Destroy(r);
            return 4;
        }
        // Sa sortie finale va dans SA cible...
        r->SetFinalColorTarget(ecran->GetTextures()->GetRHIHandle(cibles[i]->GetColorResult()));
        // ... et il rend a la taille de cette cible, pas a celle de la fenetre.
        // Sans cette ligne : cibles blanches sur certaines machines, blocage a la
        // troisieme image sur d'autres.
        r->SetRenderSizeOverride(LARGEUR_OEIL, HAUTEUR_OEIL);
        *oeils[i] = r;
    }

    NkMeshHandle cubeGauche = oeilGauche->GetMeshSystem()->GetCube();
    NkMeshHandle cubeDroit = oeilDroit->GetMeshSystem()->GetCube();
    NkRender2D *r2d = ecran->GetRender2D();

    // Sur OpenGL, la premiere rangee stockee d'une cible est celle du BAS :
    // DrawOffscreen applique la meme regle que la capture et pose l'image a
    // l'endroit. Sur les autres interfaces, DrawImage la pose telle quelle.
    const bool cibleRetournee = NkOffscreenStoredIsBottomUp(device->GetApi());

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

    const NkQuatf orientationTete = NkQuatf::Identity();  // tete droite
    const NkVec3f droite = DroiteTete(orientationTete);
    const float32 demiEcart = IPD * 0.5f;

    bool captureOk = false;
    bool captureOublieeOk = false;

    for (int image = 0; enMarche && window.IsOpen(); ++image) {
        evenements.PollEvents();
        if (!enMarche) {
            break;
        }

        // Jusqu'a l'image 30 : les yeux sont decales le long de la droite de la
        // tete. Ensuite : on « oublie » le decalage, les deux cameras sont a la
        // place de la tete.
        const bool decaler = (image <= IMAGE_CAPTURE);
        const NkVec3f positionGauche = decaler ? POSITION_TETE - droite * demiEcart : POSITION_TETE;
        const NkVec3f positionDroite = decaler ? POSITION_TETE + droite * demiEcart : POSITION_TETE;

        if (!ecran->BeginFrame()) {
            continue;
        }
        NkICommandBuffer *cmd = ecran->GetCmd();

        // Les deux yeux, la meme salle, sur le tampon de commandes de l'ecran.
        DessinerOeil(oeilGauche, cmd, cubeGauche, positionGauche);
        DessinerOeil(oeilDroit, cmd, cubeDroit, positionDroite);

        // Les deux cibles cote a cote : x = 0 puis x = 640.
        r2d->Begin(cmd, largeur, hauteur);
        for (int i = 0; i < 2; ++i) {
            const float32 x = (float32)(i * (int)LARGEUR_OEIL);
            if (cibleRetournee) {
                r2d->DrawOffscreen({x, 0.f, 640.f, 720.f}, cibles[i]->GetColorResult());
            } else {
                r2d->DrawImage(cibles[i]->GetColorResult(), {x, 0.f, 640.f, 720.f});
            }
        }
        r2d->End();

        ecran->Present();
        ecran->EndFrame();

        // Les deux yeux sont captures A LA MEME IMAGE.
        if (image == IMAGE_CAPTURE) {
            device->WaitIdle();
            captureOk = cibleGauche->Capture("oeil-gauche.png") && cibleDroite->Capture("oeil-droit.png");
            logger.Info("[MaSalleDeuxYeux] capture yeux decales : {0}", captureOk ? "OK" : "ECHEC");
        }
        if (image == IMAGE_CAPTURE_OUBLIEE) {
            device->WaitIdle();
            captureOublieeOk = cibleGauche->Capture("oeil-gauche-vue-oubliee.png") &&
                               cibleDroite->Capture("oeil-droit-vue-oubliee.png");
            logger.Info("[MaSalleDeuxYeux] capture vue oubliee : {0}", captureOublieeOk ? "OK" : "ECHEC");
            break;
        }
    }

    device->WaitIdle();
    NkRenderer::Destroy(oeilGauche);
    NkRenderer::Destroy(oeilDroit);
    ecran->DestroyOffscreen(cibleGauche);
    ecran->DestroyOffscreen(cibleDroite);
    NkRenderer::Destroy(ecran);
    NkDeviceFactory::Destroy(device);
    window.Close();

    if (!captureOk || !captureOublieeOk) {
        logger.Error("[MaSalleDeuxYeux] Captures incompletes");
        return 5;
    }

    // ── Apres la boucle : les deux soustractions ──
    const Difference deuxYeux = Soustraire("oeil-gauche.png", "oeil-droit.png", "difference.png");
    const Difference oubliee = Soustraire("oeil-gauche-vue-oubliee.png", "oeil-droit-vue-oubliee.png",
                                          "difference-vue-oubliee.png");

    std::FILE *sortie = std::fopen("difference_sortie.txt", "w");
    char ligne[160];
    std::snprintf(ligne, sizeof(ligne), "deux yeux : %d pixels sur %d, ecart max %d",
                  deuxYeux.pixelsDifferents, deuxYeux.total, deuxYeux.ecartMax);
    std::printf("%s\n", ligne);
    if (sortie != nullptr) {
        std::fprintf(sortie, "%s\n", ligne);
    }
    std::snprintf(ligne, sizeof(ligne), "vue oubliee : %d pixels sur %d, ecart max %d",
                  oubliee.pixelsDifferents, oubliee.total, oubliee.ecartMax);
    std::printf("%s\n", ligne);
    if (sortie != nullptr) {
        std::fprintf(sortie, "%s\n", ligne);
        std::fclose(sortie);
    }

    // Le temoin des captures, a part : une difference nulle entre deux images
    // blanches ne prouverait rien.
    std::FILE *temoin = std::fopen("temoin_captures.txt", "w");
    if (temoin != nullptr) {
        std::fprintf(temoin, "oeil-gauche.png : luminosite moyenne %.1f, min %d, max %d\n",
                     deuxYeux.moyenneGauche, deuxYeux.minGauche, deuxYeux.maxGauche);
        std::fprintf(temoin, "oeil-gauche-vue-oubliee.png : luminosite moyenne %.1f, min %d, max %d\n",
                     oubliee.moyenneGauche, oubliee.minGauche, oubliee.maxGauche);
        std::fclose(temoin);
    }
    std::fflush(stdout);

    return (deuxYeux.ok && oubliee.ok) ? 0 : 6;
}
