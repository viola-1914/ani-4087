//
// main.cpp
// =============================================================================
// Description :
//   Pour chaque cube, calcule la hauteur de son bas et de son haut, dit s'il
//   repose sur le sol et donne la hauteur de centre qui le pose au sol.
//   Affiche ensuite le bilan A CORRIGER / PIRE.
//
// Caracteristiques :
//   - Le cube du moteur est centre sur son origine : on utilise la
//     DEMI-hauteur e / 2, jamais la hauteur entiere.
//   - SOUS LE SOL est teste avant ENTERRE.
// =============================================================================

#include <iostream>
#include <string>

static std::string Verdict(long long bas, long long haut) {
    // Un haut qui touche le sol a zero laisse le cube entierement dessous.
    if (haut <= 0) {
        return "SOUS LE SOL";
    }
    if (bas < 0) {
        return "ENTERRE";
    }
    if (bas == 0) {
        return "POSE";
    }
    return "FLOTTE";
}

int main() {
    long long nombreCubes = 0;
    if (!(std::cin >> nombreCubes)) {
        nombreCubes = 0;
    }

    long long aCorriger = 0;
    long long pire = 0;

    for (long long i = 0; i < nombreCubes; ++i) {
        std::string nom;
        long long echelle = 0;
        long long centre = 0;

        // Une entree tronquee arrete proprement la lecture.
        if (!(std::cin >> nom >> echelle >> centre)) {
            break;
        }

        // L'echelle est paire : la demi-hauteur est exacte en entiers.
        const long long demiHauteur = echelle / 2;
        const long long bas = centre - demiHauteur;
        const long long haut = centre + demiHauteur;
        const std::string verdict = Verdict(bas, haut);

        if (verdict != "POSE") {
            ++aCorriger;
        }

        // Un cube qui flotte compte aussi : on prend la valeur absolue.
        long long ecart = bas;
        if (ecart < 0) {
            ecart = -ecart;
        }
        if (ecart > pire) {
            pire = ecart;
        }

        // La hauteur qui pose le cube ne depend pas de son centre actuel.
        std::cout << nom << ' ' << bas << ' ' << haut << ' ' << verdict << ' ' << demiHauteur << '\n' << std::flush;
    }

    std::cout << "A CORRIGER " << aCorriger << '\n';
    std::cout << "PIRE " << pire << '\n';
    return 0;
}
