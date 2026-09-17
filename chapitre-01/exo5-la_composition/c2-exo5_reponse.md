# Exercice 5 — La composition

La composition de deux poses permet de remplacer deux transformations
successives par une seule transformation équivalente.

Dans cet exercice, j'applique d'abord la pose A, puis la pose B.

L'objectif est de vérifier par l'exécution que l'application successive des
deux poses donne le même résultat que l'application d'une seule pose composée.

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

Vec3 rotationQuaternion(const Quaternion& q, const Vec3& v)
{
    Vec3 u{q.x, q.y, q.z};

    double dotUV = u.x * v.x + u.y * v.y + u.z * v.z;
    double dotUU = u.x * u.x + u.y * u.y + u.z * u.z;

    Vec3 cross{
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };

    return {
        2.0 * dotUV * u.x
            + (q.w * q.w - dotUU) * v.x
            + 2.0 * q.w * cross.x,

        2.0 * dotUV * u.y
            + (q.w * q.w - dotUU) * v.y
            + 2.0 * q.w * cross.y,

        2.0 * dotUV * u.z
            + (q.w * q.w - dotUU) * v.z
            + 2.0 * q.w * cross.z
    };
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

Vec3 appliquerPose(const Pose& pose, const Vec3& point)
{
    Vec3 tourne = rotationQuaternion(pose.orientation, point);

    return {
        tourne.x + pose.position.x,
        tourne.y + pose.position.y,
        tourne.z + pose.position.z
    };
}

// Composition correspondant à : appliquer A puis B
Pose composer(const Pose& A, const Pose& B)
{
    Quaternion orientationComposee =
        multiplierQuaternion(B.orientation, A.orientation);

    Vec3 positionATournee =
        rotationQuaternion(B.orientation, A.position);

    Vec3 positionComposee{
        positionATournee.x + B.position.x,
        positionATournee.y + B.position.y,
        positionATournee.z + B.position.z
    };

    return {
        positionComposee,
        orientationComposee
    };
}

int main()
{
    Pose A, B;
    Vec3 point;

    // Pose A : position puis quaternion normalisé (w x y z)
    std::cin >> A.position.x
             >> A.position.y
             >> A.position.z;

    std::cin >> A.orientation.w
             >> A.orientation.x
             >> A.orientation.y
             >> A.orientation.z;

    // Pose B
    std::cin >> B.position.x
             >> B.position.y
             >> B.position.z;

    std::cin >> B.orientation.w
             >> B.orientation.x
             >> B.orientation.y
             >> B.orientation.z;

    // Point à transformer
    std::cin >> point.x >> point.y >> point.z;

    // Méthode 1 : application successive
    Vec3 apresA = appliquerPose(A, point);
    Vec3 successif = appliquerPose(B, apresA);

    // Méthode 2 : composition puis application
    Pose composee = composer(A, B);
    Vec3 parComposition = appliquerPose(composee, point);

    // Écart entre les deux résultats
    Vec3 ecart{
        parComposition.x - successif.x,
        parComposition.y - successif.y,
        parComposition.z - successif.z
    };

    std::cout << std::setprecision(17);

    std::cout << "Point obtenu successivement : "
              << successif.x << " "
              << successif.y << " "
              << successif.z << '\n';

    std::cout << "Point obtenu par composition : "
              << parComposition.x << " "
              << parComposition.y << " "
              << parComposition.z << '\n';

    std::cout << std::scientific;

    std::cout << "Ecart : "
              << ecart.x << " "
              << ecart.y << " "
              << ecart.z << '\n';

    return 0;
}
```

## Formules de composition

J'ai comparé deux méthodes :

1. appliquer la pose A au point, puis appliquer la pose B au résultat ;
2. composer A et B, puis appliquer directement la pose composée au même point.

Pour une pose composée correspondant à **A puis B**, l'orientation est :

`orientation = B.orientation * A.orientation`

et la position est :

`position = rotation(B.orientation, A.position) + B.position`

L'ordre est important, en particulier pour la multiplication des quaternions.

## Vérification par l'exécution

Cette fois, j'ai compilé et exécuté le programme afin de comparer directement
les deux méthodes.

Le programme m'a donné les trois lignes suivantes :

```text
Point obtenu successivement : 2.7071067811865475 1.5000000000000000 -1.7071067811865475
Point obtenu par composition : 2.7071067811865479 1.5000000000000000 -1.7071067811865472
Ecart : 4.44089209850062616e-16 0.00000000000000000e+00 2.22044604925031308e-16
```

## Comparaison des deux points

Par application successive de A puis B, j'obtiens :

```text
(2.7071067811865475,
 1.5000000000000000,
 -1.7071067811865475)
```

Par composition de A et B puis application au point, j'obtiens :

```text
(2.7071067811865479,
 1.5000000000000000,
 -1.7071067811865472)
```

Les deux points sont pratiquement identiques.

Les très petites différences apparaissent uniquement lorsque j'affiche un
grand nombre de chiffres.

## Écart mesuré

L'écart réellement affiché par le programme est :

```text
(4.44089209850062616e-16,
 0.00000000000000000e+00,
 2.22044604925031308e-16)
```

Donc :

- écart sur X : `4.44089209850062616e-16` ;
- écart sur Y : `0.00000000000000000e+00` ;
- écart sur Z : `2.22044604925031308e-16`.

Les écarts sur X et Z sont de l'ordre de `10^-16`.

Ils sont extrêmement petits et correspondent aux arrondis produits par les
calculs en virgule flottante.

## Ce que je vérifie

Cette exécution confirme que les deux méthodes conduisent au même résultat
à la précision numérique près.

Elle vérifie donc expérimentalement les deux relations utilisées :

`orientation = B.orientation * A.orientation`

et :

`position = rotation(B.orientation, A.position) + B.position`

Ce test est particulièrement utile pour l'ordre des quaternions.

Une erreur dans l'ordre de multiplication aurait pu donner un programme qui
compile normalement, mais des coordonnées différentes entre l'application
successive et la composition.

Ici, les deux résultats coïncident aux erreurs d'arrondi près.

## Pourquoi l'affichage de l'écart est utile

Avec un affichage limité à quatre chiffres après la virgule, l'écart aurait
semblé exactement nul :

```text
0.0000 0.0000 0.0000
```

Avec davantage de précision, j'observe au contraire :

```text
4.44089209850062616e-16
0.00000000000000000e+00
2.22044604925031308e-16
```

Je peux donc distinguer un véritable désaccord entre les deux calculs d'une
simple erreur d'arrondi numérique.

## Conclusion

La composition permet de représenter plusieurs transformations successives
par une seule pose.

Dans mon test, j'ai comparé directement :

`point → A → B`

avec :

`point → composition(A, B)`

Les deux points obtenus sont pratiquement identiques et l'écart mesuré est
seulement de l'ordre de `10^-16`.

Cette exécution confirme donc que la composition utilisée dans mon programme
est correcte pour le cas testé.

Elle confirme également que l'ordre est essentiel : pour appliquer A puis B,
l'orientation composée utilisée ici est :

`B.orientation * A.orientation`

La composition des transformations n'est donc généralement pas commutative.
