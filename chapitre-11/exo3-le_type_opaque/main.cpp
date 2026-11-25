// Chapitre 11, exercice 3 : le type opaque.
// Suit les inclusions d'un projet, par niveaux (parcours en largeur), et dit
// quelles unites .cpp atteignent un en-tete externe (comme openxr.h).
// Le chemin est reconstruit grace au parent de chaque fichier rencontre.
#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

static bool FinitParCpp(const std::string &nom) {
    return nom.size() >= 4 && nom.compare(nom.size() - 4, 4, ".cpp") == 0;
}

int main() {
    long long nbExternes = 0;
    std::set<std::string> externes;
    if (std::cin >> nbExternes) {
        for (long long i = 0; i < nbExternes; ++i) {
            std::string nom;
            std::cin >> nom;
            externes.insert(nom);
        }
    }

    long long nbFichiers = 0;
    std::cin >> nbFichiers;
    std::vector<std::string> ordre;                       // fichiers dans l'ordre de l'entree
    std::map<std::string, std::vector<std::string>> inclut;
    for (long long f = 0; f < nbFichiers; ++f) {
        std::string nom;
        long long k = 0;
        if (!(std::cin >> nom >> k)) {
            break;
        }
        std::vector<std::string> liste;
        for (long long j = 0; j < k; ++j) {
            std::string inclus;
            std::cin >> inclus;
            liste.push_back(inclus);
        }
        if (inclut.count(nom) == 0) {
            ordre.push_back(nom);
        }
        inclut[nom] = liste;
    }

    long long touchees = 0, unites = 0;
    for (const std::string &unite : ordre) {
        if (!FinitParCpp(unite)) {
            continue;  // un .h n'est jamais une unite
        }
        ++unites;

        // Parcours par niveaux depuis l'unite.
        std::map<std::string, std::string> parent;   // fichier -> celui qui l'a fait rencontrer
        std::set<std::string> vus = {unite};
        std::deque<std::string> file = {unite};
        std::string trouve;
        while (!file.empty() && trouve.empty()) {
            const std::string courant = file.front();
            file.pop_front();
            const auto it = inclut.find(courant);
            if (it == inclut.end()) {
                continue;  // en-tete du systeme ou externe : n'inclut rien
            }
            for (const std::string &suivant : it->second) {
                if (!vus.insert(suivant).second) {
                    continue;  // deja rencontre : on ne le reprend pas
                }
                parent[suivant] = courant;
                if (externes.count(suivant) != 0) {
                    trouve = suivant;
                    break;
                }
                file.push_back(suivant);
            }
        }

        if (trouve.empty()) {
            std::cout << unite << " NON\n";
            continue;
        }
        std::vector<std::string> chemin;
        for (std::string n = trouve; ; n = parent[n]) {
            chemin.push_back(n);
            if (n == unite) {
                break;
            }
        }
        std::cout << unite << " OUI ";
        for (size_t i = chemin.size(); i-- > 0;) {
            std::cout << chemin[i] << (i > 0 ? ">" : "");
        }
        std::cout << '\n';
        ++touchees;
    }

    std::cout << "TOUCHEES " << touchees << " SUR " << unites << '\n';
    return 0;
}
