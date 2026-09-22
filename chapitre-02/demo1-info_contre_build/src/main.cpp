// MaSalle — le programme qui grandira au fil du livre.
// Pour l'instant, il salue et s'arrete.

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