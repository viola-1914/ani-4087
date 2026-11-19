// Chapitre 10, exercice 3 : le blocage provoque.
// Suit la session image par image : chaque BeginFrame ouvre une trame que
// seul EndFrame ferme. La boucle SAUTE n'appelle EndFrame que si elle rend :
// une pause laisse la trame ouverte, et tout est refuse ensuite.
#include <iostream>
#include <string>

int main() {
    std::string politique;
    long long nbImages = 0;
    if (!(std::cin >> politique >> nbImages)) {
        return 0;
    }
    const bool toujours = (politique == "TOUJOURS");
    std::string signes;
    if (nbImages > 0) {
        std::cin >> signes;
    }

    bool trameOuverte = false;
    long long rendues = 0, perdues = 0, erreurs = 0, premierRefus = 0;

    for (long long t = 1; t <= nbImages; ++t) {
        const char s = (t - 1 < static_cast<long long>(signes.size()))
                           ? signes[static_cast<size_t>(t - 1)] : '0';
        bool rendre = false;  // etat neuf

        // WaitFrame
        const bool waitOk = !trameOuverte;
        if (waitOk) {
            rendre = (s == '1');
        } else {
            ++erreurs;
            if (premierRefus == 0) {
                premierRefus = t;
            }
        }

        // BeginFrame
        const bool beginOk = !trameOuverte;
        if (beginOk) {
            trameOuverte = true;
        } else {
            ++erreurs;
        }

        if (rendre) {
            ++rendues;
        } else if (s == '1') {
            ++perdues;
        }

        // EndFrame : toujours, ou seulement si l'on rend.
        std::string fin = "-";
        if (toujours || rendre) {
            if (trameOuverte) {
                trameOuverte = false;
                fin = "OK";
            } else {
                fin = "REFUSE";
                ++erreurs;
            }
        }

        std::cout << t << ' ' << (waitOk ? "OK" : "REFUSE") << ' ' << (beginOk ? "OK" : "REFUSE")
                  << ' ' << (rendre ? "OUI" : "NON") << ' ' << fin << '\n';
    }

    std::cout << "RENDUES " << rendues << '\n';
    std::cout << "PERDUES " << perdues << '\n';
    std::cout << "ERREURS " << erreurs << '\n';
    std::cout << "PREMIER REFUS ";
    if (premierRefus == 0) {
        std::cout << "AUCUN\n";
    } else {
        std::cout << premierRefus << '\n';
    }
    return 0;
}
