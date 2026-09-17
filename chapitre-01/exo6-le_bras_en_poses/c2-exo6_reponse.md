# Exercice 6 — Le bras en poses

Pour cet exercice, j'ai représenté un bras articulé par trois poses :
l'épaule, le coude et la main.

Chaque articulation est exprimée dans le repère de son parent :

- l'épaule est placée à l'origine ;
- le coude est à 0,40 m de l'épaule ;
- la main est à 0,35 m du coude.

La pose dans le monde est obtenue en composant successivement les poses
de l'épaule, du coude et de la main.

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

    double dotUV = u.x*v.x + u.y*v.y + u.z*v.z;
    double dotUU = u.x*u.x + u.y*u.y + u.z*u.z;

    Vec3 cross{
        u.y*v.z - u.z*v.y,
        u.z*v.x - u.x*v.z,
        u.x*v.y - u.y*v.x
    };

    return {
        2.0*dotUV*u.x + (q.w*q.w-dotUU)*v.x
            + 2.0*q.w*cross.x,

        2.0*dotUV*u.y + (q.w*q.w-dotUU)*v.y
            + 2.0*q.w*cross.y,

        2.0*dotUV*u.z + (q.w*q.w-dotUU)*v.z
            + 2.0*q.w*cross.z
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

// Composition : parent * locale
Pose composer(const Pose& parent, const Pose& locale)
{
    Vec3 p = rotationQuaternion(
        parent.orientation,
        locale.position
    );

    Pose resultat;

    resultat.position = {
        parent.position.x + p.x,
        parent.position.y + p.y,
        parent.position.z + p.z
    };

    resultat.orientation = multiplierQuaternion(
        parent.orientation,
        locale.orientation
    );

    return resultat;
}

Quaternion rotationY(double angle)
{
    double demi = angle / 2.0;

    return {
        std::cos(demi),
        0.0,
        std::sin(demi),
        0.0
    };
}

int main()
{
    const double PI = 3.14159265358979323846;

    // Épaule à l'origine
    Pose epaule{
        {0.0, 0.0, 0.0},
        {1.0, 0.0, 0.0, 0.0}
    };

    // Coude à 0,40 m de l'épaule
    Pose coudeLocal{
        {0.40, 0.0, 0.0},
        {1.0, 0.0, 0.0, 0.0}
    };

    // Main à 0,35 m du coude
    Pose mainLocale{
        {0.35, 0.0, 0.0},
        {1.0, 0.0, 0.0, 0.0}
    };

    Pose coudeMonde = composer(epaule, coudeLocal);
    Pose mainMonde = composer(coudeMonde, mainLocale);

    std::cout << std::fixed << std::setprecision(4);

    std::cout << "Avant rotation\n";

    std::cout << "Coude : "
              << coudeMonde.position.x << " "
              << coudeMonde.position.y << " "
              << coudeMonde.position.z << '\n';

    std::cout << "Main : "
              << mainMonde.position.x << " "
              << mainMonde.position.y << " "
              << mainMonde.position.z << '\n';

    // Rotation de l'épaule de 90 degrés autour de Y
    epaule.orientation = rotationY(PI / 2.0);

    coudeMonde = composer(epaule, coudeLocal);
    mainMonde = composer(coudeMonde, mainLocale);

    std::cout << "\nApres rotation de l'epaule\n";

    std::cout << "Coude : "
              << coudeMonde.position.x << " "
              << coudeMonde.position.y << " "
              << coudeMonde.position.z << '\n';

    std::cout << "Main : "
              << mainMonde.position.x << " "
              << mainMonde.position.y << " "
              << mainMonde.position.z << '\n';

    return 0;
}
```

## Prédiction avant l'exécution

Avant de lancer le programme, j'ai essayé de prévoir les positions.

Au départ, le bras est entièrement aligné sur l'axe X positif.

Le coude se trouve à 0,40 m de l'épaule. La main est encore à 0,35 m du
coude, donc elle se trouve à :

`0,40 + 0,35 = 0,75 m`

de l'épaule.

Je prévois donc avant rotation :

- coude : `(0.4000, 0.0000, 0.0000)`
- main : `(0.7500, 0.0000, 0.0000)`

Je fais ensuite tourner l'épaule de +90 degrés autour de Y.

Avec la convention utilisée par mon quaternion et ma fonction de rotation,
un vecteur dirigé vers +X tourne alors vers -Z.

Je prévois donc après rotation :

- coude : `(0.0000, 0.0000, -0.4000)`
- main : `(0.0000, 0.0000, -0.7500)`

## Exécution du programme

J'ai ensuite compilé et lancé le programme.

### Avant rotation

Le programme affiche :

```text
Avant rotation
Coude : 0.4000 0.0000 0.0000
Main : 0.7500 0.0000 0.0000
```

Les positions mesurées sont donc :

- coude : `(0.4000, 0.0000, 0.0000)`
- main : `(0.7500, 0.0000, 0.0000)`

### Après rotation de l'épaule

Après la rotation de 90 degrés autour de Y, le programme affiche :

```text
Apres rotation de l'epaule
Coude : 0.0000 0.0000 -0.4000
Main : 0.0000 0.0000 -0.7500
```

Les nouvelles positions sont donc :

- coude : `(0.0000, 0.0000, -0.4000)`
- main : `(0.0000, 0.0000, -0.7500)`

## Comparaison entre ma prédiction et l'exécution

| Articulation | Prédiction | Résultat affiché |
|---|---|---|
| Coude avant rotation | `(0.4000, 0.0000, 0.0000)` | `(0.4000, 0.0000, 0.0000)` |
| Main avant rotation | `(0.7500, 0.0000, 0.0000)` | `(0.7500, 0.0000, 0.0000)` |
| Coude après rotation | `(0.0000, 0.0000, -0.4000)` | `(0.0000, 0.0000, -0.4000)` |
| Main après rotation | `(0.0000, 0.0000, -0.7500)` | `(0.0000, 0.0000, -0.7500)` |

Ma prédiction et les valeurs affichées par le programme coïncident.

Ce résultat me permet surtout de vérifier que la transformation de l'épaule
se propage correctement au reste de la chaîne.

La position locale du coude reste `(0.40, 0, 0)` et celle de la main reste
`(0.35, 0, 0)`, mais leurs positions dans le monde changent parce qu'elles
héritent de l'orientation de leur parent.

## Ce que j'observe

Avant la rotation, les trois articulations sont alignées sur X :

```text
épaule ---- 0,40 m ---- coude ---- 0,35 m ---- main
   0                         0,40                 0,75
```

Après la rotation de l'épaule, le bras entier est orienté vers -Z.

Le coude se retrouve à 0,40 m de l'épaule dans cette direction et la main
à 0,75 m.

La main suit donc le coude, et le coude suit l'épaule, sans que j'aie besoin
de modifier leurs positions locales.

## Conclusion

L'expérience confirme qu'une articulation enfant hérite des transformations
de son parent.

Lorsque je tourne uniquement l'épaule de 90 degrés autour de Y, le coude
passe de `(0.4000, 0.0000, 0.0000)` à
`(0.0000, 0.0000, -0.4000)`.

La main passe en même temps de `(0.7500, 0.0000, 0.0000)` à
`(0.0000, 0.0000, -0.7500)`.

Les positions locales du coude et de la main n'ont pourtant pas changé.

C'est la composition successive des poses
**épaule → coude → main** qui propage la transformation dans toute la
chaîne articulée.
