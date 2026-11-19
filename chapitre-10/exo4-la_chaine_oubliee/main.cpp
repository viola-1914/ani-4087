// Chapitre 10, exercice 4 : la chaine oubliee.
// Rejoue les appels d'une boucle un par un et dit ce que repondent la
// session, le simulateur et les deux chaines d'images (une par oeil).
// Une projection sans chaine pour un oeil est avertie tout de suite.
#include <iostream>
#include <string>

struct Chaine {
    long long index = 0;
    bool empruntee = false;
};

int main() {
    long long k = 1;
    long long nbAppels = 0;
    if (!(std::cin >> k >> nbAppels)) {
        return 0;
    }
    if (k < 1) {
        k = 1;
    }

    bool trameOuverte = false;
    Chaine chaines[2];  // 0 = gauche, 1 = droit
    long long journal = 0, avertissements = 0, silencieux = 0;

    for (long long a = 1; a <= nbAppels; ++a) {
        std::string appel;
        if (!(std::cin >> appel)) {
            break;
        }
        std::cout << a << ' ' << appel << ' ';

        if (appel == "wait") {
            if (trameOuverte) {
                std::cout << "REFUSE\n";
                ++journal;
            } else {
                std::cout << "OK\n";
            }
        } else if (appel == "begin") {
            if (trameOuverte) {
                std::cout << "REFUSE\n";
                ++journal;
            } else {
                trameOuverte = true;
                std::cout << "OK\n";
            }
        } else if (appel == "acquire" || appel == "release") {
            std::string oeil;
            std::cin >> oeil;
            Chaine &c = chaines[oeil == "D" ? 1 : 0];
            if (appel == "acquire") {
                if (c.empruntee) {
                    std::cout << "REFUSE\n";   // refus silencieux
                    ++silencieux;
                } else {
                    c.empruntee = true;
                    std::cout << "OK " << c.index << '\n';
                }
            } else {
                if (!c.empruntee) {
                    std::cout << "REFUSE\n";
                    ++silencieux;
                } else {
                    c.empruntee = false;
                    c.index = (c.index + 1) % k;
                    std::cout << "OK\n";
                }
            }
        } else if (appel == "end") {
            std::string g;
            std::cin >> g;
            const bool projection = (g != "-");
            bool renseignee[2] = {false, false};
            if (projection) {
                std::string d;
                std::cin >> d;
                renseignee[0] = (g == "1");
                renseignee[1] = (d == "1");
            }

            if (!trameOuverte) {
                std::cout << "REFUSE AUCUNE\n";
                ++journal;
                continue;
            }
            trameOuverte = false;  // la trame se ferme dans tous les cas

            if (projection && renseignee[0] && chaines[0].empruntee) {
                std::cout << "REFUSE ACQUISE G\n";
                ++journal;
            } else if (projection && renseignee[1] && chaines[1].empruntee) {
                std::cout << "REFUSE ACQUISE D\n";
                ++journal;
            } else {
                std::cout << "OK";
                if (projection && (!renseignee[0] || !renseignee[1])) {
                    std::cout << " AVERT";
                    if (!renseignee[0]) {
                        std::cout << " G";
                        ++avertissements;
                    }
                    if (!renseignee[1]) {
                        std::cout << " D";
                        ++avertissements;
                    }
                }
                std::cout << '\n';
            }
        } else {
            std::cout << "REFUSE\n";
        }
    }

    std::cout << "JOURNAL " << journal << '\n';
    std::cout << "AVERTISSEMENTS " << avertissements << '\n';
    std::cout << "SILENCIEUX " << silencieux << '\n';
    return 0;
}
