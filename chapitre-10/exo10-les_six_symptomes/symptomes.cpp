// Chapitre 10, exercice 10 : les six symptomes.
// Relie chaque faute de fin de trame a ce qu'on verrait sur un casque, et dit
// si le simulateur en ecrit quelque chose. Le simulateur ne reprojette pas :
// il ne voit ni l'instant, ni la pose, ni l'espace de la couche.
#include <iostream>
#include <string>
#include <vector>

int main() {
    long long n = 0;
    if (!(std::cin >> n)) {
        n = 0;
    }
    long long muettes = 0;

    for (long long i = 1; i <= n; ++i) {
        std::string fin, instant, pose, espace;
        int g = 1, d = 1;
        if (!(std::cin >> fin >> g >> d >> instant >> pose >> espace)) {
            break;
        }

        // Les symptomes sur un casque, dans l'ordre du modele.
        std::vector<std::string> symptomes;
        if (fin == "JAMAIS") {
            symptomes.push_back("FIGE");
        } else if (fin == "SI_RENDU") {
            symptomes.push_back("NE_REPART_PAS");   // la fenetre est reduite au moins une fois
        }
        if (g == 0) {
            symptomes.push_back("OEIL_NOIR_G");
        }
        if (d == 0) {
            symptomes.push_back("OEIL_NOIR_D");
        }
        if (instant == "N") {
            symptomes.push_back("TREMBLE");
        }
        if (pose == "F") {
            symptomes.push_back("DE_TRAVERS");
        }
        if (espace == "L") {
            symptomes.push_back("DECALE");
        }

        std::cout << i << " CASQUE";
        if (symptomes.empty()) {
            std::cout << " AUCUN";
        }
        for (const std::string &s : symptomes) {
            std::cout << ' ' << s;
        }
        std::cout << '\n';

        // Le simulateur ne voit que la discipline des trames et les chaines.
        const bool dit = (fin != "TOUJOURS") || g == 0 || d == 0;
        std::cout << i << " SIMULATEUR " << (dit ? "DIT" : "MUET") << '\n';

        if (!symptomes.empty() && !dit) {
            ++muettes;
        }
    }

    std::cout << "MUETTES " << muettes << '\n';
    return 0;
}
