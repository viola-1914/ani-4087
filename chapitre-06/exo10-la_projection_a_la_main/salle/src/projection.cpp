// projection.cpp — chapitre 06, exercice 10 : la projection a la main.
//
// Au demarrage, avant d'ouvrir la fenetre : la matrice de projection
// decentree est ecrite a la main (Projection), a partir de la matrice nulle,
// puis comparee coefficient par coefficient :
//   - epreuve symetrique : a NkMat4f::Perspective (70 deg, 1280/720, 0,05-100 m) ;
//   - epreuve quest : a la camera du moteur (NkCamera3D::SetFovAsym), pour
//     l'oeil gauche d'un Quest 2 (gauche -52, droite 45, haut 48, bas -50 deg).
// Sorties (std::printf et fichiers) :
//   ecarts_sortie.txt   : les huit lignes d'ecarts et le plus grand ecart ;
//   matrices_sortie.txt : les matrices elles-memes et les verifications en plus.
// Puis la salle s'ouvre ; Echap ferme.
//
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
    d.appName = "MaSalleProjection";
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

// ── La projection decentree, a la main ──────────────────────────────────
// Les quatre angles en radians, signes : gauche et bas sont negatifs.
// NkMat4f range ses coefficients par colonne : mat[colonne][ligne].
static NkMat4f Projection(float32 gauche, float32 droite, float32 haut, float32 bas,
                          float32 nearPlane, float32 farPlane) {
    const float32 tanLeft = math::NkTan(gauche);
    const float32 tanRight = math::NkTan(droite);
    const float32 tanUp = math::NkTan(haut);
    const float32 tanDown = math::NkTan(bas);
    const float32 tanWidth = tanRight - tanLeft;
    const float32 tanHeight = tanUp - tanDown;

    NkMat4f result;  // la matrice NULLE (le constructeur par defaut ne pose pas l'identite)
    result.mat[0][0] = 2.f / tanWidth;
    result.mat[1][1] = 2.f / tanHeight;
    // Le decentrage : troisieme colonne, lignes 0 et 1. Nuls en symetrique.
    result.mat[2][0] = (tanRight + tanLeft) / tanWidth;
    result.mat[2][1] = (tanUp + tanDown) / tanHeight;
    result.mat[2][3] = -1.f;  // w = -z_vue : la division perspective
    // La profondeur entre -1 et 1, comme NkMat4f::Perspective.
    result.mat[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
    result.mat[3][2] = 2.f * farPlane * nearPlane / (nearPlane - farPlane);
    return result;
}

static void Ecrire(std::FILE *f, const char *ligne) {
    std::printf("%s\n", ligne);
    if (f != nullptr) {
        std::fprintf(f, "%s\n", ligne);
    }
}

// Imprime une matrice ligne par ligne : la ligne r est mat[0][r] mat[1][r]
// mat[2][r] mat[3][r] (on parcourt les colonnes a ligne fixee).
static void ImprimerMatrice(std::FILE *f, const char *titre, const NkMat4f &m) {
    char ligne[160];
    std::snprintf(ligne, sizeof(ligne), "%s :", titre);
    Ecrire(f, ligne);
    for (int r = 0; r < 4; ++r) {
        std::snprintf(ligne, sizeof(ligne), "  %10.6f %10.6f %10.6f %10.6f",
                      (double)m.mat[0][r], (double)m.mat[1][r], (double)m.mat[2][r], (double)m.mat[3][r]);
        Ecrire(f, ligne);
    }
}

// Ecrit les quatre lignes d'ecarts |a - b| d'une epreuve, puis rend le plus grand.
static float32 Ecarts(std::FILE *f, const char *nom, const NkMat4f &a, const NkMat4f &b) {
    char ligne[160];
    float32 plusGrand = 0.f;
    for (int r = 0; r < 4; ++r) {
        float32 e[4];
        for (int c = 0; c < 4; ++c) {
            e[c] = std::fabs(a.mat[c][r] - b.mat[c][r]);
            if (e[c] > plusGrand) {
                plusGrand = e[c];
            }
        }
        std::snprintf(ligne, sizeof(ligne), "%s, ligne %d : %.2e %.2e %.2e %.2e", nom, r,
                      (double)e[0], (double)e[1], (double)e[2], (double)e[3]);
        Ecrire(f, ligne);
    }
    return plusGrand;
}

// Projette une direction de vue (x, y, -1) et rend ses coordonnees normalisees.
static void ProjeterDirection(const NkMat4f &m, float32 x, float32 y, float32 &ndcX, float32 &ndcY) {
    const float32 z = -1.f;
    const float32 cx = m.mat[0][0] * x + m.mat[1][0] * y + m.mat[2][0] * z + m.mat[3][0];
    const float32 cy = m.mat[0][1] * x + m.mat[1][1] * y + m.mat[2][1] * z + m.mat[3][1];
    const float32 cw = m.mat[0][3] * x + m.mat[1][3] * y + m.mat[2][3] * z + m.mat[3][3];
    ndcX = cx / cw;
    ndcY = cy / cw;
}

static void CalculerProjections() {
    const float32 nearPlane = 0.05f;
    const float32 farPlane = 100.f;

    // ── Epreuve symetrique : les quatre angles se deduisent de 70 deg et du rapport ──
    const float32 rapport = 1280.f / 720.f;
    const float32 demiVertical = 35.f * RADIANS_PAR_DEGRE;                    // 70 / 2
    const float32 demiHorizontal = std::atan(rapport * std::tan(demiVertical));  // tan(h) = rapport * tan(v)
    const NkMat4f symMain = Projection(-demiHorizontal, demiHorizontal, demiVertical, -demiVertical,
                                       nearPlane, farPlane);
    const NkMat4f symMoteur = NkMat4f::Perspective(NkAngle(70.f), rapport, nearPlane, farPlane);

    // ── Epreuve quest : l'oeil gauche d'un Quest 2 ──
    const float32 qGauche = -52.f * RADIANS_PAR_DEGRE;
    const float32 qDroite = 45.f * RADIANS_PAR_DEGRE;
    const float32 qHaut = 48.f * RADIANS_PAR_DEGRE;
    const float32 qBas = -50.f * RADIANS_PAR_DEGRE;
    const NkMat4f questMain = Projection(qGauche, qDroite, qHaut, qBas, nearPlane, farPlane);
    NkCamera3D cam;
    cam.SetNearFar(nearPlane, farPlane);
    cam.SetFovAsym(qGauche, qDroite, qHaut, qBas);  // radians, signes
    const NkMat4f questMoteur = cam.GetProj();

    // ── ecarts_sortie.txt : huit lignes d'ecarts, puis le plus grand ──
    std::FILE *ecarts = std::fopen("ecarts_sortie.txt", "w");
    const float32 maxSym = Ecarts(ecarts, "symetrique", symMain, symMoteur);
    const float32 maxQuest = Ecarts(ecarts, "quest", questMain, questMoteur);
    char ligne[200];
    std::snprintf(ligne, sizeof(ligne), "symetrique : plus grand ecart %.2e", (double)maxSym);
    Ecrire(ecarts, ligne);
    std::snprintf(ligne, sizeof(ligne), "quest : plus grand ecart %.2e", (double)maxQuest);
    Ecrire(ecarts, ligne);
    if (ecarts != nullptr) {
        std::fclose(ecarts);
    }

    // ── matrices_sortie.txt : les matrices et les verifications en plus ──
    std::FILE *mats = std::fopen("matrices_sortie.txt", "w");
    std::snprintf(ligne, sizeof(ligne), "angles symetriques (deg) : gauche %.4f droite %.4f haut %.4f bas %.4f",
                  (double)(-demiHorizontal * DEGRES_PAR_RADIAN), (double)(demiHorizontal * DEGRES_PAR_RADIAN),
                  (double)(demiVertical * DEGRES_PAR_RADIAN), (double)(-demiVertical * DEGRES_PAR_RADIAN));
    Ecrire(mats, ligne);
    ImprimerMatrice(mats, "symetrique, a la main", symMain);
    ImprimerMatrice(mats, "symetrique, NkMat4f::Perspective", symMoteur);
    ImprimerMatrice(mats, "quest, a la main", questMain);
    ImprimerMatrice(mats, "quest, NkCamera3D::GetProj", questMoteur);

    // Verification 1 : les quatre bords du champ quest tombent sur les bords
    // de l'image (-1 et +1), et le regard droit devant n'est PAS au centre.
    float32 nx = 0.f, ny = 0.f;
    ProjeterDirection(questMain, std::tan(qGauche), 0.f, nx, ny);
    std::snprintf(ligne, sizeof(ligne), "quest, bord gauche  (tan -52, 0, -1) -> x = %.6f", (double)nx);
    Ecrire(mats, ligne);
    ProjeterDirection(questMain, std::tan(qDroite), 0.f, nx, ny);
    std::snprintf(ligne, sizeof(ligne), "quest, bord droit   (tan 45, 0, -1)  -> x = %.6f", (double)nx);
    Ecrire(mats, ligne);
    ProjeterDirection(questMain, 0.f, std::tan(qHaut), nx, ny);
    std::snprintf(ligne, sizeof(ligne), "quest, bord haut    (0, tan 48, -1)  -> y = %.6f", (double)ny);
    Ecrire(mats, ligne);
    ProjeterDirection(questMain, 0.f, std::tan(qBas), nx, ny);
    std::snprintf(ligne, sizeof(ligne), "quest, bord bas     (0, tan -50, -1) -> y = %.6f", (double)ny);
    Ecrire(mats, ligne);
    ProjeterDirection(questMain, 0.f, 0.f, nx, ny);
    std::snprintf(ligne, sizeof(ligne), "quest, droit devant (0, 0, -1)       -> x = %.6f, y = %.6f", (double)nx,
                  (double)ny);
    Ecrire(mats, ligne);
    ProjeterDirection(symMain, 0.f, 0.f, nx, ny);
    std::snprintf(ligne, sizeof(ligne), "symetrique, droit devant (0, 0, -1)  -> x = %.6f, y = %.6f", (double)nx,
                  (double)ny);
    Ecrire(mats, ligne);

    // Verification 2 : le chemin decentre de la camera, nourri des angles
    // symetriques, rejoint Perspective : les deux chemins du moteur s'accordent.
    NkCamera3D camSym;
    camSym.SetNearFar(nearPlane, farPlane);
    camSym.SetFovAsym(-demiHorizontal, demiHorizontal, demiVertical, -demiVertical);
    float32 maxCam = 0.f;
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            const float32 e = std::fabs(camSym.GetProj().mat[c][r] - symMoteur.mat[c][r]);
            if (e > maxCam) {
                maxCam = e;
            }
        }
    }
    std::snprintf(ligne, sizeof(ligne), "camera SetFovAsym symetrique contre Perspective : plus grand ecart %.2e",
                  (double)maxCam);
    Ecrire(mats, ligne);

    // Verification 3 : le piege de l'ordre. Lue mat[ligne][colonne], la
    // matrice quest a la main ne ressemble plus a celle du moteur.
    float32 maxTransposee = 0.f;
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            const float32 e = std::fabs(questMain.mat[r][c] - questMoteur.mat[c][r]);
            if (e > maxTransposee) {
                maxTransposee = e;
            }
        }
    }
    std::snprintf(ligne, sizeof(ligne), "quest lue a l'envers (mat[ligne][colonne]) : plus grand ecart %.2e",
                  (double)maxTransposee);
    Ecrire(mats, ligne);

    std::fflush(stdout);
    if (mats != nullptr) {
        std::fclose(mats);
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
    CalculerProjections();

    const char *envSeule = std::getenv("MASALLE_CALCUL_SEUL");
    if (envSeule != nullptr && std::strcmp(envSeule, "1") == 0) {
        return 0;
    }

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle - la projection a la main (Echap pour quitter)";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[MaSalleProjection] Creation fenetre KO");
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
        logger.Error("[MaSalleProjection] Aucune interface graphique ne demarre le renderer");
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
