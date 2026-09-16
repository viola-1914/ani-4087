# Exercice 5 — La composition

La composition de deux poses permet de remplacer deux transformations
successives par une seule transformation équivalente.

Dans cet exercice, j'applique d'abord la pose A, puis la pose B.

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

    // Pose A : position puis quaternion (w x y z)
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

    std::cout << std::fixed << std::setprecision(4);

    // Premier point
    std::cout << successif.x << " "
              << successif.y << " "
              << successif.z << '\n';

    // Deuxième point
    std::cout << parComposition.x << " "
              << parComposition.y << " "
              << parComposition.z << '\n';

    // Écart
    std::cout << ecart.x << " "
              << ecart.y << " "
              << ecart.z << '\n';

    return 0;
}
```

## Vérification

J'ai comparé deux méthodes :

1. appliquer la pose A au point, puis appliquer la pose B au résultat ;
2. composer A et B, puis appliquer directement la pose composée au point.

Pour une pose composée correspondant à A puis B, l'orientation est :

`orientation = B.orientation * A.orientation`

et la position est :

`position = rotation(B.orientation, A.position) + B.position`

Les deux points obtenus doivent être identiques aux erreurs d'arrondi près.

L'écart attendu est donc de la forme :

`0.0000 0.0000 0.0000`

## Conclusion

La composition permet de représenter plusieurs transformations successives
par une seule pose. L'ordre reste important : composer A puis B n'est
généralement pas équivalent à composer B puis A.
