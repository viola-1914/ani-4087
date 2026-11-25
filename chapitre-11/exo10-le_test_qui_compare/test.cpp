// Chapitre 11, exercice 10 : le test qui compare.
// Un test deterministe : une tete SCRIPTEE (position, lacet, pas par image)
// et une horloge SCRIPTEE (instant = t x periode, jamais l'heure reelle).
// Il produit le meme fichier, octet pour octet, a chaque execution.
#include <cstdint>
#include <iostream>

int main() {
    int64_t hz = 0, ipd = 0, n = 0;
    if (!(std::cin >> hz >> ipd >> n)) {
        return 0;
    }
    int64_t x = 0, y = 0, z = 0, k0 = 0, dk = 0, vx = 0;
    std::cin >> x >> y >> z >> k0 >> dk >> vx;

    const int64_t periode = 1000000000LL / (hz <= 1 ? 72 : hz);
    std::cout << "PERIODE " << periode << '\n';

    // La droite de la tete, vue de dessus (x, z), selon le lacet.
    const int64_t droiteX[4] = {1, 0, -1, 0};
    const int64_t droiteZ[4] = {0, -1, 0, 1};
    const int64_t demi = ipd / 2;

    for (int64_t t = 1; t <= n; ++t) {
        const int64_t lacet = (((k0 + (t - 1) * dk) % 4) + 4) % 4;
        const int64_t tx = x + (t - 1) * vx;
        const int64_t rx = droiteX[lacet], rz = droiteZ[lacet];

        const int64_t gx = tx - demi * rx, gz = z - demi * rz;   // oeil gauche
        const int64_t dx = tx + demi * rx, dz = z + demi * rz;   // oeil droit
        const int64_t instant = t * periode;                     // horloge scriptee

        std::cout << t << " G " << gx << ' ' << y << ' ' << gz << " D " << dx << ' ' << y << ' ' << dz
                  << " I " << instant << '\n';
    }
    return 0;
}
