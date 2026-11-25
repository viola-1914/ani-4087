// Chapitre 11, exercice 5 : les quatre chemins.
// Rejoue la recherche du casque par le backend, dans l'ordre des replis :
//   variable d'environnement -> loader standard -> decouverte directe
// puis le repli de l'application : casque, simulateur sur carte, ou
// simulateur logiciel. Toute bibliotheque ouverte pour rien est liberee.
#include <iostream>
#include <string>

int main() {
    long long k = 0;
    if (!(std::cin >> k) || k <= 0) {
        std::cout << "AUCUNE MACHINE\n";
        return 0;
    }

    for (long long i = 1; i <= k; ++i) {
        std::string d, v, l, r, b, g;
        if (!(std::cin >> d >> v >> l >> r >> b >> g)) {
            break;
        }
        long long chargees = 0, liberees = 0;
        bool casque = false;

        if (d == "SIMULATEUR") {
            std::cout << i << " BACKEND SIMULATEUR DEMANDE\n";   // aucun chemin essaye
        } else {
            bool charge = false;        // une bibliotheque est chargee
            bool entree = false;        // elle a son point d'entree
            std::string source;

            // 1. La variable d'environnement.
            if (v == "ABSENT") {
                std::cout << i << " VARIABLE IGNOREE\n";
            } else if (v == "BON" || v == "SANS") {
                std::cout << i << " VARIABLE CHARGEE\n";
                ++chargees;
                charge = true;
                entree = (v == "BON");
                source = "VARIABLE";
            }

            // 2. Le loader standard, seulement si rien n'est charge.
            if (!charge) {
                if (l == "ABSENT") {
                    std::cout << i << " LOADER ABSENT\n";
                } else if (l == "BON" || l == "SANS") {
                    std::cout << i << " LOADER CHARGE\n";
                    ++chargees;
                    charge = true;
                    entree = (l == "BON");
                    source = "LOADER";
                }
            }

            // Une bibliotheque sans point d'entree est liberee.
            if (charge && !entree) {
                std::cout << i << " SANS_ENTREE\n";
                ++liberees;
                charge = false;
            }

            // 3. La decouverte directe par le registre.
            if (!entree) {
                std::cout << i << " DIRECT " << r << '\n';
                if (r == "NEGO_KO" || r == "BON") {
                    ++chargees;
                    charge = true;
                    entree = (r == "BON");
                    source = "DIRECT";
                }
            }

            if (!entree) {
                std::cout << i << " BACKEND INDISPONIBLE\n";
                if (charge) {
                    ++liberees;          // NEGO_KO : rien ne doit rester
                    charge = false;
                }
            } else {
                std::cout << i << " BACKEND OPENXR " << source << '\n';
                if (b == "NON") {
                    std::cout << i << " LIAISON ECHOUEE\n";
                    ++liberees;          // session detruite, bibliotheque liberee
                    charge = false;
                } else {
                    casque = true;
                }
            }
        }

        // Le niveau : on ne refuse jamais de demarrer.
        if (casque) {
            std::cout << i << " NIVEAU 1 CASQUE\n";
        } else if (g == "OUI") {
            std::cout << i << " NIVEAU 2 SIMULATEUR CARTE\n";
        } else {
            std::cout << i << " NIVEAU 3 SIMULATEUR LOGICIEL\n";
        }
        std::cout << i << " CHARGEES " << chargees << " LIBEREES " << liberees << '\n';
    }
    return 0;
}
