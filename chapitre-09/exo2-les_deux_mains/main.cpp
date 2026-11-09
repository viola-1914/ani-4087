// Chapitre 9, exercice 2 : les deux mains.
// Dit a quelle main et a quelle commande du bureau le simulateur lie chaque
// action, puis lit ces actions image par image.
#include <iostream>
#include <set>
#include <string>
#include <vector>

struct Liaison {
    const char *usage;
    const char *main;
    const char *commande;
};

// La table du simulateur. Sans suffixe : main droite, sauf MENU et MOVE.
static const Liaison kTable[] = {
    {"SELECT", "DROITE", "SOURIS_G"},
    {"GRAB", "DROITE", "SOURIS_D"},
    {"MENU", "GAUCHE", "TAB"},
    {"MOVE", "GAUCHE", "FLECHES"},
    {"AIM_POSE", "DROITE", "AUCUNE"},
    {"GRIP_POSE", "DROITE", "AUCUNE"},
    {"HAPTIC", "DROITE", "AUCUNE"},
    {"SELECT_LEFT", "GAUCHE", "SOURIS_M"},
    {"GRAB_LEFT", "GAUCHE", "MAJ_G"},
    {"AIM_POSE_LEFT", "GAUCHE", "AUCUNE"},
    {"GRIP_POSE_LEFT", "GAUCHE", "AUCUNE"},
    {"HAPTIC_LEFT", "GAUCHE", "AUCUNE"},
    {"MOVE_RIGHT", "DROITE", "IJKL"},
};

static const Liaison &Trouver(const std::string &usage) {
    for (const Liaison &l : kTable) {
        if (usage == l.usage) {
            return l;
        }
    }
    return kTable[6];  // usage inconnu : traite comme une action sans commande
}

// Manche a deux axes : x = droite - gauche, y = haut - bas.
static std::string Manche(const std::set<std::string> &tenues, const char *droite,
                          const char *gauche, const char *haut, const char *bas) {
    const int x = (tenues.count(droite) ? 1 : 0) - (tenues.count(gauche) ? 1 : 0);
    const int y = (tenues.count(haut) ? 1 : 0) - (tenues.count(bas) ? 1 : 0);
    return std::to_string(x) + "," + std::to_string(y);
}

int main() {
    int nbActions = 0;
    long long nbImages = 0;
    if (!(std::cin >> nbActions >> nbImages)) {
        return 0;
    }

    std::vector<std::string> noms;
    std::vector<const Liaison *> liaisons;
    int droite = 0;
    int gauche = 0;
    for (int a = 0; a < nbActions; ++a) {
        std::string nom, usage;
        std::cin >> nom >> usage;
        const Liaison &l = Trouver(usage);
        noms.push_back(nom);
        liaisons.push_back(&l);
        if (std::string(l.main) == "DROITE") {
            ++droite;
        } else {
            ++gauche;
        }
        std::cout << nom << ' ' << l.main << ' ' << l.commande << '\n';
    }

    const std::set<std::string> kTete = {"Z", "Q", "S", "D", "W", "A"};
    long long imagesTete = 0;

    for (long long t = 1; t <= nbImages; ++t) {
        int k = 0;
        std::cin >> k;
        std::set<std::string> tenues;
        bool tete = false;
        for (int i = 0; i < k; ++i) {
            std::string commande;
            std::cin >> commande;
            tenues.insert(commande);
            if (kTete.count(commande) != 0) {
                tete = true;
            }
        }
        if (tete) {
            ++imagesTete;
        }

        std::cout << t;
        for (const Liaison *l : liaisons) {
            const std::string commande = l->commande;
            std::cout << ' ';
            if (commande == "AUCUNE") {
                std::cout << '-';
            } else if (commande == "FLECHES") {
                std::cout << Manche(tenues, "F_DROITE", "F_GAUCHE", "F_HAUT", "F_BAS");
            } else if (commande == "IJKL") {
                std::cout << Manche(tenues, "L", "J", "I", "K");
            } else {
                std::cout << (tenues.count(commande) ? 1 : 0);
            }
        }
        std::cout << '\n';
    }

    std::cout << "DROITE " << droite << '\n';
    std::cout << "GAUCHE " << gauche << '\n';
    std::cout << "TETE " << imagesTete << '\n';
    return 0;
}
