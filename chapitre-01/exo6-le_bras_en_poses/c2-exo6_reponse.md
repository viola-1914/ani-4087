# Exercice 6 — Le bras en poses

Pour cet exercice, j'ai représenté un bras articulé par trois poses :
l'épaule, le coude et la main.

Chaque articulation est exprimée dans le repère de son parent :

- l'épaule est placée à l'origine ;
- le coude est à 0,40 m de l'épaule ;
- la main est à 0,35 m du coude.

La pose dans le monde est obtenue en composant les poses successives.

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

// parent * locale
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

## Résultat attendu

Avant la rotation de l'épaule, le coude et la main sont alignés :

- coude : environ `(0.4000, 0.0000, 0.0000)`
- main : environ `(0.7500, 0.0000, 0.0000)`

Lorsque je fais tourner l'épaule de 90 degrés autour de l'axe Y, la position
du coude change et la main suit également le mouvement.

Cela montre que la pose de la main dépend de celle du coude, qui dépend
elle-même de celle de l'épaule.

## Conclusion

Une articulation enfant hérite des transformations de son parent.

Ainsi, lorsque l'épaule tourne, le coude est déplacé dans le monde et la main
suit automatiquement, même si les poses locales du coude et de la main
n'ont pas changé.

La composition des poses permet donc de construire naturellement une chaîne
articulée.
