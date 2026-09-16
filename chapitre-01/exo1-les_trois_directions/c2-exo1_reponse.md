# Exercice 1 — Les trois directions

Dans la convention utilisée par le module :

- Avant correspond à l'axe -Z ;
- Haut correspond à l'axe +Y ;
- Droite correspond à l'axe +X.

Les trois vecteurs unitaires sont donc :

- Avant : (0, 0, -1)
- Haut : (0, 1, 0)
- Droite : (1, 0, 0)

## Code

```cpp
#include <iostream>
#include <iomanip>

struct Vec3
{
    double x;
    double y;
    double z;
};

Vec3 Avant()
{
    return {0.0, 0.0, -1.0};
}

Vec3 Haut()
{
    return {0.0, 1.0, 0.0};
}

Vec3 Droite()
{
    return {1.0, 0.0, 0.0};
}

double ProduitScalaire(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

int main()
{
    Vec3 point;

    std::cin >> point.x >> point.y >> point.z;

    std::cout << std::fixed << std::setprecision(4);

    std::cout << ProduitScalaire(point, Avant()) << '\n';
    std::cout << ProduitScalaire(point, Haut()) << '\n';
    std::cout << ProduitScalaire(point, Droite()) << '\n';

    return 0;
}
