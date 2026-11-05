// Chapitre 8, exercice 7 : l'ecran noir provoque.
// Suit la cible, la chaine et les images d'un oeil, et dit ce que le
// compositeur affiche : la cible courante, rien (ecran noir) ou une
// ancienne cible (image perimee).
#include <cstdint>
#include <iostream>
#include <string>

int main() {
    long long nbOrdres = 0;
    if (!(std::cin >> nbOrdres)) {
        nbOrdres = 0;
    }

    uint64_t prochainePoignee = 101;
    uint64_t poigneeCourante = 0;   // avant la premiere cible
    uint64_t index0 = 0;            // ce que la chaine a range a l'index 0
    long long numeroImage = 0;

    long long affichees = 0;
    long long noires = 0;
    long long perimees = 0;

    for (long long k = 0; k < nbOrdres; ++k) {
        std::string ordre;
        if (!(std::cin >> ordre)) {
            break;
        }
        if (ordre == "cible") {
            long long w = 0, h = 0;
            std::cin >> w >> h;
            poigneeCourante = prochainePoignee++;
            std::cout << "CIBLE " << poigneeCourante << ' ' << w << ' ' << h << '\n';
        } else if (ordre == "chaine") {
            index0 = 0;                     // chaine neuve : rien de range
        } else if (ordre == "enregistre") {
            index0 = poigneeCourante;       // SetImageNative(0, poignee courante)
        } else if (ordre == "image") {
            ++numeroImage;
            if (index0 == 0) {
                std::cout << "IMAGE " << numeroImage << " NOIR\n";
                ++noires;
            } else if (index0 == poigneeCourante) {
                std::cout << "IMAGE " << numeroImage << " AFFICHE " << index0 << '\n';
                ++affichees;
            } else {
                std::cout << "IMAGE " << numeroImage << " PERIMEE " << index0 << '\n';
                ++perimees;
            }
        }
    }

    std::cout << "AFFICHEES " << affichees << '\n';
    std::cout << "NOIRES " << noires << '\n';
    std::cout << "PERIMEES " << perimees << '\n';
    return 0;
}
