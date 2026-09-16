# Exercice 3 — L'ordre inverse

Dans cet exercice, j'ai comparé deux façons d'appliquer une pose à un point.

La première applique la rotation puis la translation.

La seconde applique la translation puis la rotation.

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
    Vec3 u{q.x, q.y, q.z};

    double dotUV = u.x * v.x + u.y * v.y + u.z * v.z;
    double dotUU = u.x * u.x + u.y * u.y + u.z * u.z;

    Vec3 cross{
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };

    return {
        2.0 * dotUV * u.x + (q.w * q.w - dotUU) * v.x
            + 2.0 * q.w * cross.x,

        2.0 * dotUV * u.y + (q.w * q.w - dotUU) * v.y
            + 2.0 * q.w * cross.y,

        2.0 * dotUV * u.z + (q.w * q.w - dotUU) * v.z
            + 2.0 * q.w * cross.z
    };
}

// Rotation puis translation
Vec3 rotationPuisTranslation(const Pose& pose, const Vec3& point)
{
    Vec3 p = rotationQuaternion(pose.orientation, point);

    return {
        p.x + pose.position.x,
        p.y + pose.position.y,
        p.z + pose.position.z
    };
}

// Translation puis rotation
Vec3 translationPuisRotation(const Pose& pose, const Vec3& point)
{
    Vec3 p{
        point.x + pose.position.x,
        point.y + pose.position.y,
        point.z + pose.position.z
    };

    return rotationQuaternion(pose.orientation, p);
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

    // Point
    std::cin >> point.x >> point.y >> point.z;

    Vec3 resultat1 = rotationPuisTranslation(pose, point);
    Vec3 resultat2 = translationPuisRotation(pose, point);

    std::cout << std::fixed << std::setprecision(4);

    std::cout << resultat1.x << " "
              << resultat1.y << " "
              << resultat1.z << '\n';

    std::cout << resultat2.x << " "
              << resultat2.y << " "
              << resultat2.z << '\n';

    return 0;
}
```

## Comparaison

En général, les deux résultats ne sont pas identiques.

Avec la première méthode :

`R(point) + translation`

Avec la seconde :

`R(point + translation)`

Dans la seconde méthode, la translation est elle-même affectée par la rotation.
C'est pour cette raison que changer l'ordre des opérations change généralement
le résultat.

## Un cas où les deux résultats coïncident

On peut prendre par exemple :

- position de la pose : (0, 0, 0) ;
- quaternion identité : (1, 0, 0, 0) ;
- point : (1, 2, 3).

Dans ce cas, les deux méthodes donnent :

`(1, 2, 3)`

Les résultats coïncident parce que la translation est nulle. Il n'y a donc
aucun déplacement à faire tourner dans la deuxième méthode.

## Conclusion

La rotation et la translation ne peuvent pas être interverties librement.
L'ordre des transformations est important. Une pose normale applique la
rotation au point avant d'ajouter la translation.
