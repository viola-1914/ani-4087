# Exercice 4 — L'inverse d'une pose

Une pose contient une position et une orientation représentée par un quaternion.

Pour inverser une pose, je prends le conjugué du quaternion, puis j'applique
ce quaternion à l'opposé de la position.

L'objectif est ensuite de vérifier que l'application d'une pose suivie de son
inverse permet bien de retrouver le point de départ, aux erreurs numériques près.

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

Vec3 appliquerPose(const Pose& pose, const Vec3& point)
{
    Vec3 tourne = rotationQuaternion(pose.orientation, point);

    return {
        tourne.x + pose.position.x,
        tourne.y + pose.position.y,
        tourne.z + pose.position.z
    };
}

Pose Inverser(const Pose& pose)
{
    // Conjugué du quaternion normalisé
    Quaternion conjugue{
        pose.orientation.w,
        -pose.orientation.x,
        -pose.orientation.y,
        -pose.orientation.z
    };

    // Opposé de la position
    Vec3 opposee{
        -pose.position.x,
        -pose.position.y,
        -pose.position.z
    };

    // Rotation de l'opposé de la position par le quaternion inverse
    Vec3 positionInverse =
        rotationQuaternion(conjugue, opposee);

    return {
        positionInverse,
        conjugue
    };
}

int main()
{
    Pose pose;
    Vec3 point;

    // Position de la pose
    std::cin >> pose.position.x
             >> pose.position.y
             >> pose.position.z;

    // Quaternion normalisé : w x y z
    std::cin >> pose.orientation.w
             >> pose.orientation.x
             >> pose.orientation.y
             >> pose.orientation.z;

    // Point de départ
    std::cin >> point.x
             >> point.y
             >> point.z;

    // Application de la pose
    Vec3 transforme = appliquerPose(pose, point);

    // Calcul de la pose inverse
    Pose inverse = Inverser(pose);

    // Application de l'inverse au point transformé
    Vec3 retrouve = appliquerPose(inverse, transforme);

    // Calcul de l'écart avec le point initial
    Vec3 ecart{
        retrouve.x - point.x,
        retrouve.y - point.y,
        retrouve.z - point.z
    };

    std::cout << std::setprecision(17);

    std::cout << "Point de depart : "
              << point.x << " "
              << point.y << " "
              << point.z << '\n';

    std::cout << "Apres la pose : "
              << transforme.x << " "
              << transforme.y << " "
              << transforme.z << '\n';

    std::cout << "Apres la pose inverse : "
              << retrouve.x << " "
              << retrouve.y << " "
              << retrouve.z << '\n';

    std::cout << std::scientific;

    std::cout << "Ecart : "
              << ecart.x << " "
              << ecart.y << " "
              << ecart.z << '\n';

    return 0;
}
```

## Vérification

Si une pose transforme un point `P` en `P'`, alors l'application de la pose
inverse à `P'` doit permettre de retrouver `P`.

Pour une pose constituée d'une rotation `R` et d'une translation `t`,
la position de la pose inverse est :

`R^-1(-t)`

Son orientation est donnée par le conjugué du quaternion original lorsque
celui-ci est normalisé.

Je ne me suis donc pas limité au résultat théorique : j'ai compilé et exécuté
le programme afin de mesurer l'écart obtenu après l'application successive de
la pose et de son inverse.

## Résultat de l'exécution

Pour le test effectué, mon point de départ était :

```text
Point de depart :
1.3 -0.7 2.1
```

Après application de la pose, le programme a affiché :

```text
Apres la pose :
3.4041630560342617 1.3 1.5656854249492382
```

J'ai ensuite appliqué la pose inverse à ce résultat.

Le point retrouvé était :

```text
Apres la pose inverse :
1.3000000000000003 -0.69999999999999996 2.0999999999999996
```

Il est donc pratiquement identique au point de départ, mais pas exactement
identique au niveau de sa représentation numérique.

## Écart mesuré

J'ai calculé la différence entre le point retrouvé et le point initial.

Le programme a affiché :

```text
Ecart :
2.22044604925031308e-16 0.00000000000000000e+00 -4.44089209850062616e-16
```

Les trois écarts mesurés sont donc :

- sur X : `2.22044604925031308e-16`
- sur Y : `0.00000000000000000e+00`
- sur Z : `-4.44089209850062616e-16`

## Ce que j'observe

Les écarts sur X et Z ne sont pas exactement nuls.

Ils sont cependant extrêmement petits, de l'ordre de `10^-16`.

Sur Y, l'écart obtenu lors de ce test est exactement nul dans la
représentation affichée.

Si j'avais conservé un affichage limité à quatre chiffres après la virgule,
j'aurais simplement obtenu :

```text
0.0000 0.0000 -0.0000
```

et je n'aurais pas vu les petites différences produites par les calculs.

L'affichage avec davantage de précision permet donc de voir concrètement ce
que signifie l'expression « nul aux arrondis près ».

Les nombres obtenus ne montrent pas un échec de l'inversion. Ils correspondent
aux petites erreurs numériques produites par les opérations successives sur
des nombres en virgule flottante.

## Comparaison

| Coordonnée | Point initial | Point retrouvé | Écart |
|---|---:|---:|---:|
| X | 1.3 | 1.3000000000000003 | `2.22044604925031308e-16` |
| Y | -0.7 | -0.69999999999999996 | `0.00000000000000000e+00` |
| Z | 2.1 | 2.0999999999999996 | `-4.44089209850062616e-16` |

La comparaison montre donc que l'application de la pose inverse ramène bien
le point à sa position de départ, à la précision numérique près.

## Conclusion

Cette fois, la fonction `Inverser(pose)` n'est pas seulement vérifiée de
manière théorique.

J'ai appliqué une pose à un point, puis appliqué la pose inverse au résultat
et mesuré l'écart avec le point initial.

J'obtiens :

```text
(2.22044604925031308e-16,
 0.00000000000000000e+00,
 -4.44089209850062616e-16)
```

Ces valeurs sont nulles à la précision des calculs près.

Cette expérience confirme donc que ma fonction `Inverser(pose)` annule
correctement la rotation et la translation de la pose initiale. Elle m'a
également permis d'observer directement les petites erreurs d'arrondi liées
aux calculs en virgule flottante.
