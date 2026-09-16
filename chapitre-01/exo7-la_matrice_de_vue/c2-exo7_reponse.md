# Exercice 7 — La matrice de vue

La matrice de vue correspond à l'inverse de la transformation qui place
l'observateur dans le monde.

J'ai comparé deux méthodes :

1. construire la matrice 4 × 4 de la pose et utiliser une inversion générale ;
2. construire directement l'inverse de la pose avec le conjugué du quaternion
   et la translation opposée tournée.

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

struct Mat4
{
    double m[4][4];
};

Quaternion conjugue(const Quaternion& q)
{
    return {q.w, -q.x, -q.y, -q.z};
}

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

Mat4 matricePose(const Pose& pose)
{
    Quaternion q = pose.orientation;

    double w = q.w;
    double x = q.x;
    double y = q.y;
    double z = q.z;

    Mat4 M{};

    M.m[0][0] = 1.0 - 2.0*(y*y + z*z);
    M.m[0][1] = 2.0*(x*y - z*w);
    M.m[0][2] = 2.0*(x*z + y*w);
    M.m[0][3] = pose.position.x;

    M.m[1][0] = 2.0*(x*y + z*w);
    M.m[1][1] = 1.0 - 2.0*(x*x + z*z);
    M.m[1][2] = 2.0*(y*z - x*w);
    M.m[1][3] = pose.position.y;

    M.m[2][0] = 2.0*(x*z - y*w);
    M.m[2][1] = 2.0*(y*z + x*w);
    M.m[2][2] = 1.0 - 2.0*(x*x + y*y);
    M.m[2][3] = pose.position.z;

    M.m[3][0] = 0.0;
    M.m[3][1] = 0.0;
    M.m[3][2] = 0.0;
    M.m[3][3] = 1.0;

    return M;
}

/*
   Inversion générale par Gauss-Jordan.

   Pour reproduire le problème étudié dans l'exercice,
   cette version renvoie l'identité si la matrice
   est considérée comme non inversible.
*/
Mat4 inverseGenerale(Mat4 A)
{
    double aug[4][8]{};

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
            aug[i][j] = A.m[i][j];

        aug[i][i + 4] = 1.0;
    }

    for (int col = 0; col < 4; ++col)
    {
        int pivot = col;

        for (int ligne = col + 1; ligne < 4; ++ligne)
        {
            if (std::fabs(aug[ligne][col]) >
                std::fabs(aug[pivot][col]))
            {
                pivot = ligne;
            }
        }

        if (std::fabs(aug[pivot][col]) < 1e-12)
        {
            Mat4 identite{};

            for (int i = 0; i < 4; ++i)
                identite.m[i][i] = 1.0;

            return identite;
        }

        if (pivot != col)
        {
            for (int j = 0; j < 8; ++j)
            {
                double temp = aug[col][j];
                aug[col][j] = aug[pivot][j];
                aug[pivot][j] = temp;
            }
        }

        double p = aug[col][col];

        for (int j = 0; j < 8; ++j)
            aug[col][j] /= p;

        for (int ligne = 0; ligne < 4; ++ligne)
        {
            if (ligne == col)
                continue;

            double facteur = aug[ligne][col];

            for (int j = 0; j < 8; ++j)
                aug[ligne][j] -= facteur * aug[col][j];
        }
    }

    Mat4 inverse{};

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            inverse.m[i][j] = aug[i][j + 4];

    return inverse;
}

Mat4 vueDirecte(const Pose& pose)
{
    Quaternion qc = conjugue(pose.orientation);

    Vec3 opposee{
        -pose.position.x,
        -pose.position.y,
        -pose.position.z
    };

    Vec3 translationInverse =
        rotationQuaternion(qc, opposee);

    Pose inverse{
        translationInverse,
        qc
    };

    return matricePose(inverse);
}

void afficher(const Mat4& M)
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
            std::cout << M.m[i][j] << " ";

        std::cout << '\n';
    }
}

int main()
{
    Pose pose;

    // Position
    std::cin >> pose.position.x
             >> pose.position.y
             >> pose.position.z;

    // Quaternion normalisé : w x y z
    std::cin >> pose.orientation.w
             >> pose.orientation.x
             >> pose.orientation.y
             >> pose.orientation.z;

    Mat4 M = matricePose(pose);

    Mat4 vue1 = inverseGenerale(M);
    Mat4 vue2 = vueDirecte(pose);

    std::cout << std::fixed << std::setprecision(6);

    std::cout << "Matrice par inversion generale :\n";
    afficher(vue1);

    std::cout << "\nMatrice construite directement :\n";
    afficher(vue2);

    std::cout << "\nEcarts sur les 16 coefficients :\n";

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            std::cout
                << std::fabs(vue1.m[i][j] - vue2.m[i][j])
                << " ";
        }

        std::cout << '\n';
    }

    // Pose dégénérée : quaternion nul
    Pose degeneree{
        {1.0, 2.0, 3.0},
        {0.0, 0.0, 0.0, 0.0}
    };

    Mat4 Md = matricePose(degeneree);
    Mat4 resultatDeg = inverseGenerale(Md);

    std::cout << "\nResultat pour la pose degeneree :\n";
    afficher(resultatDeg);

    return 0;
}
```

## Comparaison

Pour une pose valide, les deux méthodes doivent produire les mêmes seize
coefficients, aux erreurs d'arrondi près.

Les écarts doivent donc être nuls ou très proches de zéro.

La première méthode construit une matrice de pose complète puis effectue une
inversion générale 4 × 4.

La seconde utilise directement les propriétés de la pose. Pour un quaternion
normalisé, son inverse est son conjugué. La translation inverse est obtenue
en faisant tourner l'opposé de la position par ce conjugué.

## Pose dégénérée

J'ai également testé une pose dont le quaternion est nul :

`(0, 0, 0, 0)`

Cette valeur ne représente pas une orientation valide.

Ce test montre le danger d'une fonction d'inversion générale qui peut masquer
un problème en renvoyant une matrice identité lorsqu'elle considère la matrice
comme non inversible.

Le programme peut alors continuer avec une valeur qui semble valide alors que
la pose d'origine était incorrecte.

## Conclusion

Pour une pose XR valide, la construction directe de la matrice de vue est plus
adaptée : elle exploite directement la position et le quaternion.

Elle rend également plus explicites les hypothèses faites sur la pose, au lieu
de dépendre du comportement d'une routine générale d'inversion de matrice.
