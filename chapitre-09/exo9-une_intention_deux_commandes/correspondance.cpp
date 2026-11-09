// Chapitre 9, exercice 9 : une intention, deux commandes.
// Les actions (nom + usage) sont declarees UNE fois. Deux tables de
// correspondance usage -> commande sont ensuite rejouees sur les memes
// intentions : seule la table change, jamais le reste du code.
// Aucun nom de bouton n'apparait dans ce fichier : les commandes ne
// viennent que des tables lues en entree.
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

struct Action {
    std::string nom;
    std::string usage;   // l'intention, jamais un bouton
};

// La table de correspondance : le SEUL morceau qui depend du materiel.
using Table = std::map<std::string, std::string>;  // usage -> commande

static Table LireTable() {
    Table table;
    int nbLignes = 0;
    std::cin >> nbLignes;
    for (int i = 0; i < nbLignes; ++i) {
        std::string usage, commande;
        std::cin >> usage >> commande;
        table[usage] = commande;
    }
    return table;
}

// Rejoue les images avec une table. Ne lit l'etat qu'a travers la table.
static std::vector<std::string> Rejouer(int numero, const std::vector<Action> &actions,
                                        const Table &table) {
    std::cout << "TABLE " << numero << '\n';
    std::vector<bool> active(actions.size(), false);
    for (size_t a = 0; a < actions.size(); ++a) {
        active[a] = table.count(actions[a].usage) != 0;
        if (!active[a]) {
            std::cout << "INACTIVE " << actions[a].nom << '\n';
        }
    }

    std::vector<std::string> lignes;
    std::vector<bool> appuyee(actions.size(), false);  // rien d'appuye au depart
    long long nbImages = 0;
    std::cin >> nbImages;
    for (long long t = 1; t <= nbImages; ++t) {
        int k = 0;
        std::cin >> k;
        std::set<std::string> tenues;
        for (int i = 0; i < k; ++i) {
            std::string commande;
            std::cin >> commande;
            tenues.insert(commande);
        }

        std::string ligne = std::to_string(t);
        bool unAppui = false;
        for (size_t a = 0; a < actions.size(); ++a) {
            const bool maintenant =
                active[a] && tenues.count(table.at(actions[a].usage)) != 0;
            if (maintenant && !appuyee[a]) {   // vient d'etre appuyee : un front
                ligne += ' ' + actions[a].nom;
                unAppui = true;
            }
            appuyee[a] = maintenant;
        }
        if (!unAppui) {
            ligne += " -";
        }
        std::cout << ligne << '\n';
        lignes.push_back(ligne);
    }
    return lignes;
}

int main() {
    int nbActions = 0;
    if (!(std::cin >> nbActions)) {
        return 0;
    }
    std::vector<Action> actions(static_cast<size_t>(nbActions));
    for (Action &a : actions) {
        std::cin >> a.nom >> a.usage;
    }

    const Table table1 = LireTable();
    const std::vector<std::string> images1 = Rejouer(1, actions, table1);
    const Table table2 = LireTable();
    const std::vector<std::string> images2 = Rejouer(2, actions, table2);

    std::cout << "IDENTIQUES " << (images1 == images2 ? "OUI" : "NON") << '\n';
    return 0;
}
