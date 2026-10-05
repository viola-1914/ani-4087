//
// salle_avant.cpp
// =============================================================================
// Description :
//   MaSalle, version AVANT les fonctions de pose. Chaque piece est placee en
//   ecrivant a la main la hauteur de son centre.
//   Dimensions du plan du chapitre 1 (exercice 10), en millimetres :
//   salle 6000 x 4000 x 2800, porte 900 x 2000, deux fenetres 1500 x 1200,
//   table 1200 x 800 x 800, un ordinateur portable et une lampe sur la table.
// =============================================================================

#include <iostream>
#include <string>

static void Afficher(const std::string &nom, long long x, long long y, long long z) {
    std::cout << nom << ' ' << x << ' ' << y << ' ' << z << '\n';
}

int main() {
    // Sol : dalle de 100, dessus a 0.
    Afficher("sol", 0, 0 - 100 / 2, 0);

    // Murs de 2800 de haut, 100 d'epaisseur, poses au sol.
    Afficher("mur_fond", 0, 2800 / 2, -2050);
    Afficher("mur_entree", 0, 2800 / 2, 2050);
    Afficher("mur_gauche", -3050, 2800 / 2, 0);
    Afficher("mur_droit", 3050, 2800 / 2, 0);

    // Porte de 2000 de haut, posee au sol, contre le mur de l'entree.
    Afficher("porte", -1500, 2000 / 2, 1980);

    // Fenetres de 1200 de haut, posees sur une allege de 900.
    Afficher("fenetre_fond", -1500, 900 + 1200 / 2, -1980);
    Afficher("fenetre_droite", 2980, 900 + 1200 / 2, 500);

    // Table : dessus a 800, plateau de 40, pieds de 60 de cote.
    Afficher("plateau", 500, 800 - 40 / 2, 0);
    Afficher("pied", 500 - (1200 / 2 - 60), (800 - 40) / 2, 0 - (800 / 2 - 60));
    Afficher("pied", 500 + (1200 / 2 - 60), (800 - 40) / 2, 0 - (800 / 2 - 60));
    Afficher("pied", 500 - (1200 / 2 - 60), (800 - 40) / 2, 0 + (800 / 2 - 60));
    Afficher("pied", 500 + (1200 / 2 - 60), (800 - 40) / 2, 0 + (800 / 2 - 60));

    // Objets poses sur la table.
    Afficher("ordinateur", 400, 800 + 20 / 2, 0);
    Afficher("lampe", 900, 800 + 450 / 2, -200);
    return 0;
}
