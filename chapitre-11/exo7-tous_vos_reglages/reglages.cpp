// Chapitre 11, exercice 7 : tous vos reglages.
// Classe chaque reglage d'un module selon ce que le code en fait :
//   CACHE     : un fichier lit lui-meme la variable d'environnement (GETENV)
//   FANTOME   : personne ne le lit (pas de LIT)
//   CONSTANTE : lu, mais jamais pose (ni ECRIT ni ENV)
//   MENU      : lu et pose par un champ : un menu pourrait le changer
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

int main() {
    long long r = 0;
    if (!(std::cin >> r)) {
        r = 0;
    }

    std::vector<std::string> ordre;                       // premiere apparition
    std::map<std::string, std::set<std::string>> actions;
    for (long long i = 0; i < r; ++i) {
        std::string nom, action;
        if (!(std::cin >> nom >> action)) {
            break;
        }
        if (actions.count(nom) == 0) {
            ordre.push_back(nom);
        }
        actions[nom].insert(action);
    }

    long long menus = 0;
    for (const std::string &nom : ordre) {
        const std::set<std::string> &a = actions[nom];
        std::string classe;
        if (a.count("GETENV") != 0) {
            classe = "CACHE";            // l'emporte sur tout
        } else if (a.count("LIT") == 0) {
            classe = "FANTOME";
        } else if (a.count("ECRIT") == 0 && a.count("ENV") == 0) {
            classe = "CONSTANTE";        // DECLARE n'est pas une ecriture
        } else {
            classe = "MENU";
            ++menus;
        }
        std::cout << nom << ' ' << classe << '\n';
    }
    std::cout << "MENU " << menus << " SUR " << ordre.size() << '\n';
    return 0;
}
