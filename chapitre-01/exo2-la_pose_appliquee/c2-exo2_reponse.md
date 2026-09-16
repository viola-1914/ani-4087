# Exercice 2 — La pose appliquée

Une pose contient une position et une orientation représentée par un quaternion.

Pour appliquer une pose à un point, il faut d'abord effectuer la rotation du
point par le quaternion, puis ajouter la translation.

## Code C++

```cpp
#include <iostream>
#include <iomanip>

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
    // Rotation d'un vecteur par un quaternion normalisé.
    Vec3 u{q.x, q.y, q.z};

    double dotUV = u.x * v.x + u.y * v.y + u.z * v.z;
    double dotUU = u.x * u.x + u.y * u.y + u.z * u.z;

    Vec3 cross{
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };

    return {
        2.0 * dotUV * u.x + (q.w * q.w - dotUU) * v.x + 2.0 * q.w * cross.x,
        2.0 * dotUV * u.y + (q.w * q.w - dotUU) * v.y + 2.0 * q.w * cross.y,
        2.0 * dotUV * u.z + (q.w * q.w - dotUU) * v.z + 2.0 * q.w * cross.z
    };
}

Vec3 appliquerPose(const Pose& pose, const Vec3& point)
{
    // 1. Rotation
    Vec3 p = rotationQuaternion(pose.orientation, point);

    // 2. Translation
    return {
        p.x + pose.position.x,
        p.y + pose.position.y,
        p.z + pose.position.z
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

    // Point à transformer
    std::cin >> point.x >> point.y >> point.z;

    Vec3 resultat = appliquerPose(pose, point);

    std::cout << std::fixed << std::setprecision(4);
    std::cout << resultat.x << " "
              << resultat.y << " "
              << resultat.z << '\n';

    return 0;
}
```

## Principe

Si la pose possède une position `t` et une orientation `q`, le point transformé
est obtenu par :

`point_transforme = rotation(q, point) + t`

L'ordre est important : la rotation est appliquée en premier, puis la
translation.
