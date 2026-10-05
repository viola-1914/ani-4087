//
// poser.cpp
// =============================================================================
// Description :
//   Batit une table (plateau et quatre pieds) puis pose des objets au sol ou
//   sur la table, uniquement avec deux fonctions : PoserAuSol et PoserSurTable.
//
// Caracteristiques :
//   - Un objet est un pave centre sur son origine : le poser sur une hauteur
//     de base, c'est mettre son centre a base + sy / 2, jamais a base + sy.
//   - Le calcul de la demi-hauteur n'est ecrit qu'a un seul endroit.
// =============================================================================

#include <iostream>
#include <string>

// Centre d'un objet, en millimetres.
struct Position {
    long long x;
    long long y;
    long long z;
};

// Pose un objet de hauteur sy sur une surface horizontale de hauteur H.
// Son centre est a H + sy / 2 : c'est la demi-hauteur qui compte.
static Position PoserSurTable(long long x, long long z, long long sy, long long H) {
    Position centre;
    centre.x = x;
    centre.y = H + sy / 2;
    centre.z = z;
    return centre;
}

// Pose un objet de hauteur sy au sol : le sol est une surface de hauteur 0.
static Position PoserAuSol(long long x, long long z, long long sy) {
    return PoserSurTable(x, z, sy, 0);
}

static void Afficher(const std::string &etiquette, const Position &centre) {
    std::cout << etiquette << ' ' << centre.x << ' ' << centre.y << ' ' << centre.z << '\n';
}

int main() {
    long long L = 0;
    long long P = 0;
    long long H = 0;
    long long ep = 0;
    long long pied = 0;
    long long tx = 0;
    long long tz = 0;
    if (!(std::cin >> L >> P >> H >> ep >> pied >> tx >> tz)) {
        return 0;
    }

    // Un pied s'arrete sous le plateau : il mesure H - ep, pas H.
    const long long hauteurPied = H - ep;

    // Le plateau repose sur le haut des pieds, a la hauteur H - ep.
    // Son centre tombe donc a H - ep / 2, sous le dessus annonce H.
    Afficher("PLATEAU", PoserSurTable(tx, tz, ep, hauteurPied));

    // Ordre impose : moins-moins, plus-moins, moins-plus, plus-plus.
    const long long dx = L / 2 - pied;
    const long long dz = P / 2 - pied;
    Afficher("PIED", PoserAuSol(tx - dx, tz - dz, hauteurPied));
    Afficher("PIED", PoserAuSol(tx + dx, tz - dz, hauteurPied));
    Afficher("PIED", PoserAuSol(tx - dx, tz + dz, hauteurPied));
    Afficher("PIED", PoserAuSol(tx + dx, tz + dz, hauteurPied));

    long long nombreObjets = 0;
    if (!(std::cin >> nombreObjets)) {
        nombreObjets = 0;
    }

    for (long long i = 0; i < nombreObjets; ++i) {
        std::string nom;
        long long sx = 0;
        long long sy = 0;
        long long sz = 0;
        long long x = 0;
        long long z = 0;
        std::string ou;

        // Une entree tronquee arrete proprement la lecture.
        if (!(std::cin >> nom >> sx >> sy >> sz >> x >> z >> ou)) {
            break;
        }

        if (ou == "TABLE") {
            Afficher(nom, PoserSurTable(x, z, sy, H));
        } else {
            Afficher(nom, PoserAuSol(x, z, sy));
        }
    }

    return 0;
}
