//
// salle_apres.cpp
// =============================================================================
// Description :
//   MaSalle, version APRES les fonctions de pose. La hauteur du centre n'est
//   plus jamais ecrite a la main : PoserAuSol et PoserSurTable la calculent.
//   Memes dimensions que salle_avant.cpp, meme sortie.
// =============================================================================

#include <iostream>
#include <string>

struct Position {
    long long x;
    long long y;
    long long z;
};

// Pose un objet de hauteur sy sur une surface de hauteur H.
static Position PoserSurTable(long long x, long long z, long long sy, long long H) {
    Position centre;
    centre.x = x;
    centre.y = H + sy / 2;
    centre.z = z;
    return centre;
}

// Pose un objet de hauteur sy au sol, surface de hauteur 0.
static Position PoserAuSol(long long x, long long z, long long sy) {
    return PoserSurTable(x, z, sy, 0);
}

static void Afficher(const std::string &nom, const Position &centre) {
    std::cout << nom << ' ' << centre.x << ' ' << centre.y << ' ' << centre.z << '\n';
}

int main() {
    const long long hauteurMur = 2800;
    const long long hauteurTable = 800;
    const long long plateau = 40;
    const long long pied = 60;
    const long long dx = 1200 / 2 - pied;
    const long long dz = 800 / 2 - pied;

    // Le sol est une dalle de 100 dont le dessus est a 0.
    Afficher("sol", PoserSurTable(0, 0, 100, -100));

    Afficher("mur_fond", PoserAuSol(0, -2050, hauteurMur));
    Afficher("mur_entree", PoserAuSol(0, 2050, hauteurMur));
    Afficher("mur_gauche", PoserAuSol(-3050, 0, hauteurMur));
    Afficher("mur_droit", PoserAuSol(3050, 0, hauteurMur));

    Afficher("porte", PoserAuSol(-1500, 1980, 2000));

    // Les fenetres reposent sur une allege de 900.
    Afficher("fenetre_fond", PoserSurTable(-1500, -1980, 1200, 900));
    Afficher("fenetre_droite", PoserSurTable(2980, 500, 1200, 900));

    // Le plateau repose sur le haut des pieds.
    Afficher("plateau", PoserSurTable(500, 0, plateau, hauteurTable - plateau));
    Afficher("pied", PoserAuSol(500 - dx, 0 - dz, hauteurTable - plateau));
    Afficher("pied", PoserAuSol(500 + dx, 0 - dz, hauteurTable - plateau));
    Afficher("pied", PoserAuSol(500 - dx, 0 + dz, hauteurTable - plateau));
    Afficher("pied", PoserAuSol(500 + dx, 0 + dz, hauteurTable - plateau));

    Afficher("ordinateur", PoserSurTable(400, 0, 20, hauteurTable));
    Afficher("lampe", PoserSurTable(900, -200, 450, hauteurTable));
    return 0;
}
