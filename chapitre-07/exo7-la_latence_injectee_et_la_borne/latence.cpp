// latence.cpp — chapitre 07, exercice 7 : la latence injectee et la borne.
//
// Au demarrage, avant d'ouvrir la fenetre, sans casque : une tete fait un
// quart de tour en 400 ms (depart et arrivee doux), commence a t = 1 s. Une
// pose par periode de 72 hertz, pendant trois secondes. Comme le simulateur du
// moteur, on garde les 64 dernieres poses dans un tableau circulaire, chacune
// avec son instant et sa vitesse, et pour simuler une latence on ne rend pas
// la plus recente mais celle d'il y a « latence » secondes (SampleAt).
// La pose affichee est predite pour l'instant d'affichage, t + periode, a
// partir de cet echantillon (PredictAt, bornee a 100 ms), puis comparee a la
// vraie pose de cet instant. Moyenne et maximum de t = 1 s a t = 1,9 s, avec
// et sans prediction, pour des latences de 0, 10, 20, 40 et 80 ms.
// Puis : latence d'une seconde, sans la borne puis avec, pour l'image dont
// l'echantillon est le plus proche de t = 1,2 s (le milieu du geste).
// Sortie : std::printf et latence_sortie.txt. Puis la salle s'ouvre.
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
    d.appName = "MaSalleLatence";
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

// ── Le geste ─────────────────────────────────────────────────────────────
static const double kPi = 3.14159265358979323846;

// Un quart de tour en 400 ms, depart et arrivee doux.
static double Lacet(double t) {  // t en secondes depuis le debut, lacet en degres
    const double amplitude = 90.0, duree = 0.400;
    if (t <= 0.0) {
        return 0.0;
    }
    if (t >= duree) {
        return amplitude;
    }
    return amplitude * 0.5 * (1.0 - std::cos(kPi * t / duree));
}

static const double DEBUT_GESTE = 1.0;  // le quart de tour commence a t = 1 s

// La vraie pose de la tete a l'instant t (NkAngle se construit en degres).
static NkQuatf VraiePose(double t) {
    return NkQuatf::RotateY(NkAngle((float32)Lacet(t - DEBUT_GESTE)));
}

// ── La vitesse et l'extrapolation, comme le module du moteur ────────────
static NkVec3f VitesseAngulaire(const NkQuatf &from, const NkQuatf &to, float32 dtSeconds) {
    if (dtSeconds <= 0.f) {
        return NkVec3f(0.f, 0.f, 0.f);
    }
    NkQuatf delta = (to * from.Conjugate()).Normalized();
    // Les deux hemispheres du quaternion codent la meme rotation ; on
    // force le chemin court, sinon un delta minuscule peut se lire
    // comme un tour presque complet dans l'autre sens.
    if (delta.w < 0.f) {
        delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);
    }
    const float32 angleRad = delta.Angle().Rad();
    if (angleRad <= 1e-6f) {
        return NkVec3f(0.f, 0.f, 0.f);
    }
    return delta.Axis() * (angleRad / dtSeconds);
}

static NkQuatf Extrapoler(const NkQuatf &orientation, const NkVec3f &vitesse, float32 dtSeconds) {
    const float32 speedRad = vitesse.Len();
    if (speedRad > 1e-6f) {
        const NkVec3f axis = vitesse * (1.f / speedRad);
        // Delta en espace de BASE : il se compose a GAUCHE de
        // l'orientation courante (meme convention que la mesure).
        const NkQuatf delta(NkAngle::FromRad(speedRad * dtSeconds), axis);
        return (delta * orientation).Normalized();
    }
    return orientation;
}

// ── Les poses horodatees et l'historique circulaire du simulateur ───────
struct PoseDatee {
    double time = 0.0;          // secondes
    NkQuatf pose = NkQuatf::Identity();
    NkVec3f velocity = {0.f, 0.f, 0.f};  // rad/s

    // Pose predite a l'instant demande. La fenetre d'extrapolation est
    // bornee : au-dela de 100 ms le modele « vitesses constantes » ment
    // plus qu'il n'aide (et OpenXR borne pareil cote runtimes).
    // (borne = false : la meme chose, la borne retiree.)
    NkQuatf PredictAt(double when, bool borne) const {
        float32 dt = (float32)(when - time);
        if (borne) {
            dt = math::NkClamp(dt, -0.1f, 0.1f);
        }
        return Extrapoler(pose, velocity, dt);
    }
};

struct Historique {
    static const uint32 kMaxSamples = 64;
    PoseDatee mSamples[kMaxSamples];
    uint32 mSampleHead = 0;   // indice de la plus recente
    uint32 mSampleCount = 0;

    void Ajouter(const PoseDatee &p) {
        if (mSampleCount > 0) {
            mSampleHead = (mSampleHead + 1u) % kMaxSamples;
        }
        mSamples[mSampleHead] = p;
        if (mSampleCount < kMaxSamples) {
            ++mSampleCount;
        }
    }

    const PoseDatee &Derniere() const {
        return mSamples[mSampleHead];
    }

    // Comme NkXrSimulatorBackend::SampleAt, en secondes.
    const PoseDatee &SampleAt(double now, double latencySeconds) const {
        // Latence nulle : l'echantillon le plus recent, point.
        if (latencySeconds <= 0.0 || mSampleCount <= 1u) {
            return mSamples[mSampleHead];
        }
        // Latence simulee : on rend l'echantillon d'il y a « latence »
        // secondes — la prediction doit alors combler l'ecart.
        const double target = now - latencySeconds;
        uint32 index = mSampleHead;
        for (uint32 i = 0u; i < mSampleCount; ++i) {
            if (mSamples[index].time <= target) {
                return mSamples[index];
            }
            index = (index + kMaxSamples - 1u) % kMaxSamples;
        }
        // Plus vieux disponible : l'historique ne remonte pas si loin.
        return mSamples[(mSampleHead + kMaxSamples - (mSampleCount - 1u)) % kMaxSamples];
    }
};

// L'erreur : l'angle entre la pose montree a et la vraie pose b, en degres.
static double Erreur(const NkQuatf &a, const NkQuatf &b) {
    return (b * a.Conjugate()).Normalized().Angle().Deg();
}

// Le lacet d'une rotation autour de la verticale : 2 * atan2(q.y, q.w),
// ramene entre -180 et 180 degres.
static double LacetDe(const NkQuatf &q) {
    double l = 2.0 * std::atan2((double)q.y, (double)q.w) * 180.0 / kPi;
    while (l > 180.0) {
        l -= 360.0;
    }
    while (l <= -180.0) {
        l += 360.0;
    }
    return l;
}

static const double HZ = 72.0;
static const double PERIODE = 1.0 / HZ;
static const int NB_POSES = 216;  // trois secondes a 72 Hz

static void Ecrire(std::FILE *f, const char *ligne) {
    std::printf("%s\n", ligne);
    if (f != nullptr) {
        std::fprintf(f, "%s\n", ligne);
    }
}

// Une passe sur les trois secondes : pour une latence donnee, moyenne et max
// des erreurs de t = 1 s a t = 1,9 s, avec et sans prediction.
struct Resultat {
    double moyAvec = 0.0, maxAvec = 0.0, moySans = 0.0, maxSans = 0.0;
    int n = 0;
};

static Resultat Mesurer(double latence) {
    Historique h;
    Resultat r;
    double sommeAvec = 0.0, sommeSans = 0.0;
    for (int k = 0; k < NB_POSES; ++k) {
        const double t = k * PERIODE;
        // La pose de cet instant entre dans l'historique, avec sa vitesse
        // calculee depuis la pose precedente.
        PoseDatee p;
        p.time = t;
        p.pose = VraiePose(t);
        p.velocity = (h.mSampleCount > 0) ? VitesseAngulaire(h.Derniere().pose, p.pose, (float32)PERIODE)
                                          : NkVec3f(0.f, 0.f, 0.f);
        h.Ajouter(p);

        if (t < 1.0 - 1e-9 || t > 1.9 + 1e-9) {
            continue;
        }
        // L'echantillon rendu pour cette latence, la pose montree a t + periode.
        const PoseDatee &e = h.SampleAt(t, latence);
        const double quand = t + PERIODE;
        const NkQuatf vraie = VraiePose(quand);
        const double errAvec = Erreur(e.PredictAt(quand, true), vraie);
        const double errSans = Erreur(e.pose, vraie);
        sommeAvec += errAvec;
        sommeSans += errSans;
        r.maxAvec = (errAvec > r.maxAvec) ? errAvec : r.maxAvec;
        r.maxSans = (errSans > r.maxSans) ? errSans : r.maxSans;
        ++r.n;
    }
    r.moyAvec = sommeAvec / r.n;
    r.moySans = sommeSans / r.n;
    return r;
}

// Latence d'une seconde : l'image dont l'echantillon est le plus proche de
// t = 1,2 s. On imprime l'instant de l'echantillon, l'horizon, les lacets.
static void LigneBorne(std::FILE *f, bool borne) {
    const double latence = 1.0;
    Historique h;
    double meilleur = 1e9;
    double tEch = 0.0, horizon = 0.0, lacetPredit = 0.0, lacetVrai = 0.0;
    for (int k = 0; k < NB_POSES; ++k) {
        const double t = k * PERIODE;
        PoseDatee p;
        p.time = t;
        p.pose = VraiePose(t);
        p.velocity = (h.mSampleCount > 0) ? VitesseAngulaire(h.Derniere().pose, p.pose, (float32)PERIODE)
                                          : NkVec3f(0.f, 0.f, 0.f);
        h.Ajouter(p);

        const PoseDatee &e = h.SampleAt(t, latence);
        const double ecart = std::fabs(e.time - 1.2);
        if (ecart < meilleur - 1e-12) {
            meilleur = ecart;
            const double quand = t + PERIODE;
            tEch = e.time;
            double hz = quand - e.time;  // l'horizon demande
            if (borne) {
                hz = (hz > 0.1) ? 0.1 : ((hz < -0.1) ? -0.1 : hz);  // ce que la borne laisse
            }
            horizon = hz;
            lacetPredit = LacetDe(e.PredictAt(quand, borne));
            lacetVrai = LacetDe(VraiePose(quand));
        }
    }
    char ligne[200];
    std::snprintf(ligne, sizeof(ligne), "%s : echantillon a %.3f s, horizon %.1f ms, predit %.2f deg, vrai %.2f deg",
                  borne ? "avec borne" : "sans borne", tEch, horizon * 1000.0, lacetPredit, lacetVrai);
    Ecrire(f, ligne);
}

static void CalculerLatences() {
    std::FILE *sortie = std::fopen("latence_sortie.txt", "w");
    char ligne[200];
    const int latencesMs[5] = {0, 10, 20, 40, 80};
    for (int ms : latencesMs) {
        const Resultat r = Mesurer(ms / 1000.0);
        std::snprintf(ligne, sizeof(ligne), "latence %d ms : avec %.3f deg (max %.3f), sans %.3f deg (max %.3f)", ms,
                      r.moyAvec, r.maxAvec, r.moySans, r.maxSans);
        Ecrire(sortie, ligne);
    }
    LigneBorne(sortie, false);  // la borne retiree
    LigneBorne(sortie, true);   // la borne remise
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
    CalculerLatences();

    const char *envSeule = std::getenv("MASALLE_CALCUL_SEUL");
    if (envSeule != nullptr && std::strcmp(envSeule, "1") == 0) {
        return 0;
    }

    NkWindowConfig winCfg;
    winCfg.title = "Ma salle - la latence injectee (Echap pour quitter)";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[MaSalleLatence] Creation fenetre KO");
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
        logger.Error("[MaSalleLatence] Aucune interface graphique ne demarre le renderer");
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
