// Chapitre 9, exercice 6 : la vibration.
// Relit des appels a ApplyHaptic (amplitude, duree, frequence, dans cet
// ordre), remet chaque valeur dans ses bornes et juge la vibration :
// courte (50 a 200 ms) et non muette, c'est BONNE.
#include <iostream>

int main() {
    long long nbAppels = 0;
    if (!(std::cin >> nbAppels)) {
        nbAppels = 0;
    }

    long long bonnes = 0, longues = 0, muettes = 0;
    long long temps = 0;  // 64 bits : peut depasser un int

    for (long long i = 1; i <= nbAppels; ++i) {
        long long a = 0, d = 0, f = 0;   // amplitude, duree, frequence
        if (!(std::cin >> a >> d >> f)) {
            break;
        }

        const long long amplitude = (a < 0) ? 0 : (a > 100 ? 100 : a);

        std::cout << i << ' ' << amplitude << ' ';
        if (d > 0) {
            std::cout << d;
            temps += d;
        } else {
            std::cout << "MIN";          // compte pour 0 dans TEMPS
        }
        std::cout << ' ';
        if (f > 0) {
            std::cout << f;
        } else {
            std::cout << "DEFAUT";
        }

        if (amplitude == 0) {
            std::cout << " MUETTE\n";
            ++muettes;
        } else if (d < 50) {
            std::cout << " BREVE\n";     // MIN compris
        } else if (d <= 200) {
            std::cout << " BONNE\n";
            ++bonnes;
        } else {
            std::cout << " LONGUE\n";
            ++longues;
        }
    }

    std::cout << "BONNES " << bonnes << '\n';
    std::cout << "LONGUES " << longues << '\n';
    std::cout << "MUETTES " << muettes << '\n';
    std::cout << "TEMPS " << temps << '\n';
    return 0;
}
