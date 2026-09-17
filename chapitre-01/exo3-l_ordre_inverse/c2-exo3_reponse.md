# Exercice 3 — L'ordre inverse

Dans cet exercice, j'ai comparé deux façons d'appliquer une pose à un même
point :

1. appliquer la rotation puis la translation ;
2. appliquer la translation puis la rotation.

L'objectif est de vérifier par l'exécution du programme que l'ordre des
transformations change généralement le résultat, puis de chercher un cas
non trivial dans lequel les deux résultats coïncident.

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

    double dotUV =
        u.x * v.x +
        u.y * v.y +
        u.z * v.z;

    double dotUU =
        u.x * u.x +
        u.y * u.y +
        u.z * u.z;

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

// Rotation puis translation : R(p) + t
Vec3 rotationPuisTranslation(
    const Pose& pose,
    const Vec3& point)
{
    Vec3 p =
        rotationQuaternion(
            pose.orientation,
            point
        );

    return {
        p.x + pose.position.x,
        p.y + pose.position.y,
        p.z + pose.position.z
    };
}

// Translation puis rotation : R(p + t)
Vec3 translationPuisRotation(
    const Pose& pose,
    const Vec3& point)
{
    Vec3 p{
        point.x + pose.position.x,
        point.y + pose.position.y,
        point.z + pose.position.z
    };

    return rotationQuaternion(
        pose.orientation,
        p
    );
}

void afficher(const Vec3& v)
{
    std::cout
        << "("
        << v.x << ", "
        << v.y << ", "
        << v.z
        << ")";
}

int main()
{
    std::cout
        << std::fixed
        << std::setprecision(4);

    const double angle =
        90.0 * 3.14159265358979323846 / 180.0;

    const double demiAngle = angle / 2.0;

    // Rotation de 90 degrés autour de Y
    Quaternion rotationY90{
        std::cos(demiAngle),
        0.0,
        std::sin(demiAngle),
        0.0
    };

    // Même point pour les deux expériences
    Vec3 point{
        1.0,
        2.0,
        3.0
    };

    // ================================================
    // TEST 1 : translation hors de l'axe Y
    // ================================================

    Pose pose1{
        {1.0, 0.0, 0.0},
        rotationY90
    };

    Vec3 resultat1A =
        rotationPuisTranslation(
            pose1,
            point
        );

    Vec3 resultat1B =
        translationPuisRotation(
            pose1,
            point
        );

    std::cout
        << "====================================\n";

    std::cout
        << "TEST 1 - Translation hors axe Y\n";

    std::cout
        << "Translation : (1, 0, 0)\n";

    std::cout
        << "Point : (1, 2, 3)\n";

    std::cout
        << "Rotation : 90 degres autour de Y\n\n";

    std::cout
        << "Rotation puis translation : ";

    afficher(resultat1A);

    std::cout << '\n';

    std::cout
        << "Translation puis rotation : ";

    afficher(resultat1B);

    std::cout << "\n\n";

    // ================================================
    // TEST 2 : translation sur l'axe Y
    // ================================================

    Pose pose2{
        {0.0, 2.0, 0.0},
        rotationY90
    };

    Vec3 resultat2A =
        rotationPuisTranslation(
            pose2,
            point
        );

    Vec3 resultat2B =
        translationPuisRotation(
            pose2,
            point
        );

    std::cout
        << "====================================\n";

    std::cout
        << "TEST 2 - Translation sur axe Y\n";

    std::cout
        << "Translation : (0, 2, 0)\n";

    std::cout
        << "Point : (1, 2, 3)\n";

    std::cout
        << "Rotation : 90 degres autour de Y\n\n";

    std::cout
        << "Rotation puis translation : ";

    afficher(resultat2A);

    std::cout << '\n';

    std::cout
        << "Translation puis rotation : ";

    afficher(resultat2B);

    std::cout << "\n\n";

    // Comparaison automatique du deuxième test

    const double epsilon = 0.000001;

    bool identiques =
        std::fabs(
            resultat2A.x - resultat2B.x
        ) < epsilon
        &&
        std::fabs(
            resultat2A.y - resultat2B.y
        ) < epsilon
        &&
        std::fabs(
            resultat2A.z - resultat2B.z
        ) < epsilon;

    std::cout
        << "====================================\n";

    std::cout
        << "COMPARAISON DU TEST 2\n";

    if (identiques)
    {
        std::cout
            << "Les deux resultats coincident.\n";
    }
    else
    {
        std::cout
            << "Les deux resultats sont differents.\n";
    }

    return 0;
}
```

## Premier test : les deux ordres donnent des résultats différents

Pour le premier test, j'ai utilisé :

- point : `(1, 2, 3)` ;
- translation : `(1, 0, 0)` ;
- rotation : `90°` autour de l'axe Y.

J'ai exécuté le programme et obtenu :

```text
TEST 1 - Translation hors axe Y
Translation : (1, 0, 0)
Point : (1, 2, 3)
Rotation : 90 degres autour de Y

Rotation puis translation : (4.0000, 2.0000, -1.0000)
Translation puis rotation : (3.0000, 2.0000, -2.0000)
```

Les deux positions finales sont donc :

```text
Rotation puis translation : (4.0000, 2.0000, -1.0000)
Translation puis rotation : (3.0000, 2.0000, -2.0000)
```

Elles sont clairement différentes.

## Pourquoi les deux résultats sont différents

Dans la première méthode, le programme calcule :

`R(point) + translation`

Dans la deuxième méthode, il calcule :

`R(point + translation)`

Or la rotation est linéaire, donc :

`R(point + translation) = R(point) + R(translation)`

La différence vient donc du fait que, dans la deuxième méthode, la
translation est elle-même affectée par la rotation.

Dans mon premier test, la translation est :

`(1, 0, 0)`

Elle n'est pas sur l'axe Y de la rotation.

La rotation de 90° autour de Y modifie donc sa direction.

C'est pourquoi j'obtiens deux résultats différents.

## Recherche d'un cas où les deux résultats coïncident

Dans ma première version, j'avais choisi le cas suivant :

- translation nulle ;
- quaternion identité.

Les deux résultats coïncidaient bien, mais ce cas était trop trivial.

J'ai donc cherché un cas dans lequel :

- la translation n'est pas nulle ;
- la rotation n'est pas l'identité ;
- les deux résultats coïncident malgré tout.

Pour que les deux expressions soient égales, il faut avoir :

`R(point) + translation = R(point) + R(translation)`

Il faut donc que :

`translation = R(translation)`

Autrement dit, il faut trouver une translation qui reste inchangée lorsqu'on
lui applique la rotation.

## Deuxième test : translation sur l'axe de rotation

J'ai gardé la rotation de **90° autour de l'axe Y**.

Cette fois, j'ai choisi une translation :

`(0, 2, 0)`

Cette translation est parallèle à l'axe Y.

J'ai conservé le même point :

`(1, 2, 3)`

J'ai ensuite exécuté le programme.

La sortie obtenue est :

```text
TEST 2 - Translation sur axe Y
Translation : (0, 2, 0)
Point : (1, 2, 3)
Rotation : 90 degres autour de Y

Rotation puis translation : (3.0000, 4.0000, -1.0000)
Translation puis rotation : (3.0000, 4.0000, -1.0000)

====================================
COMPARAISON DU TEST 2
Les deux resultats coincident.
```

Cette fois, les deux méthodes donnent exactement :

`(3.0000, 4.0000, -1.0000)`

La translation est pourtant non nulle et la rotation est bien de 90°.

## Pourquoi les résultats coïncident

La rotation utilisée se fait autour de l'axe Y.

La translation :

`(0, 2, 0)`

est elle-même dirigée selon Y.

Une rotation autour de Y ne modifie pas un vecteur situé sur cet axe.

On a donc :

`R(0, 2, 0) = (0, 2, 0)`

Ainsi :

`R(point) + translation`

et :

`R(point + translation)`

donnent le même résultat.

Dans mon exécution :

```text
R puis T : (3.0000, 4.0000, -1.0000)
T puis R : (3.0000, 4.0000, -1.0000)
```

## La famille de cas

Ce deuxième test permet de trouver une famille entière de cas et pas
seulement un exemple particulier.

Pour une rotation autour de l'axe Y, toute translation parallèle à cet axe
reste inchangée par la rotation.

On peut donc prendre par exemple :

```text
(0, 1, 0)
(0, 2, 0)
(0, 5, 0)
(0, -3, 0)
```

Plus généralement :

`(0, a, 0)`

où `a` peut prendre différentes valeurs.

Pour toutes ces translations, une rotation autour de Y laisse le vecteur de
translation inchangé.

La condition générale est donc :

`R(t) = t`

où `t` représente la translation.

Lorsque cette condition est respectée :

`R(p) + t = R(p + t)`

et les deux ordres coïncident.

## Comparaison des deux expériences

| Test | Translation | Résultat R puis T | Résultat T puis R | Conclusion |
|---|---|---|---|---|
| 1 | `(1,0,0)` | `(4,2,-1)` | `(3,2,-2)` | différents |
| 2 | `(0,2,0)` | `(3,4,-1)` | `(3,4,-1)` | identiques |

Le premier test montre le comportement général : changer l'ordre change le
résultat.

Le deuxième montre que ce n'est pas toujours le cas.

Les deux opérations peuvent donner le même résultat lorsque la rotation ne
modifie pas le vecteur de translation.

## Ce que j'ai découvert en exécutant le programme

Ma première réponse utilisait seulement une translation nulle.

Après avoir exécuté plusieurs cas, j'ai constaté qu'il existe une situation
plus intéressante : la translation peut être non nulle et les deux résultats
peuvent quand même être identiques.

Ce qui compte réellement n'est donc pas que la translation soit nulle.

La condition importante est que **la rotation laisse la translation
inchangée**.

Dans mon deuxième test, c'est exactement ce qui se produit parce que la
translation est portée par l'axe Y autour duquel j'effectue la rotation.

## Conclusion

L'exécution du programme confirme que l'ordre entre rotation et translation
est généralement important.

Avec une translation `(1, 0, 0)` et une rotation de 90° autour de Y, j'ai
obtenu :

```text
Rotation puis translation : (4.0000, 2.0000, -1.0000)
Translation puis rotation : (3.0000, 2.0000, -2.0000)
```

Les deux résultats sont différents.

J'ai ensuite trouvé un cas non trivial avec une translation `(0, 2, 0)` sur
l'axe Y. Le programme donne alors :

```text
Rotation puis translation : (3.0000, 4.0000, -1.0000)
Translation puis rotation : (3.0000, 4.0000, -1.0000)
```

Les deux résultats coïncident.

J'en conclus que la translation n'a pas besoin d'être nulle pour que les deux
ordres donnent le même résultat. Il suffit que la rotation laisse le vecteur
de translation inchangé, c'est-à-dire :

`R(t) = t`

Pour une rotation autour de Y, toutes les translations parallèles à l'axe Y
forment donc une famille de cas où les deux ordres coïncident.
