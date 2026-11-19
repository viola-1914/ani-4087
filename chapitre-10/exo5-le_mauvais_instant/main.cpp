// Chapitre 10, exercice 5 : le mauvais instant.
// La couche doit porter la pose du RENDU (w x P), pas la pose relue au
// moment de soumettre (w x S). Le casque laisse a l'ecran
//   CASQUE = pose soumise - lacet du rendu
// et le simulateur, qui ne reprojette pas, montre
//   SIMULATEUR = w x A - w x P, quelle que soit la pose soumise.
// Tout est calcule sur 64 bits.
#include <cstdint>
#include <iostream>
#include <string>

static int64_t Absolu(int64_t v) {
    return v < 0 ? -v : v;
}

int main() {
    int64_t w = 0;
    long long nbImages = 0;
    if (!(std::cin >> w >> nbImages)) {
        std::cout << "FAUSSES 0\nPIRE CASQUE 0\nPIRE SIMULATEUR 0\n";
        return 0;
    }

    long long fausses = 0;
    int64_t pireCasque = 0, pireSimulateur = 0;

    for (long long t = 1; t <= nbImages; ++t) {
        int64_t p = 0, s = 0, a = 0;
        std::string m;
        if (!(std::cin >> p >> s >> a >> m)) {
            break;
        }

        // Ecarts calcules sur les differences d'instants : w x S - w x P = w x (S - P).
        const int64_t casque = (m == "F") ? w * (s - p) : 0;
        const int64_t simulateur = w * (a - p);

        std::cout << t << " CASQUE " << casque << " SIMULATEUR " << simulateur << '\n';
        if (casque != 0) {
            ++fausses;
        }
        if (Absolu(casque) > pireCasque) {
            pireCasque = Absolu(casque);
        }
        if (Absolu(simulateur) > pireSimulateur) {
            pireSimulateur = Absolu(simulateur);
        }
    }

    std::cout << "FAUSSES " << fausses << '\n';
    std::cout << "PIRE CASQUE " << pireCasque << '\n';
    std::cout << "PIRE SIMULATEUR " << pireSimulateur << '\n';
    return 0;
}
