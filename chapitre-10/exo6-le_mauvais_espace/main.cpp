// Chapitre 10, exercice 6 : le mauvais espace.
// La vraie pose d'un oeil, dans STAGE, depend de l'espace ou elle a ete
// demandee (D). Le compositeur, lui, la lit dans l'espace declare dans la
// couche (C). Si les deux different, tout est decale de l'ancre de LOCAL :
// une hauteur de personne.
#include <iostream>
#include <string>

struct Pose {
    long long x = 0, y = 0, z = 0;
    int k = 0;
};

static int Normaliser(long long k) {
    return static_cast<int>(((k % 4) + 4) % 4);
}

// Tourne (x, z) de k quarts de tour ; y ne tourne pas.
static void Tourner(long long x, long long z, int k, long long &ox, long long &oz) {
    switch (Normaliser(k)) {
        case 1: ox = z; oz = -x; break;
        case 2: ox = -x; oz = -z; break;
        case 3: ox = -z; oz = x; break;
        default: ox = x; oz = z; break;
    }
}

// Ancre * p : passage de LOCAL a STAGE (tourner, puis ajouter l'ancre).
static Pose Composer(const Pose &ancre, const Pose &p) {
    Pose r;
    long long tx = 0, tz = 0;
    Tourner(p.x, p.z, ancre.k, tx, tz);
    r.x = ancre.x + tx;
    r.y = ancre.y + p.y;
    r.z = ancre.z + tz;
    r.k = Normaliser(ancre.k + p.k);
    return r;
}

int main() {
    Pose ancre;
    long long ak = 0;
    if (!(std::cin >> ancre.x >> ancre.y >> ancre.z >> ak)) {
        return 0;
    }
    ancre.k = Normaliser(ak);

    long long nbPoses = 0;
    std::cin >> nbPoses;
    long long justes = 0, fausses = 0;

    for (long long i = 1; i <= nbPoses; ++i) {
        std::string d, c;
        Pose p;
        long long k = 0;
        if (!(std::cin >> d >> c >> p.x >> p.y >> p.z >> k)) {
            break;
        }
        p.k = Normaliser(k);

        const Pose vraie = (d == "L") ? Composer(ancre, p) : p;
        const Pose crue = (c == "L") ? Composer(ancre, p) : p;

        const long long dx = crue.x - vraie.x;
        const long long dy = crue.y - vraie.y;
        const long long dz = crue.z - vraie.z;
        const int dk = (crue.k - vraie.k + 4) % 4;

        std::cout << i << " CRU " << crue.x << ' ' << crue.y << ' ' << crue.z << ' ' << crue.k << '\n';
        std::cout << i << " ECART " << dx << ' ' << dy << ' ' << dz << ' ' << dk << '\n';
        if (dx == 0 && dy == 0 && dz == 0 && dk == 0) {
            ++justes;
        } else {
            ++fausses;
        }
    }

    std::cout << "JUSTES " << justes << '\n';
    std::cout << "FAUSSES " << fausses << '\n';
    return 0;
}
