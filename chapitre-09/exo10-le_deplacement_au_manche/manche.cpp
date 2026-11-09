// Chapitre 9, exercice 10 : le deplacement au manche.
// Deplace le joueur avec l'action MOVE par un DECALAGE DU MONDE :
//   decalage += v * (y * avant + x * droite)
// ou avant et droite suivent le lacet de la tete, a plat au sol.
// La pose de la tete n'est jamais ecrite : elle vient du casque.
#include <iostream>
#include <set>
#include <string>

struct Sol {
    long long x = 0;
    long long z = 0;
};

// Avant et droite de la tete au sol, selon son lacet en quarts de tour.
static void Repere(int k, Sol &avant, Sol &droite) {
    switch (((k % 4) + 4) % 4) {
        case 1: avant = {-1, 0}; droite = {0, -1}; break;
        case 2: avant = {0, 1}; droite = {-1, 0}; break;
        case 3: avant = {1, 0}; droite = {0, 1}; break;
        default: avant = {0, -1}; droite = {1, 0}; break;
    }
}

int main() {
    long long v = 0;
    long long nbImages = 0;
    if (!(std::cin >> v >> nbImages)) {
        return 0;
    }

    Sol decalage;            // (0, 0) au depart ; seule chose que l'on ecrit
    long long diagonales = 0;

    for (long long t = 1; t <= nbImages; ++t) {
        int k = 0, n = 0;
        std::cin >> k >> n;
        std::set<std::string> tenues;
        for (int i = 0; i < n; ++i) {
            std::string fleche;
            std::cin >> fleche;
            tenues.insert(fleche);
        }

        // Le manche : deux fleches opposees s'annulent.
        const int x = (tenues.count("F_DROITE") ? 1 : 0) - (tenues.count("F_GAUCHE") ? 1 : 0);
        const int y = (tenues.count("F_HAUT") ? 1 : 0) - (tenues.count("F_BAS") ? 1 : 0);
        if (x != 0 && y != 0) {
            ++diagonales;
        }

        Sol avant, droite;
        Repere(k, avant, droite);
        decalage.x += v * (y * avant.x + x * droite.x);
        decalage.z += v * (y * avant.z + x * droite.z);

        std::cout << t << ' ' << x << ',' << y << ' ' << decalage.x << ' ' << decalage.z << '\n';
    }

    std::cout << "DECALAGE " << decalage.x << ' ' << decalage.z << '\n';
    std::cout << "DIAGONALES " << diagonales << '\n';
    return 0;
}
