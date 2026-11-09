// Chapitre 9, exercice 8 : paume ou visee.
// Place, comme le simulateur du moteur, la paume et la visee de chaque main
// dans la salle : decalage dans le repere de la tete, tourne du lacet de la
// tete (k quarts de tour), puis ajoute a la position de la tete.
// Dit ensuite laquelle des deux poses droites peut saisir un objet.
#include <cmath>
#include <iostream>
#include <vector>

struct Vec3 {
    long long x = 0;
    long long y = 0;
    long long z = 0;
};

// Decalages dans le repere de la tete (main droite), en millimetres.
static const Vec3 kPaumeDroite = {180, -300, -250};
static const Vec3 kViseeDroite = {180, -250, -400};

// Tourne (x, z) de k quarts de tour, puis ajoute la position de la tete.
static Vec3 VersSalle(const Vec3 &tete, int k, const Vec3 &local) {
    long long x = local.x, z = local.z;
    switch (((k % 4) + 4) % 4) {
        case 1: x = local.z; z = -local.x; break;
        case 2: x = -local.x; z = -local.z; break;
        case 3: x = -local.z; z = local.x; break;
        default: break;
    }
    return {tete.x + x, tete.y + local.y, tete.z + z};
}

// La main gauche : x change de signe AVANT la rotation.
static Vec3 Gauche(const Vec3 &droite) {
    return {-droite.x, droite.y, droite.z};
}

static void Afficher(const char *nom, const Vec3 &p) {
    std::cout << nom << ' ' << p.x << ' ' << p.y << ' ' << p.z << '\n';
}

// Objet le plus proche a 150 mm au plus (plus petit numero a egalite), 0 sinon.
static int PlusProche(const Vec3 &main, const std::vector<Vec3> &objets) {
    int meilleur = 0;
    long long meilleure = 0;
    for (size_t j = 0; j < objets.size(); ++j) {
        const long long dx = objets[j].x - main.x;
        const long long dy = objets[j].y - main.y;
        const long long dz = objets[j].z - main.z;
        const long long d2 = dx * dx + dy * dy + dz * dz;
        if (d2 <= 22500 && (meilleur == 0 || d2 < meilleure)) {
            meilleur = static_cast<int>(j) + 1;
            meilleure = d2;
        }
    }
    return meilleur;
}

static void AfficherSaisie(const char *nom, int j) {
    std::cout << "SAISIE " << nom << ' ';
    if (j == 0) {
        std::cout << "AUCUNE\n";
    } else {
        std::cout << j << '\n';
    }
}

int main() {
    int nbTetes = 0, nbObjets = 0;
    if (!(std::cin >> nbTetes >> nbObjets)) {
        return 0;
    }
    std::vector<Vec3> tetes(static_cast<size_t>(nbTetes));
    std::vector<int> lacets(static_cast<size_t>(nbTetes));
    for (int h = 0; h < nbTetes; ++h) {
        std::cin >> tetes[h].x >> tetes[h].y >> tetes[h].z >> lacets[h];
    }
    std::vector<Vec3> objets(static_cast<size_t>(nbObjets));
    for (Vec3 &o : objets) {
        std::cin >> o.x >> o.y >> o.z;
    }

    for (int h = 0; h < nbTetes; ++h) {
        const Vec3 paumeD = VersSalle(tetes[h], lacets[h], kPaumeDroite);
        const Vec3 viseeD = VersSalle(tetes[h], lacets[h], kViseeDroite);
        const Vec3 paumeG = VersSalle(tetes[h], lacets[h], Gauche(kPaumeDroite));
        const Vec3 viseeG = VersSalle(tetes[h], lacets[h], Gauche(kViseeDroite));

        std::cout << "TETE " << h + 1 << '\n';
        Afficher("PAUME D", paumeD);
        Afficher("VISEE D", viseeD);
        Afficher("PAUME G", paumeG);
        Afficher("VISEE G", viseeG);
        AfficherSaisie("PAUME", PlusProche(paumeD, objets));
        AfficherSaisie("VISEE", PlusProche(viseeD, objets));
    }

    // L'ecart ne depend pas de la tete : une rotation garde les distances.
    const double dx = static_cast<double>(kViseeDroite.x - kPaumeDroite.x);
    const double dy = static_cast<double>(kViseeDroite.y - kPaumeDroite.y);
    const double dz = static_cast<double>(kViseeDroite.z - kPaumeDroite.z);
    std::cout << "ECART " << std::llround(std::sqrt(dx * dx + dy * dy + dz * dz)) << '\n';
    return 0;
}
