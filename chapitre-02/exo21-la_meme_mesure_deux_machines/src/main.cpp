// MaSalle — exercice 21 : la meme mesure sur deux machines.
//
// Une boucle de calcul IDENTIQUE sur l'ordinateur et sur le telephone :
// 300 tours de 1 000 000 de points 3D transformes par une matrice 4x4,
// soit 300 millions de points au total. Le programme la chronometre et
// rapporte le temps total, le temps par tour, et la comparaison avec le
// budget d'une image de casque (11 ms).
//
// Windows : rapport dans une boite de message. Linux : dans la console.
// Android : rapport dans le journal, etiquette "MaSalle".

#include <chrono>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

static const int NB_POINTS = 1000000;   // points transformes par tour
static const int NB_TOURS  = 500;       // tours de boucle      // tours de boucle
static const double BUDGET_MS = 11.0;   // une image de casque a 90 Hz

struct Vec4 { float x, y, z, w; };

static Vec4 Transformer(const float m[16], const Vec4& v)
{
    return {
        m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12] * v.w,
        m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13] * v.w,
        m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
        m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w
    };
}

static std::string Mesurer()
{
    std::vector<Vec4> points(NB_POINTS);
    for (int i = 0; i < NB_POINTS; ++i)
        points[i] = { float(i % 97), float(i % 89), float(i % 83), 1.0f };
    std::vector<Vec4> sortie(NB_POINTS);

    float temoin = 0.0f;               // empeche la suppression du calcul
    auto debut = std::chrono::steady_clock::now();

    for (int tour = 0; tour < NB_TOURS; ++tour) {
        float a = 0.01f * tour;
        float m[16] = { std::cos(a), std::sin(a), 0, 0,
                       -std::sin(a), std::cos(a), 0, 0,
                        0, 0, 1, 0,
                        1, 2, 3, 1 };
        for (int i = 0; i < NB_POINTS; ++i)
            sortie[i] = Transformer(m, points[i]);
        temoin += sortie[tour * 1000].x;
    }

    auto fin = std::chrono::steady_clock::now();
    double totalMs = std::chrono::duration<double, std::milli>(fin - debut).count();
    double parTourMs = totalMs / NB_TOURS;

    char tampon[512];
    std::snprintf(tampon, sizeof(tampon),
        "Calcul : %d tours x %d points = %.0f millions de points\n"
        "Temps total   : %.0f ms (%.2f s)\n"
        "Par tour      : %.3f ms\n"
        "Budget casque : %.1f ms -> un tour occupe %.0f %% de l'image\n"
        "(temoin : %.1f)",
        NB_TOURS, NB_POINTS, double(NB_TOURS) * NB_POINTS / 1e6,
        totalMs, totalMs / 1000.0, parTourMs,
        BUDGET_MS, 100.0 * parTourMs / BUDGET_MS, temoin);
    return std::string(tampon);
}

// =============================================================================
//  Android
// =============================================================================
#if defined(__ANDROID__)
#include <android_native_app_glue.h>
#include <android/native_window.h>
#include <android/log.h>
#include <cstdint>
#define MASALLE_LOG(...) __android_log_print(ANDROID_LOG_INFO, "MaSalle", __VA_ARGS__)

static const uint32_t COULEUR = 0xFF8C8C1Eu;   // bleu-vert

static void Peindre(ANativeWindow* fenetre)
{
    ANativeWindow_setBuffersGeometry(fenetre, 0, 0, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer tampon;
    if (ANativeWindow_lock(fenetre, &tampon, nullptr) != 0)
        return;
    uint32_t* pixels = static_cast<uint32_t*>(tampon.bits);
    for (int32_t y = 0; y < tampon.height; ++y)
        for (int32_t x = 0; x < tampon.width; ++x)
            pixels[y * tampon.stride + x] = COULEUR;
    ANativeWindow_unlockAndPost(fenetre);
}

static void SurCommande(android_app* app, int32_t commande)
{
    if ((commande == APP_CMD_INIT_WINDOW || commande == APP_CMD_WINDOW_REDRAW_NEEDED)
        && app->window != nullptr)
        Peindre(app->window);
}

extern "C" void android_main(android_app* app)
{
    app->onAppCmd = SurCommande;

    MASALLE_LOG("Mesure en cours, patientez...");
    std::string rapport = Mesurer();
    // Une ligne de journal par ligne du rapport.
    size_t debut = 0;
    while (debut < rapport.size()) {
        size_t fin = rapport.find('\n', debut);
        if (fin == std::string::npos) fin = rapport.size();
        MASALLE_LOG("%s", rapport.substr(debut, fin - debut).c_str());
        debut = fin + 1;
    }

    while (!app->destroyRequested) {
        int evenements = 0;
        android_poll_source* source = nullptr;
        if (ALooper_pollOnce(-1, nullptr, &evenements,
                             reinterpret_cast<void**>(&source)) >= 0 && source)
            source->process(app, source);
    }
}

// =============================================================================
//  Windows et Linux
// =============================================================================
#else
#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

int main()
{
    std::string rapport = Mesurer();
#if defined(_WIN32)
    MessageBoxA(nullptr, rapport.c_str(), "MaSalle - exercice 21", MB_OK | MB_ICONINFORMATION);
#else
    std::printf("%s\n", rapport.c_str());
#endif
    return 0;
}
#endif
