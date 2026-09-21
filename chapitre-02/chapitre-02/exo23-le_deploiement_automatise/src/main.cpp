// MaSalle — le programme qui grandira au fil du livre.
// Pour l'instant : sous Windows et Linux il salue, sous Android (le Quest 2)
// il remplit l'ecran d'une seule couleur.

#if defined(__ANDROID__)
// =============================================================================
//  Android : application native (NativeActivity + android_native_app_glue)
// =============================================================================
#include <android_native_app_glue.h>
#include <android/native_window.h>
#include <android/log.h>
#include <cstdint>

#define MASALLE_LOG(...) __android_log_print(ANDROID_LOG_INFO, "MaSalle", __VA_ARGS__)

// Bleu-vert (R=0x1E, G=0x8C, B=0x8C). En RGBA_8888, un pixel se lit en memoire
// R, G, B, A : en entier 32 bits petit-boutiste, cela donne 0xAABBGGRR.
static const uint32_t COULEUR = 0xFF8C8C1Eu;

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
    static bool dejaAnnonce = false;        // une seule fois, au demarrage
    if (dejaAnnonce) return;
    dejaAnnonce = true;
    MASALLE_LOG("3/3 Ecran rempli en bleu-vert : %d x %d pixels", tampon.width, tampon.height);
}

static void SurCommande(android_app* app, int32_t commande)
{
    if (commande == APP_CMD_INIT_WINDOW && app->window != nullptr) {
        MASALLE_LOG("2/3 Fenetre prete : %d x %d pixels",
                    ANativeWindow_getWidth(app->window), ANativeWindow_getHeight(app->window));
        Peindre(app->window);
    } else if (commande == APP_CMD_WINDOW_REDRAW_NEEDED && app->window != nullptr) {
        Peindre(app->window);
    }
}

extern "C" void android_main(android_app* app)
{
    MASALLE_LOG("1/3 Bienvenue dans MaSalle ! Demarrage de l'application native.");
    app->onAppCmd = SurCommande;
    while (!app->destroyRequested) {
        int evenements = 0;
        android_poll_source* source = nullptr;
        if (ALooper_pollOnce(-1, nullptr, &evenements,
                             reinterpret_cast<void**>(&source)) >= 0 && source)
            source->process(app, source);
    }
}

#else
// =============================================================================
//  Windows et Linux : le programme salue et s'arrete (exercice 12)
// =============================================================================
#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN     // deja pose par le filtre Windows du .jenga
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>   // MessageBoxW -> user32, deja lie au chapitre 2
#else
    #include <cstdio>
#endif

int main()
{
#if defined(_WIN32)
    // MaSalle est une application fenetree : sous Windows elle n'a pas de
    // console, un std::cout ne s'afficherait nulle part. On salue par une boite.
    MessageBoxW(nullptr,
                L"Bienvenue dans MaSalle !\nLe projet est pret pour tout le livre.",
                L"MaSalle",
                MB_OK | MB_ICONINFORMATION);
#else
    std::printf("Bienvenue dans MaSalle !\nLe projet est pret pour tout le livre.\n");
#endif
    return 0;
}
#endif
