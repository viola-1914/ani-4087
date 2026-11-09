// Chapitre 9, exercice 1 : declarer et verifier.
// Declare des actions (CreateAction), attache le jeu d'actions une seule
// fois (AttachActionSet), et dit ce que chaque handle designe.
// Un handle vaut l'index de l'action plus un ; 0 veut dire invalide.
#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <vector>

static int NumeroUsage(const std::string &usage) {
    static const char *const kUsages[] = {
        "SELECT", "GRAB", "MENU", "MOVE", "AIM_POSE", "GRIP_POSE", "HAPTIC",
        "SELECT_LEFT", "GRAB_LEFT", "AIM_POSE_LEFT", "GRIP_POSE_LEFT",
        "HAPTIC_LEFT", "MOVE_RIGHT",
    };
    for (int i = 0; i < 13; ++i) {
        if (usage == kUsages[i]) {
            return i;
        }
    }
    return -1;
}

int main() {
    long long nbOrdres = 0;
    if (!(std::cin >> nbOrdres)) {
        nbOrdres = 0;
    }

    std::vector<std::string> noms;      // noms[h - 1] = nom de l'action h
    std::set<std::string> nomsVus;
    long long nomsEnDouble = 0;
    bool attache = false;
    uint64_t attachees = 0;             // k

    for (long long i = 0; i < nbOrdres; ++i) {
        std::string ordre;
        if (!(std::cin >> ordre)) {
            break;
        }
        if (ordre == "cree") {
            std::string nom, type, usage;
            std::cin >> nom >> type >> usage;
            if (!nomsVus.insert(nom).second) {
                ++nomsEnDouble;          // le nom ne sert qu'au journal
            }
            noms.push_back(nom);
            std::cout << "ACTION " << noms.size() << ' ' << nom << ' ' << type << ' '
                      << NumeroUsage(usage) << '\n';
        } else if (ordre == "attache") {
            if (attache) {
                std::cout << "ATTACHE REFUSE\n";
            } else {
                attache = true;
                attachees = noms.size();
                std::cout << "ATTACHE " << attachees << '\n';
            }
        } else if (ordre == "lit") {
            uint64_t h = 0;
            std::cin >> h;
            if (h == 0 || h > noms.size()) {
                std::cout << "LIT " << h << " INVALIDE\n";
            } else if (!attache || h > attachees) {
                std::cout << "LIT " << h << " INCONNUE\n";
            } else {
                std::cout << "LIT " << h << ' ' << noms[static_cast<size_t>(h - 1)] << '\n';
            }
        }
    }

    std::cout << "ACTIONS " << noms.size() << '\n';
    std::cout << "ATTACHEES " << attachees << '\n';
    std::cout << "NOMS EN DOUBLE " << nomsEnDouble << '\n';
    return 0;
}
