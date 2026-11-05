// Chapitre 8, exercice 6 : l'identifiant opaque suivi.
// La chaine range des poignees de 64 bits sans savoir ce qu'elles designent
// (SetImageNative / GetImageNative). Le programme relit chaque poignee et
// dit si la texture designee existe encore.
#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <vector>

int main() {
    uint64_t n = 1;
    long long nbOrdres = 0;
    if (!(std::cin >> n >> nbOrdres)) {
        std::cout << "MORTES 0\nLARGES 0\n";
        return 0;
    }

    const uint64_t kMax32 = 4294967295ull;
    std::vector<uint64_t> emplacements(static_cast<size_t>(n > 4 ? 4 : n), 0);
    const uint64_t taille = emplacements.size();
    std::set<uint64_t> textures;

    long long mortes = 0;
    long long larges = 0;

    for (long long k = 0; k < nbOrdres; ++k) {
        std::string ordre;
        if (!(std::cin >> ordre)) {
            break;
        }
        if (ordre == "cree") {
            uint64_t h = 0;
            std::cin >> h;
            textures.insert(h);
        } else if (ordre == "detruit") {
            uint64_t h = 0;
            std::cin >> h;
            textures.erase(h);   // l'emplacement garde le vieux nombre
        } else if (ordre == "set") {
            uint64_t i = 0, h = 0;
            std::cin >> i >> h;
            if (i >= taille) {
                std::cout << "SET IGNORE " << i << '\n';
            } else {
                emplacements[static_cast<size_t>(i)] = h;
                if (h > kMax32) {
                    ++larges;
                }
            }
        } else if (ordre == "get") {
            uint64_t i = 0;
            std::cin >> i;
            const uint64_t h = (i < taille) ? emplacements[static_cast<size_t>(i)] : 0;
            std::cout << "GET " << i << ' ' << h << ' ';
            if (h == 0) {
                std::cout << "VIDE\n";
            } else if (textures.count(h) != 0) {
                std::cout << "VIVANTE\n";
            } else {
                std::cout << "MORTE\n";
                ++mortes;
            }
        }
    }

    std::cout << "MORTES " << mortes << '\n';
    std::cout << "LARGES " << larges << '\n';
    return 0;
}
