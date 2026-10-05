//
// main.cpp
// =============================================================================
// Description :
//   Calcule ou un objet se retrouve quand on ecrit l'echelle avant la
//   translation (Scale * Translate) au lieu de l'inverse, et l'ecart avec
//   la bonne position. Affiche ensuite le bilan DEPLACES / PIRE.
//
// Caracteristiques :
//   - Chaque axe reste chez lui : sx ne touche que tx, sy que ty, sz que tz.
//   - On multiplie AVANT de diviser, sinon 100 / 1000 vaut deja 0.
//   - La division entiere du C++ tronque vers zero : -79,2 donne -79.
// =============================================================================

#include <iostream>
#include <string>

// Position obtenue par le mauvais ordre sur un axe.
static long long MauvaisePosition(long long echelle, long long translation) {
    return echelle * translation / 1000;
}

static long long Absolu(long long valeur) {
    if (valeur < 0) {
        return -valeur;
    }
    return valeur;
}

static long long Plus(long long a, long long b) {
    if (a > b) {
        return a;
    }
    return b;
}

int main() {
    long long nombreObjets = 0;
    if (!(std::cin >> nombreObjets)) {
        nombreObjets = 0;
    }

    long long deplaces = 0;
    long long pire = 0;

    for (long long i = 0; i < nombreObjets; ++i) {
        std::string nom;
        long long tx = 0;
        long long ty = 0;
        long long tz = 0;
        long long sx = 0;
        long long sy = 0;
        long long sz = 0;

        // Une entree tronquee arrete proprement la lecture.
        if (!(std::cin >> nom >> tx >> ty >> tz >> sx >> sy >> sz)) {
            break;
        }

        const long long x = MauvaisePosition(sx, tx);
        const long long y = MauvaisePosition(sy, ty);
        const long long z = MauvaisePosition(sz, tz);

        // L'ecart retenu est le plus grand des trois, en valeur absolue.
        long long ecart = Absolu(tx - x);
        ecart = Plus(ecart, Absolu(ty - y));
        ecart = Plus(ecart, Absolu(tz - z));

        if (ecart != 0) {
            ++deplaces;
        }
        pire = Plus(pire, ecart);

        std::cout << nom << ' ' << x << ' ' << y << ' ' << z << ' ' << ecart << '\n' << std::flush;
    }

    std::cout << "DEPLACES " << deplaces << '\n';
    std::cout << "PIRE " << pire << '\n';
    return 0;
}
