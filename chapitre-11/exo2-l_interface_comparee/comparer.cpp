// Chapitre 11, exercice 2 : l'interface comparee.
// Compare les methodes publiques de deux backends : A (le vrai casque) et
// B (le simulateur). Affiche le nombre de noms communs, les noms de A seul,
// ceux de B seul, puis combien de noms different sur combien en tout.
#include <iostream>
#include <set>
#include <string>
#include <vector>

// Lit une liste : un nombre, puis les noms. Garde l'ordre, sans doublon.
static std::vector<std::string> LireListe() {
    long long n = 0;
    std::vector<std::string> noms;
    if (!(std::cin >> n)) {
        return noms;
    }
    std::set<std::string> vus;
    for (long long i = 0; i < n; ++i) {
        std::string nom;
        if (!(std::cin >> nom)) {
            break;
        }
        if (vus.insert(nom).second) {
            noms.push_back(nom);
        }
    }
    return noms;
}

int main() {
    const std::vector<std::string> a = LireListe();   // vrai casque
    const std::vector<std::string> b = LireListe();   // simulateur
    const std::set<std::string> ensembleA(a.begin(), a.end());
    const std::set<std::string> ensembleB(b.begin(), b.end());

    long long deux = 0;
    for (const std::string &nom : a) {
        if (ensembleB.count(nom) != 0) {
            ++deux;
        }
    }
    std::cout << "DEUX " << deux << '\n';

    long long differentes = 0;
    for (const std::string &nom : a) {
        if (ensembleB.count(nom) == 0) {
            std::cout << "SEUL_A " << nom << '\n';
            ++differentes;
        }
    }
    for (const std::string &nom : b) {
        if (ensembleA.count(nom) == 0) {
            std::cout << "SEUL_B " << nom << '\n';
            ++differentes;
        }
    }

    // u : les noms des deux listes reunies, chacun compte une fois.
    const long long reunis = static_cast<long long>(a.size() + b.size()) - deux;
    std::cout << "DIFFERENTES " << differentes << " SUR " << reunis << '\n';
    return 0;
}
