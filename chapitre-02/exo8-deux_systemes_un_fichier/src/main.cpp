#include <iostream>

#if defined(EXO8_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>   // GetSystemMetrics -> user32, GetStockObject -> gdi32
#elif defined(EXO8_LINUX)
    #include <pthread.h>   // pthread_create / pthread_join -> pthread
    #include <cmath>       // std::cbrt -> m (libm)
#endif

#if defined(EXO8_LINUX)
static void* travail(void* arg) {
    double* x = static_cast<double*>(arg);
    *x = std::cbrt(*x);
    return nullptr;
}
#endif

int main() {
#if defined(EXO8_WINDOWS)
    std::cout << "Systeme : Windows (filtre system:Windows applique)" << std::endl;
    int largeur = GetSystemMetrics(SM_CXSCREEN);                 // user32
    HGDIOBJ pinceau = GetStockObject(WHITE_BRUSH);                // gdi32
    std::cout << "user32 : largeur de l'ecran = " << largeur << " px" << std::endl;
    std::cout << "gdi32  : pinceau blanc " << (pinceau ? "obtenu" : "introuvable") << std::endl;
#elif defined(EXO8_LINUX)
    std::cout << "Systeme : Linux (filtre system:Linux applique)" << std::endl;
    double valeur = 27.0;
    pthread_t fil;
    pthread_create(&fil, nullptr, travail, &valeur);             // pthread
    pthread_join(fil, nullptr);
    std::cout << "pthread + m : racine cubique de 27 calculee dans un fil = " << valeur << std::endl;
#else
    std::cout << "Aucun filtre de systeme applique." << std::endl;
#endif
    return 0;
}
