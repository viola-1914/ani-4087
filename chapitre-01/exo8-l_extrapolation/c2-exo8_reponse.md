# Exercice 8 — L'extrapolation

L'extrapolation consiste à estimer la pose future en supposant que les
vitesses linéaire et angulaire restent constantes pendant une durée `dt`.

La position est avancée avec la vitesse linéaire, tandis que l'orientation
est avancée à partir de la vitesse angulaire.

## Code C++

```cpp
#include <iostream>
#include <iomanip>
#include <cmath>

struct Vec3
{
    double x, y, z;
};

struct Quaternion
{
    double w, x, y, z;
};

struct Pose
{
    Vec3 position;
    Quaternion orientation;
};

double norme(const Vec3& v)
{
    return std::sqrt(
        v.x * v.x +
        v.y * v.y +
        v.z * v.z
    );
}

Quaternion multiplierQuaternion(
    const Quaternion& a,
    const Quaternion& b)
{
    return {
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w
    };
}

Quaternion normaliser(const Quaternion& q)
{
    double n = std::sqrt(
        q.w*q.w +
        q.x*q.x +
        q.y*q.y +
        q.z*q.z
    );

    if (n < 1e-12)
        return {1.0, 0.0, 0.0, 0.0};

    return {
        q.w / n,
        q.x / n,
        q.y / n,
        q.z / n
    };
}

Pose extrapoler(
    const Pose& pose,
    const Vec3& vitesseLineaire,
    const Vec3& vitesseAngulaire,
    double dt)
{
    Pose resultat = pose;

    // Avancement de la position
    resultat.position.x += vitesseLineaire.x * dt;
    resultat.position.y += vitesseLineaire.y * dt;
    resultat.position.z += vitesseLineaire.z * dt;

    // Norme de la vitesse angulaire
    double omega = norme(vitesseAngulaire);

    // Cas particulier : aucune rotation
    if (omega < 1e-12)
    {
        resultat.orientation = pose.orientation;
        return resultat;
    }

    // Axe unitaire de rotation
    Vec3 axe{
        vitesseAngulaire.x / omega,
        vitesseAngulaire.y / omega,
        vitesseAngulaire.z / omega
    };

    // Angle parcouru pendant dt
    double angle = omega * dt;
    double demiAngle = angle / 2.0;

    Quaternion delta{
        std::cos(demiAngle),
        axe.x * std::sin(demiAngle),
        axe.y * std::sin(demiAngle),
        axe.z * std::sin(demiAngle)
    };

    // Mise à jour de l'orientation
    resultat.orientation =
        normaliser(multiplierQuaternion(delta, pose.orientation));

    return resultat;
}

int main()
{
    Pose pose;
    Vec3 vitesseLineaire;
    Vec3 vitesseAngulaire;
    double dt;

    // Position initiale
    std::cin >> pose.position.x
             >> pose.position.y
             >> pose.position.z;

    // Quaternion : w x y z
    std::cin >> pose.orientation.w
             >> pose.orientation.x
             >> pose.orientation.y
             >> pose.orientation.z;

    // Vitesse linéaire en m/s
    std::cin >> vitesseLineaire.x
             >> vitesseLineaire.y
             >> vitesseLineaire.z;

    // Vitesse angulaire en rad/s
    std::cin >> vitesseAngulaire.x
             >> vitesseAngulaire.y
             >> vitesseAngulaire.z;

    // Durée en secondes
    std::cin >> dt;

    Pose future = extrapoler(
        pose,
        vitesseLineaire,
        vitesseAngulaire,
        dt
    );

    std::cout << std::fixed << std::setprecision(4);

    std::cout << future.position.x << " "
              << future.position.y << " "
              << future.position.z << '\n';

    std::cout << future.orientation.w << " "
              << future.orientation.x << " "
              << future.orientation.y << " "
              << future.orientation.z << '\n';

    return 0;
}
```

## Principe

Pour la position, j'utilise :

`position_future = position + vitesse_lineaire * dt`

Pour l'orientation, la norme de la vitesse angulaire donne la vitesse de
rotation en radians par seconde.

L'angle parcouru pendant `dt` est donc :

`angle = |vitesse_angulaire| * dt`

La direction de la vitesse angulaire donne l'axe de rotation.

À partir de cet axe et de cet angle, je construis un quaternion `delta`
qui représente la rotation effectuée pendant `dt`.

Ce quaternion est ensuite composé avec l'orientation initiale.

## Cas d'une vitesse angulaire nulle

Si la norme de la vitesse angulaire est nulle, il ne faut pas essayer de
calculer :

`axe = vitesse_angulaire / |vitesse_angulaire|`

car cela provoquerait une division par zéro.

Dans ce cas, je conserve simplement l'orientation initiale.

## Conclusion

L'extrapolation permet d'estimer la pose future à partir des vitesses
linéaire et angulaire.

Cette estimation est valable sur une courte durée tant que l'hypothèse
de vitesses constantes reste raisonnable.
