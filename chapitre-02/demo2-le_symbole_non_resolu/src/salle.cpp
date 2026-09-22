#include "salle.h"
#include "geometrie.h"
#include <cstdio>
void DecrireSalle()
{
    using namespace geometrie;
    const Point coins[4] = { {0, 0}, {5, 0}, {5, 4}, {0, 4} };
    std::printf("Mur nord    : %.2f m\n", LongueurMur(coins[3], coins[2]));
    std::printf("Mur est     : %.2f m\n", LongueurMur(coins[1], coins[2]));
    std::printf("Surface     : %.2f m2\n", AirePolygone(coins, 4));
    std::printf("Perimetre   : %.2f m\n", Perimetre(coins, 4));
    Point c = Centre(coins, 4);
    std::printf("Centre      : (%.2f, %.2f)\n", c.x, c.y);
}
