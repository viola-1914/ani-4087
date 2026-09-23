// MaSalle — exercice 20 : lire un fichier de ressources.
//
// Deux lectures du MEME chemin "Resources/salle.txt" :
//   1) directement avec fopen ;
//   2) en passant par un petit systeme de fichiers, bati sur le modele de
//      NKFileSystem (le module du moteur Nkentseu) : chemin unique cote code,
//      et sous Android l'AAssetManager va chercher le fichier dans l'APK.
//
// Sous Windows et Linux le programme ecrit son rapport (boite de message ou
// console) ; sous Android il l'ecrit dans le journal, etiquette "MaSalle".

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>

static const char* CHEMIN = "Resources/salle.txt";

#if defined(__ANDROID__)
    #include <android_native_app_glue.h>
    #include <android/native_window.h>
    #include <android/asset_manager.h>
    #include <android/log.h>
    #include <cstdint>
    #define MASALLE_LOG(...) __android_log_print(ANDROID_LOG_INFO, "MaSalle", __VA_ARGS__)
#else
    #if defined(_WIN32)
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN
        #endif
        #include <windows.h>
    #endif
#endif

// =============================================================================
//  Essai 1 : fopen, tel quel
// =============================================================================
static std::string EssaiFopen(const char* chemin)
{
    std::FILE* f = std::fopen(chemin, "rb");
    if (!f)
        return std::string("fopen(\"") + chemin + "\") : ECHEC — errno "
               + std::to_string(errno) + " (" + std::strerror(errno) + ")";

    char premiere[128] = {0};
    if (!std::fgets(premiere, sizeof(premiere), f))
        premiere[0] = '\0';
    std::fseek(f, 0, SEEK_END);
    long taille = std::ftell(f);
    std::fclose(f);

    std::string ligne(premiere);
    while (!ligne.empty() && (ligne.back() == '\n' || ligne.back() == '\r'))
        ligne.pop_back();
    return std::string("fopen(\"") + chemin + "\") : OK — " + std::to_string(taille)
           + " octets, 1re ligne : \"" + ligne + "\"";
}

// =============================================================================
//  Essai 2 : le systeme de fichiers, sur le modele de NKFileSystem
// =============================================================================
#if defined(__ANDROID__)
// Meme principe que NkFile.cpp du moteur : un AAssetManager global, pose au
// demarrage, et le prefixe "Resources/" retire, car androidassets() a copie le
// CONTENU du dossier dans assets/ de l'APK.
static AAssetManager* gGestionnaireAssets = nullptr;

static void PoserGestionnaireAssets(AAssetManager* mgr) { gGestionnaireAssets = mgr; }

static std::string EssaiSystemeDeFichiers(const char* chemin)
{
    if (!gGestionnaireAssets)
        return "systeme de fichiers : ECHEC — aucun gestionnaire d'assets";

    const char* relatif = chemin;
    if (std::strncmp(relatif, "Resources/", 10) == 0)
        relatif += 10;                       // "Resources/salle.txt" -> "salle.txt"

    AAsset* asset = AAssetManager_open(gGestionnaireAssets, relatif, AASSET_MODE_BUFFER);
    if (!asset)
        return std::string("systeme de fichiers (\"") + chemin + "\") : ECHEC — asset \""
               + relatif + "\" introuvable dans l'APK";

    off_t taille = AAsset_getLength(asset);
    std::string contenu(static_cast<size_t>(taille), '\0');
    AAsset_read(asset, &contenu[0], static_cast<size_t>(taille));
    AAsset_close(asset);

    std::string ligne = contenu.substr(0, contenu.find('\n'));
    while (!ligne.empty() && (ligne.back() == '\n' || ligne.back() == '\r'))
        ligne.pop_back();
    return std::string("systeme de fichiers (\"") + chemin + "\") : OK — "
           + std::to_string(static_cast<long>(taille)) + " octets, 1re ligne : \"" + ligne + "\"";
}
#else
// Sur un ordinateur, le systeme de fichiers du moteur lit le disque : meme
// resultat que fopen, mais le code appelant, lui, ne change pas d'une
// plateforme a l'autre.
static std::string EssaiSystemeDeFichiers(const char* chemin)
{
    std::string r = EssaiFopen(chemin);
    return "systeme de fichiers -> disque : " + r;
}
#endif

// =============================================================================
//  Android
// =============================================================================
#if defined(__ANDROID__)
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
    PoserGestionnaireAssets(app->activity->assetManager);

    MASALLE_LOG("Lecture de la ressource %s", CHEMIN);
    MASALLE_LOG("%s", EssaiFopen(CHEMIN).c_str());
    MASALLE_LOG("%s", EssaiSystemeDeFichiers(CHEMIN).c_str());

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
int main()
{
    std::string rapport = std::string("Lecture de la ressource ") + CHEMIN + "\n\n"
                        + EssaiFopen(CHEMIN) + "\n\n"
                        + EssaiSystemeDeFichiers(CHEMIN) + "\n";
#if defined(_WIN32)
    MessageBoxA(nullptr, rapport.c_str(), "MaSalle — exercice 20", MB_OK | MB_ICONINFORMATION);
#else
    std::printf("%s", rapport.c_str());
#endif
    return 0;
}
#endif
