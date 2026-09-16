# Exercice 4 — L'inverse d'une pose

Une pose contient une position et une orientation représentée par un quaternion.

Pour inverser une pose, je prends le conjugué du quaternion, puis j'applique
ce quaternion à l'opposé de la position.

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

    // Position opposée
    Vec3 opposee{
        -pose.position.x,
        -pose.position.y,
        -pose.position.z
    };

    // L'opposé de la position est tourné par le conjugué
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

    // Quaternion : w x y z
    std::cin >> pose.orientation.w
             >> pose.orientation.x
             >> pose.orientation.y
             >> pose.orientation.z;

    // Point de départ
    std::cin >> point.x >> point.y >> point.z;

    // Application de la pose
    Vec3 transforme = appliquerPose(pose, point);

    // Calcul de la pose inverse
    Pose inverse = Inverser(pose);

    // Application de l'inverse au résultat
    Vec3 retrouve = appliquerPose(inverse, transforme);

    // Écart avec le point initial
    Vec3 ecart{
        retrouve.x - point.x,
        retrouve.y - point.y,
        retrouve.z - point.z
    };

    std::cout << std::fixed << std::setprecision(4);

    std::cout << ecart.x << " "
              << ecart.y << " "
              << ecart.z << '\n';

    return 0;
}
```

## Vérification

Si une pose transforme un point `P` en `P'`, alors l'application de la pose
inverse à `P'` doit redonner `P`.

La position de la pose inverse est :

`R^-1(-t)`

et son orientation est le conjugué du quaternion original.

Pour un quaternion normalisé, le conjugué représente la rotation inverse.

Après avoir appliqué la pose puis son inverse, l'écart avec le point de départ
doit donc être nul aux erreurs d'arrondi près.

Par exemple, l'affichage attendu peut être :

`0.0000 0.0000 0.0000`

## Conclusion

La pose inverse annule à la fois la translation et la rotation de la pose
initiale. La vérification permet de confirmer que l'on retrouve bien le point
de départ, aux erreurs numériques près.
