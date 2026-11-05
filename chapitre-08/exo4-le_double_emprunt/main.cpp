// Chapitre 8, exercice 4 : le double emprunt.
// Montre ce que rendent AcquireImage et ReleaseImage appeles deux fois de
// suite, et ce que devient la variable de l'appelant quand l'emprunt est
// refuse : elle n'est pas ecrite.
#include <cstdint>
#include <iostream>
#include <string>

int main() {
    uint32_t n = 1;
    uint32_t v = 0;
    long long nbOrdres = 0;
    if (!(std::cin >> n >> v >> nbOrdres)) {
        std::cout << "EMPRUNTEE NON\nPROCHAINE 0\n";
        return 0;
    }
    if (n < 1) {
        n = 1;
    }

    uint32_t variables[26];
    for (uint32_t &x : variables) {
        x = v;
    }
    uint32_t prochaine = 0;
    bool empruntee = false;

    for (long long i = 0; i < nbOrdres; ++i) {
        std::string ordre;
        if (!(std::cin >> ordre)) {
            break;
        }
        if (ordre == "acquire") {
            std::string nom;
            std::cin >> nom;
            const char lettre = nom.empty() ? 'a' : nom[0];
            uint32_t &x = variables[(lettre >= 'a' && lettre <= 'z') ? lettre - 'a' : 0];
            bool ok = false;
            if (!empruntee) {
                x = prochaine;      // ecrite seulement si l'emprunt reussit
                empruntee = true;
                ok = true;
            }
            std::cout << "ACQUIRE " << (ok ? "VRAI " : "FAUX ") << lettre << ' ' << x << '\n';
        } else if (ordre == "release") {
            if (empruntee) {
                prochaine = (prochaine + 1) % n;
                empruntee = false;
                std::cout << "RELEASE VRAI\n";
            } else {
                std::cout << "RELEASE FAUX\n";
            }
        } else if (ordre == "last") {
            std::cout << "LAST " << (prochaine + n - 1) % n << '\n';
        }
    }

    std::cout << "EMPRUNTEE " << (empruntee ? "OUI" : "NON") << '\n';
    std::cout << "PROCHAINE " << prochaine << '\n';
    return 0;
}
