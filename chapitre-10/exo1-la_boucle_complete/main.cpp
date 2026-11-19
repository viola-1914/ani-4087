// Chapitre 10, exercice 1 : la boucle complete.
// Relit, image par image, la suite des appels d'une boucle de realite
// virtuelle et trouve la premiere faute de chaque image :
//   WAIT -> BEGIN -> VUES -> (PREND_x -> LACHE_x) par oeil -> END
#include <iostream>
#include <string>

int main() {
    long long nbImages = 0;
    if (!(std::cin >> nbImages)) {
        nbImages = 0;
    }

    long long justes = 0, fautives = 0;

    for (long long t = 1; t <= nbImages; ++t) {
        int n = 0;
        std::cin >> n;

        // Chaque image repart de zero.
        bool attendu = false, commence = false, vues = false, fini = false;
        bool prisG = false, prisD = false;
        int placeFaute = 0;
        std::string code;

        for (int p = 1; p <= n; ++p) {
            std::string appel;
            std::cin >> appel;
            if (placeFaute != 0) {
                continue;  // on lit la suite de la ligne sans l'analyser
            }

            std::string faute;
            if (fini) {
                faute = "APRES_END";
            } else if (appel == "WAIT") {
                if (commence) {
                    faute = "WAIT_APRES_BEGIN";
                } else {
                    attendu = true;
                }
            } else if (appel == "BEGIN") {
                if (!attendu) {
                    faute = "BEGIN_SANS_WAIT";
                } else if (commence) {
                    faute = "DOUBLE_BEGIN";
                } else {
                    commence = true;
                }
            } else if (appel == "VUES") {
                if (!attendu) {
                    faute = "VUES_SANS_WAIT";
                } else {
                    vues = true;
                }
            } else if (appel == "PREND_G" || appel == "PREND_D") {
                bool &pris = (appel == "PREND_G") ? prisG : prisD;
                if (!commence) {
                    faute = "PREND_SANS_BEGIN";
                } else if (!vues) {
                    faute = "PREND_SANS_VUES";
                } else if (pris) {
                    faute = "DOUBLE_PREND";
                } else {
                    pris = true;
                }
            } else if (appel == "LACHE_G" || appel == "LACHE_D") {
                bool &pris = (appel == "LACHE_G") ? prisG : prisD;
                if (!pris) {
                    faute = "LACHE_SANS_PREND";
                } else {
                    pris = false;
                }
            } else if (appel == "END") {
                if (!commence) {
                    faute = "END_SANS_BEGIN";
                } else if (prisG || prisD) {
                    faute = "END_IMAGE_PRISE";
                } else {
                    fini = true;
                }
            }

            if (!faute.empty()) {
                placeFaute = p;
                code = faute;
            }
        }

        if (placeFaute == 0 && !fini) {
            placeFaute = n + 1;
            code = "SANS_END";
        }

        if (placeFaute == 0) {
            std::cout << t << " OK\n";
            ++justes;
        } else {
            std::cout << t << " FAUTE " << placeFaute << ' ' << code << '\n';
            ++fautives;
        }
    }

    std::cout << "JUSTES " << justes << '\n';
    std::cout << "FAUTIVES " << fautives << '\n';
    return 0;
}
