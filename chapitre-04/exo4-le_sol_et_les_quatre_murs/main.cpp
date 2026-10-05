//
// main.cpp
// =============================================================================
// Description :
//   Calcule l'emprise au sol de chaque mur, vue de dessus, puis dit si les
//   quatre angles de la piece sont bouches. Affiche enfin le nombre de trous.
//
// Caracteristiques :
//   - Un angle n'est bouche que si UN SEUL mur contient tout son carre :
//     deux murs qui en couvrent chacun une moitie ne suffisent pas.
//   - Les angles sont toujours affiches dans le meme ordre, quel que soit
//     l'ordre des murs.
// =============================================================================

#include <iostream>
#include <string>
#include <vector>

// Rectangle aligne sur les axes, vu de dessus.
struct Rectangle {
    long long xMin;
    long long xMax;
    long long zMin;
    long long zMax;
};

// Vrai si le mur contient entierement le carre de l'angle.
// Les bords qui se touchent comptent : on compare avec <= et >=.
static bool Contient(const Rectangle &mur, const Rectangle &angle) {
    if (mur.xMin > angle.xMin) {
        return false;
    }
    if (mur.xMax < angle.xMax) {
        return false;
    }
    if (mur.zMin > angle.zMin) {
        return false;
    }
    if (mur.zMax < angle.zMax) {
        return false;
    }
    return true;
}

static bool EstBouche(const std::vector<Rectangle> &murs, const Rectangle &angle) {
    for (const Rectangle &mur : murs) {
        if (Contient(mur, angle)) {
            return true;
        }
    }
    return false;
}

int main() {
    long long cote = 0;
    long long epaisseur = 0;
    if (!(std::cin >> cote >> epaisseur)) {
        cote = 0;
        epaisseur = 0;
    }

    long long nombreMurs = 0;
    if (!(std::cin >> nombreMurs)) {
        nombreMurs = 0;
    }

    std::vector<Rectangle> murs;

    for (long long i = 0; i < nombreMurs; ++i) {
        std::string nom;
        long long cx = 0;
        long long cz = 0;
        long long sx = 0;
        long long sz = 0;

        // Une entree tronquee arrete proprement la lecture.
        if (!(std::cin >> nom >> cx >> cz >> sx >> sz)) {
            break;
        }

        // Les tailles sont paires : les demi-tailles sont exactes en entiers.
        Rectangle mur;
        mur.xMin = cx - sx / 2;
        mur.xMax = cx + sx / 2;
        mur.zMin = cz - sz / 2;
        mur.zMax = cz + sz / 2;
        murs.push_back(mur);

        std::cout << nom << ' ' << mur.xMin << ' ' << mur.xMax << ' ' << mur.zMin << ' ' << mur.zMax << '\n' << std::flush;
    }

    // Chaque angle est un carre de cote e, juste a l'exterieur du sol.
    const long long h = cote / 2;
    const std::string nomsAngles[4] = {"FOND_GAUCHE", "FOND_DROIT", "ENTREE_GAUCHE", "ENTREE_DROIT"};
    const Rectangle angles[4] = {
        {-h - epaisseur, -h, -h - epaisseur, -h},
        {h, h + epaisseur, -h - epaisseur, -h},
        {-h - epaisseur, -h, h, h + epaisseur},
        {h, h + epaisseur, h, h + epaisseur},
    };

    long long trous = 0;
    for (int i = 0; i < 4; ++i) {
        if (EstBouche(murs, angles[i])) {
            std::cout << nomsAngles[i] << " BOUCHE" << '\n';
        } else {
            ++trous;
            std::cout << nomsAngles[i] << " TROU" << '\n';
        }
    }

    std::cout << "TROUS " << trous << '\n';
    return 0;
}
