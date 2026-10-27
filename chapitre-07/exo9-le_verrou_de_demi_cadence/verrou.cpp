// verrou.cpp — chapitre 07, exercice 9 : le verrou de demi-cadence.
//
// La salle est alourdie : elle est soumise K fois, chaque copie decalee de
// 5 metres le long de x. Au demarrage, le programme ajoute des copies une a
// une jusqu'a ce que la moyenne des images depasse la periode de l'ecran
// (MASALLE_HZ, la frequence lue dans les parametres d'affichage de Windows).
// Puis, la salle alourdie dans les deux cas :
//   1. sans verrou : mille intervalles entre deux debuts d'image ;
//   2. avec verrou : a la fin de chaque image, on attend que DEUX periodes de
//      l'ecran se soient ecoulees depuis son debut (demi-cadence), puis mille
//      intervalles.
// Pour chaque mesure : moyenne, ecart type et maximum. La salle se ferme seule.
// Sortie : std::printf et verrou_sortie.txt (pas de console en Release).

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
#include <string>
#include <algorithm>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "MaSalleVerrou";
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

// Images ecartees au debut de chaque mesure, intervalles mesures.
static const int IMAGES_ECARTEES = 30;
static const int INTERVALLES_MESURES = 1000;
// Etalonnage : images par palier de copies, dont les premieres ecartees.
static const int IMAGES_PAR_PALIER = 40;
static const int ECARTEES_PAR_PALIER = 10;
static const int COPIES_MAX = 80;
static const float32 ECART_COPIES = 5.f;  // metres, le long de x

static uint64 EnvU64(const char *nom, uint64 parDefaut) {
    const char *v = std::getenv(nom);
    if (v == nullptr || v[0] == '\0') {
        return parDefaut;
    }
    return (uint64)std::strtoull(v, nullptr, 10);
}

// ── Le verrou : attendre jusqu'a un instant donne ───────────────────────
// NkChrono::SleepMilliseconds, sans BeginPreciseTiming(), dure ~15,5 ms quoi
// qu'on lui demande entre 1 et 12 ms (commentaire de NKTime/NkChrono.h,
// mesure sous Windows 11) : un verrou fait de ce seul sommeil serait quantifie,
// pas cadence. D'ou trois precautions :
//   1. BeginPreciseTiming() au demarrage : minuterie de 1 ms pour ce processus ;
//   2. on ne dort que la partie entiere du temps restant, moins 2 ms de marge,
//      car un sommeil peut deborder d'environ une milliseconde ;
//   3. on finit par une courte attente active (YieldThread) jusqu'a l'instant
//      vise, a la microseconde pres, sur l'horloge qui ne recule jamais.
static void AttendreJusqua(float64 cibleNs) {
    for (;;) {
        const float64 resteMs = (cibleNs - NkChrono::Now().nanoseconds) / 1.0e6;
        if (resteMs <= 0.0) {
            return;
        }
        if (resteMs > 3.0) {
            NkChrono::SleepMilliseconds((int64)(resteMs - 2.0));
        } else {
            NkChrono::YieldThread();
        }
    }
}

// Moyenne, ecart type (racine de la moyenne des carres des ecarts) et maximum.
struct Stats {
    double moyenne = 0.0, ecartType = 0.0, maximum = 0.0;
};

static Stats Calculer(const std::vector<double> &t) {
    Stats s;
    if (t.empty()) {
        return s;
    }
    double somme = 0.0;
    for (double v : t) {
        somme += v;
        s.maximum = (v > s.maximum) ? v : s.maximum;
    }
    s.moyenne = somme / (double)t.size();
    double carres = 0.0;
    for (double v : t) {
        carres += (v - s.moyenne) * (v - s.moyenne);
    }
    s.ecartType = std::sqrt(carres / (double)t.size());
    return s;
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
    std::FILE *sortie = std::fopen("verrou_sortie.txt", "w");

    // La periode de l'ecran, tiree de sa frequence (parametres de Windows).
    const uint64 hzLu = EnvU64("MASALLE_HZ", 60);
    const double hz = (hzLu > 1) ? (double)hzLu : 60.0;
    const double periodeNs = 1.0e9 / hz;

    // Minuterie fine pour ce processus : sans elle, le verrou serait quantifie.
    NkChrono::BeginPreciseTiming();

    // MASALLE_EXIT : la salle s'arrete seule apres ce nombre d'images.
    const char *envExit = std::getenv("MASALLE_EXIT");
    const long long imagesMax = (envExit != nullptr) ? std::atoll(envExit) : 0;

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle alourdie - verrou de demi-cadence, ne touchez a rien";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[MaSalleVerrou] Creation fenetre KO");
        return 1;
    }

    // Pointeur cache et retenu dans la fenetre : la souris pilote la tete.
    window.ShowMouse(false);
    window.CaptureMouse(true);
    window.ClipMouseToClient(true);

    uint32 largeur = (uint32)window.GetSize().width;
    uint32 hauteur = (uint32)window.GetSize().height;

    // On essaie les interfaces une a une et on garde la premiere qui demarre
    // (sur cette machine : Vulkan echoue, DirectX 12 ne prepare pas le PBR).
    // La salle attend l'ecran (synchronisation par defaut) dans les deux mesures.
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
        logger.Error("[MaSalleVerrou] Aucune interface graphique ne demarre le renderer");
        window.ClipMouseToClient(false);
        window.CaptureMouse(false);
        window.ShowMouse(true);
        window.Close();
        return 2;
    }

    const char *nomApi = NkGraphicsApiName(device->GetApi());
    logger.Info("[MaSalleVerrou] api={0}", nomApi);

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

    // Trois phases : etalonnage (copies ajoutees), mesure sans verrou, mesure avec.
    enum class Phase { Etalonnage, SansVerrou, AvecVerrou, Fini };
    Phase phase = Phase::Etalonnage;
    int copies = 1;
    int imageDansPhase = 0;
    std::vector<double> temps;  // intervalles de la phase en cours, ms
    std::vector<double> tempsSans, tempsAvec;
    std::vector<double> etalonnage;  // moyenne de chaque palier
    float64 debutPrecedent = -1.0;
    char ligne[220];

    while (enMarche && window.IsOpen() && phase != Phase::Fini) {
        // ── DEBUT DE L'IMAGE : on note l'instant present ──
        const float64 debutImage = NkChrono::Now().nanoseconds;
        if (debutPrecedent >= 0.0) {
            const double intervalle = (debutImage - debutPrecedent) / 1.0e6;  // ns -> ms
            ++imageDansPhase;
            if (phase == Phase::Etalonnage) {
                if (imageDansPhase > ECARTEES_PAR_PALIER) {
                    temps.push_back(intervalle);
                }
                if (imageDansPhase >= IMAGES_PAR_PALIER) {
                    const Stats st = Calculer(temps);
                    etalonnage.push_back(st.moyenne);
                    // La salle ne tient plus la periode de l'ecran : on garde ce K.
                    if (st.moyenne > (periodeNs / 1.0e6) * 1.10 || copies >= COPIES_MAX) {
                        phase = Phase::SansVerrou;
                    } else {
                        ++copies;
                    }
                    temps.clear();
                    imageDansPhase = 0;
                }
            } else {
                if (imageDansPhase > IMAGES_ECARTEES) {
                    temps.push_back(intervalle);
                }
                if ((int)temps.size() >= INTERVALLES_MESURES) {
                    if (phase == Phase::SansVerrou) {
                        tempsSans = temps;
                        phase = Phase::AvecVerrou;
                    } else {
                        tempsAvec = temps;
                        phase = Phase::Fini;
                    }
                    temps.clear();
                    imageDansPhase = 0;
                }
            }
        }
        debutPrecedent = debutImage;
        if (phase == Phase::Fini) {
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
        camData.farPlane = 500.f;  // les copies s'alignent loin le long de x
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
        // La salle, soumise K fois : chaque copie decalee de 5 m le long de x.
        for (int c = 0; c < copies; ++c) {
            const NkVec3f decalage = {ECART_COPIES * (float32)c, 0.f, 0.f};
            for (const Piece &piece : PIECES) {
                const NkVec3f centre = {piece.centre.x + decalage.x, piece.centre.y, piece.centre.z};
                NkDrawCall3D dc;
                dc.mesh = cube;
                dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(piece.taille);
                dc.aabb = {{centre.x - piece.taille.x * 0.5f, centre.y - piece.taille.y * 0.5f,
                            centre.z - piece.taille.z * 0.5f},
                           {centre.x + piece.taille.x * 0.5f, centre.y + piece.taille.y * 0.5f,
                            centre.z + piece.taille.z * 0.5f}};
                dc.tint = piece.couleur;
                dc.roughness = 0.8f;
                r3d->Submit(dc);
            }
        }

        renderer->Present();
        renderer->EndFrame();

        // ── LE VERROU : a la fin de l'image, attendre que deux periodes de
        // l'ecran se soient ecoulees depuis son DEBUT. ──
        if (phase == Phase::AvecVerrou) {
            AttendreJusqua(debutImage + 2.0 * periodeNs);
        }
    }


    NkChrono::EndPreciseTiming();

    // ── Les resultats ──
    auto Ecrire = [&](const char *l) {
        std::printf("%s\n", l);
        if (sortie != nullptr) {
            std::fprintf(sortie, "%s\n", l);
        }
    };
    std::snprintf(ligne, sizeof(ligne), "copies de la salle : %d", copies);
    Ecrire(ligne);
    const Stats sans = Calculer(tempsSans);
    const Stats avec = Calculer(tempsAvec);
    std::snprintf(ligne, sizeof(ligne), "sans verrou : moyenne %.3f ms, ecart type %.3f ms, max %.3f ms", sans.moyenne,
                  sans.ecartType, sans.maximum);
    Ecrire(ligne);
    std::snprintf(ligne, sizeof(ligne), "avec verrou : moyenne %.3f ms, ecart type %.3f ms, max %.3f ms", avec.moyenne,
                  avec.ecartType, avec.maximum);
    Ecrire(ligne);
    // Pour la reponse : la periode, les cadences, les intervalles par tranche.
    auto Compter = [](const std::vector<double> &t, double bas, double haut) {
        int n = 0;
        for (double v : t) {
            n += (v >= bas && v < haut) ? 1 : 0;
        }
        return n;
    };
    const double p = periodeNs / 1.0e6;
    std::snprintf(ligne, sizeof(ligne),
                  "(info) api=%s, hz=%d, periode %.3f ms, intervalles sans=%d avec=%d, images/s sans %.1f avec %.1f",
                  nomApi, (int)hz, p, (int)tempsSans.size(), (int)tempsAvec.size(),
                  sans.moyenne > 0 ? 1000.0 / sans.moyenne : 0.0, avec.moyenne > 0 ? 1000.0 / avec.moyenne : 0.0);
    Ecrire(ligne);
    std::snprintf(ligne, sizeof(ligne),
                  "(info) sans verrou : < 1,5 periode %d, 1,5 a 2,5 periodes %d, >= 2,5 periodes %d",
                  Compter(tempsSans, 0.0, 1.5 * p), Compter(tempsSans, 1.5 * p, 2.5 * p), Compter(tempsSans, 2.5 * p, 1e9));
    Ecrire(ligne);
    std::snprintf(ligne, sizeof(ligne),
                  "(info) avec verrou : < 1,5 periode %d, 1,5 a 2,5 periodes %d, >= 2,5 periodes %d",
                  Compter(tempsAvec, 0.0, 1.5 * p), Compter(tempsAvec, 1.5 * p, 2.5 * p), Compter(tempsAvec, 2.5 * p, 1e9));
    Ecrire(ligne);
    std::string paliers = "(info) etalonnage, moyenne par nombre de copies :";
    for (size_t i = 0; i < etalonnage.size(); ++i) {
        char m[32];
        std::snprintf(m, sizeof(m), " %d=%.1f", (int)(i + 1), etalonnage[i]);
        paliers += m;
    }
    Ecrire(paliers.c_str());
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
