# Exercice 9 — Le chemin court

Deux quaternions opposés `q` et `-q` représentent la même orientation.

Lorsqu'on calcule une vitesse angulaire entre deux orientations, il faut donc
forcer le chemin court. Sinon, une petite variation peut être interprétée
comme une rotation presque complète dans le sens opposé.

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

Vec3 vitesseAngulaire(
    Quaternion q1,
    Quaternion q2,
    double dt,
    bool cheminCourt)
{
    if (dt <= 0.0)
        return {0.0, 0.0, 0.0};

    // q et -q représentent la même orientation.
    // On change le signe de q2 si nécessaire.
    if (cheminCourt && produitScalaire(q1, q2) < 0.0)
    {
        q2.w = -q2.w;
        q2.x = -q2.x;
        q2.y = -q2.y;
        q2.z = -q2.z;
    }

    Quaternion delta =
        multiplier(q2, conjugue(q1));

    // Protection contre les petites erreurs numériques
    double w = delta.w;

    if (w > 1.0) w = 1.0;
    if (w < -1.0) w = -1.0;

    double angle = 2.0 * std::acos(w);

    double s = std::sqrt(
        delta.x*delta.x +
        delta.y*delta.y +
        delta.z*delta.z
    );

    if (s < 1e-12)
        return {0.0, 0.0, 0.0};

    Vec3 axe{
        delta.x / s,
        delta.y / s,
        delta.z / s
    };

    return {
        axe.x * angle / dt,
        axe.y * angle / dt,
        axe.z * angle / dt
    };
}

void afficher(const Vec3& v)
{
    std::cout << v.x << " "
              << v.y << " "
              << v.z << '\n';
}

int main()
{
    const double PI = 3.14159265358979323846;

    /*
       Exemple volontaire :
       q1 = orientation identité.

       q2 représente une petite rotation de 2 degrés autour de Y,
       mais on utilise son quaternion opposé.

       q2 et -q2 représentent pourtant exactement la même orientation.
    */

    double angle = 2.0 * PI / 180.0;

    Quaternion q1{
        1.0, 0.0, 0.0, 0.0
    };

    Quaternion q2{
        -std::cos(angle / 2.0),
        0.0,
        -std::sin(angle / 2.0),
        0.0
    };

    double dt = 0.1;

    Vec3 avec = vitesseAngulaire(
        q1, q2, dt, true
    );

    Vec3 sans = vitesseAngulaire(
        q1, q2, dt, false
    );

    std::cout << std::fixed << std::setprecision(4);

    std::cout << "Avec chemin court : ";
    afficher(avec);

    std::cout << "Sans chemin court : ";
    afficher(sans);

    return 0;
}
```

## Exemple choisi

J'ai pris deux orientations séparées par une petite rotation de 2 degrés
autour de l'axe Y.

Pour la deuxième orientation, j'utilise volontairement le quaternion opposé.
Il représente pourtant exactement la même orientation physique.

La durée choisie est :

`dt = 0,1 s`

### Avec le chemin court

Le produit scalaire entre les deux quaternions est négatif. Le programme
change donc le signe du deuxième quaternion avant de calculer le delta.

Il retrouve alors la petite rotation d'environ 2 degrés.

La vitesse angulaire est d'environ :

`0,0000 0,3491 0,0000 rad/s`

### Sans le chemin court

Sans changement de signe, le delta peut être interprété comme une rotation
d'environ 358 degrés dans l'autre sens.

On obtient alors une vitesse angulaire d'environ :

`0,0000 -62,4828 0,0000 rad/s`

Cette valeur est absurde par rapport au mouvement réel, qui n'était que de
2 degrés en 0,1 seconde.

## Conclusion

Le forçage du chemin court est indispensable lorsqu'on travaille avec des
quaternions.

Comme `q` et `-q` représentent la même orientation, il faut choisir leurs
signes de manière à conserver le chemin de rotation le plus court.

Sans cette précaution, un delta minuscule peut être interprété comme une
rotation presque complète dans le sens opposé, ce qui produit une vitesse
angulaire complètement irréaliste.
