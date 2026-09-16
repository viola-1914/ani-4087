# Démo 3 — Le tour complet à l'envers

Cette démonstration montre ce qui peut arriver lorsqu'on calcule la différence
entre deux orientations sans forcer le chemin court entre les quaternions.

Deux quaternions `q` et `-q` représentent la même orientation physique.
Cependant, si leurs signes ne sont pas pris en compte, une petite rotation
peut être interprétée comme presque un tour complet dans le sens opposé.

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

double produitScalaire(
    const Quaternion& a,
    const Quaternion& b)
{
    return a.w*b.w +
           a.x*b.x +
           a.y*b.y +
           a.z*b.z;
}

Quaternion conjugue(const Quaternion& q)
{
    return {q.w, -q.x, -q.y, -q.z};
}

Quaternion multiplier(
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

Vec3 calculerVitesse(
    Quaternion q1,
    Quaternion q2,
    double dt,
    bool cheminCourt)
{
    /*
       Les trois lignes essentielles qui forcent
       le chemin court.
    */
    if (cheminCourt && produitScalaire(q1, q2) < 0.0)
        q2 = {-q2.w, -q2.x, -q2.y, -q2.z};

    Quaternion delta =
        multiplier(q2, conjugue(q1));

    double w = delta.w;

    if (w > 1.0) w = 1.0;
    if (w < -1.0) w = -1.0;

    double angle = 2.0 * std::acos(w);

    double longueur = std::sqrt(
        delta.x*delta.x +
        delta.y*delta.y +
        delta.z*delta.z
    );

    if (longueur < 1e-12 || dt <= 0.0)
        return {0.0, 0.0, 0.0};

    Vec3 axe{
        delta.x / longueur,
        delta.y / longueur,
        delta.z / longueur
    };

    return {
        axe.x * angle / dt,
        axe.y * angle / dt,
        axe.z * angle / dt
    };
}

int main()
{
    const double PI = 3.14159265358979323846;

    // Petite rotation réelle : 2 degrés
    double angle = 2.0 * PI / 180.0;

    Quaternion q1{
        1.0, 0.0, 0.0, 0.0
    };

    /*
       Quaternion opposé représentant pourtant
       la même petite rotation physique.
    */
    Quaternion q2{
        -std::cos(angle / 2.0),
        0.0,
        -std::sin(angle / 2.0),
        0.0
    };

    double dt = 0.1;

    Vec3 sans =
        calculerVitesse(q1, q2, dt, false);

    Vec3 avec =
        calculerVitesse(q1, q2, dt, true);

    std::cout << std::fixed << std::setprecision(4);

    std::cout << "Sans chemin court : "
              << sans.x << " "
              << sans.y << " "
              << sans.z << '\n';

    std::cout << "Avec chemin court : "
              << avec.x << " "
              << avec.y << " "
              << avec.z << '\n';

    return 0;
}
```

## Sans le chemin court

La rotation physique entre les deux orientations n'est que de 2 degrés.

Pourtant, sans correction du signe du quaternion, le programme peut lire
cette différence comme une rotation d'environ 358 degrés dans le sens opposé.

Pour `dt = 0,1 s`, on obtient environ :

`0.0000 -62.4828 0.0000 rad/s`

Cette vitesse est complètement disproportionnée par rapport au petit
mouvement réel.

## Les trois lignes ajoutées

La correction essentielle est :

```cpp
if (cheminCourt && produitScalaire(q1, q2) < 0.0)
    q2 = {-q2.w, -q2.x, -q2.y, -q2.z};
```

Si le produit scalaire des deux quaternions est négatif, je change le signe
du second quaternion.

Son orientation physique ne change pas, puisque `q` et `-q` représentent
la même orientation.

## Avec le chemin court

Après cette correction, le programme retrouve la petite rotation réelle
de 2 degrés.

La vitesse devient environ :

`0.0000 0.3491 0.0000 rad/s`

Cette valeur correspond bien à :

`2 degrés / 0,1 seconde = 20 degrés par seconde`

soit environ :

`0,3491 rad/s`

## Conclusion

Sans le chemin court, un changement minuscule d'orientation peut être
interprété comme presque un tour complet à l'envers.

Après la correction du signe du quaternion, le même mouvement est interprété
comme une petite rotation normale.

Cette démonstration montre pourquoi le forçage du chemin court est nécessaire
pour calculer correctement une vitesse angulaire à partir de quaternions.
