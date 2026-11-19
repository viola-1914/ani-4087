// Chapitre 10, exercice 7 : un panneau en couche quad.
// Place des panneaux (couches quad) dans la salle, calcule leurs quatre
// coins dans STAGE et leur distance a l'oeil. Un panneau sans espace
// precise est dans LOCAL, comme dans le module : l'ancre s'y ajoute.
#include <cmath>
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

int main() {
    Pose ancre;
    long long ak = 0;
    if (!(std::cin >> ancre.x >> ancre.y >> ancre.z >> ak)) {
        std::cout << "PANNEAUX 0\n";
        return 0;
    }
    ancre.k = Normaliser(ak);
    long long ex = 0, ey = 0, ez = 0;
    std::cin >> ex >> ey >> ez;

    long long nbPanneaux = 0;
    std::cin >> nbPanneaux;
    long long lus = 0;

    for (long long i = 1; i <= nbPanneaux; ++i) {
        std::string espace;
        Pose p;
        long long k = 0, w = 0, h = 0;
        if (!(std::cin >> espace >> p.x >> p.y >> p.z >> k >> w >> h)) {
            break;
        }
        ++lus;
        p.k = Normaliser(k);

        // Sans espace precise : LOCAL.
        const bool stage = (espace == "S");
        Pose centre = p;
        if (!stage) {
            long long tx = 0, tz = 0;
            Tourner(p.x, p.z, ancre.k, tx, tz);
            centre.x = ancre.x + tx;
            centre.y = ancre.y + p.y;
            centre.z = ancre.z + tz;
            centre.k = Normaliser(ancre.k + p.k);
        }

        std::cout << "PANNEAU " << i << ' ' << (stage ? "STAGE" : "LOCAL") << '\n';
        const long long cx[4] = {-w / 2, w / 2, w / 2, -w / 2};
        const long long cy[4] = {-h / 2, -h / 2, h / 2, h / 2};
        for (int c = 0; c < 4; ++c) {
            long long tx = 0, tz = 0;
            Tourner(cx[c], 0, centre.k, tx, tz);   // z du coin vaut 0
            std::cout << centre.x + tx << ' ' << centre.y + cy[c] << ' ' << centre.z + tz << '\n';
        }

        const long double dx = static_cast<long double>(centre.x - ex);
        const long double dy = static_cast<long double>(centre.y - ey);
        const long double dz = static_cast<long double>(centre.z - ez);
        std::cout << "DISTANCE " << std::llround(std::sqrt(dx * dx + dy * dy + dz * dz)) << '\n';
    }

    std::cout << "PANNEAUX " << lus << '\n';
    return 0;
}
