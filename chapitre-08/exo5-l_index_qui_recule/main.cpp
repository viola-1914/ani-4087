// Chapitre 8, exercice 5 : l'index qui recule.
// Calcule l'index de la derniere image relachee avec l'ecriture du moteur,
// (prochaine + n - 1) % n, et le compare a deux ecritures plus courtes,
// (prochaine - 1) % n en int signe puis en uint32_t.
#include <cstdint>
#include <iostream>

int main() {
    long long nbLignes = 0;
    std::cin >> nbLignes;

    long long refusees = 0;
    long long fauxSigne = 0;
    long long fauxNonSigne = 0;

    for (long long i = 0; i < nbLignes; ++i) {
        uint64_t n = 0;
        uint64_t r = 0;
        if (!(std::cin >> n >> r)) {
            break;
        }
        if (n == 0 || n > 4) {
            std::cout << n << ' ' << r << " REFUSEE\n";
            ++refusees;
            continue;
        }

        const uint64_t prochaine = r % n;                         // sur 64 bits
        const uint64_t juste = (prochaine + n - 1) % n;           // l'ecriture du moteur

        const int pSigne = static_cast<int>(prochaine);
        const int nSigne = static_cast<int>(n);
        const int signe = (pSigne - 1) % nSigne;                  // -1 % 3 vaut -1

        const uint32_t pNonSigne = static_cast<uint32_t>(prochaine);
        const uint32_t nNonSigne = static_cast<uint32_t>(n);
        const uint32_t nonSigne = (pNonSigne - 1u) % nNonSigne;   // 0 - 1 vaut 4294967295

        std::cout << n << ' ' << r << ' ' << prochaine << ' ' << juste << ' '
                  << signe << ' ' << nonSigne << '\n';

        if (static_cast<long long>(signe) != static_cast<long long>(juste)) {
            ++fauxSigne;
        }
        if (static_cast<uint64_t>(nonSigne) != juste) {
            ++fauxNonSigne;
        }
    }

    std::cout << "REFUSEES " << refusees << '\n';
    std::cout << "FAUX SIGNE " << fauxSigne << '\n';
    std::cout << "FAUX NON SIGNE " << fauxNonSigne << '\n';
    return 0;
}
