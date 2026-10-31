// Chapitre 8, exercice 1 : la cible hors ecran.
// Suit la creation d'une cible, son branchement au rendu,
// et repere la boucle qui se fige quand la zone deborde de la cible.
#include <iostream>
#include <string>
#include <vector>

struct Cible {
    long long largeur;
    long long hauteur;
};

int main() {
    long long fenetreL = 0, fenetreH = 0;
    int nbTampons = 0;
    long long nbOrdres = 0;
    if (!(std::cin >> fenetreL >> fenetreH >> nbTampons >> nbOrdres)) {
        std::cout << "IMAGES 0\nETAT LIBRE\n";
        return 0;
    }
    if (nbTampons < 1) {
        nbTampons = 1;
    }

    bool offscreenAllume = false;
    std::vector<Cible> cibles;          // cible k = cibles[k - 1]
    long long destination = 0;          // 0 = fenetre
    long long zoneL = fenetreL, zoneH = fenetreH;
    std::vector<bool> tamponPris(static_cast<size_t>(nbTampons), false);
    long long numeroImage = 0;
    long long imagesAffichees = 0;
    bool bloque = false;

    for (long long i = 0; i < nbOrdres; ++i) {
        std::string ordre;
        if (!(std::cin >> ordre)) {
            break;
        }

        if (ordre == "allume") {
            std::string nom;
            std::cin >> nom;
            if (!bloque && nom == "OFFSCREEN") {
                offscreenAllume = true;
            }
        } else if (ordre == "cible") {
            long long w = 0, h = 0;
            std::cin >> w >> h;
            if (bloque) {
                continue;
            }
            if (!offscreenAllume) {
                std::cout << "CIBLE NULLE\n";
            } else {
                cibles.push_back({w, h});
                std::cout << "CIBLE " << cibles.size() << ' ' << w << ' ' << h << '\n';
            }
        } else if (ordre == "vers") {
            long long k = 0;
            std::cin >> k;
            if (bloque) {
                continue;
            }
            if (k == 0 || (k >= 1 && k <= static_cast<long long>(cibles.size()))) {
                destination = k;
            } else {
                std::cout << "VERS REFUSE\n";
            }
        } else if (ordre == "taille") {
            long long w = 0, h = 0;
            std::cin >> w >> h;
            if (!bloque) {
                zoneL = w;
                zoneH = h;
            }
        } else if (ordre == "image") {
            if (bloque) {
                continue;
            }
            ++numeroImage;
            size_t tampon = static_cast<size_t>((numeroImage - 1) % nbTampons);
            if (tamponPris[tampon]) {
                std::cout << "BLOQUE " << numeroImage << '\n';
                bloque = true;
                continue;
            }
            ++imagesAffichees;
            if (destination == 0) {
                std::cout << "IMAGE " << numeroImage << " FENETRE\n";
            } else {
                const Cible& c = cibles[static_cast<size_t>(destination - 1)];
                std::cout << "IMAGE " << numeroImage << " CIBLE " << destination << '\n';
                if (zoneL > c.largeur || zoneH > c.hauteur) {
                    tamponPris[tampon] = true;   // travail jamais termine
                }
            }
        }
    }

    std::cout << "IMAGES " << imagesAffichees << '\n';
    std::cout << "ETAT " << (bloque ? "BLOQUE" : "LIBRE") << '\n';
    return 0;
}
