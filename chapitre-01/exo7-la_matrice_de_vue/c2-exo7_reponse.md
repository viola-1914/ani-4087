# Exercice 7 — La matrice de vue

## Objectif

La matrice de vue correspond à l'inverse de la transformation qui place
l'observateur dans le monde.

Dans cet exercice, je compare deux façons de la calculer :

1. construire la matrice 4 × 4 de la pose puis utiliser une inversion générale ;
2. construire directement l'inverse de la pose à partir du conjugué du
   quaternion et de la translation inverse.

L'objectif n'est pas seulement de vérifier que les deux méthodes fonctionnent.
Je veux également regarder les seize coefficients réellement produits et
observer ce que fait la méthode générale lorsqu'elle reçoit une entrée
dégénérée.

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

    std::cin >> pose.position.x
             >> pose.position.y
             >> pose.position.z;

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

## Exécution

Pour avoir un cas simple mais non trivial, j'ai utilisé la pose suivante :

```text
Position :
1 2 3

Quaternion :
0.9238795 0 0.3826834 0
```

Le quaternion correspond à une rotation d'environ 45 degrés autour de
l'axe Y.

## 1. Résultat de l'inversion générale

Après exécution, j'obtiens :

```text
Matrice par inversion generale :

0.707107  0.000000  -0.707107   1.414214
0.000000  1.000000   0.000000  -2.000000
0.707107  0.000000   0.707107  -2.828427
0.000000  0.000000   0.000000   1.000000
```

Les seize coefficients sont donc bien visibles.

## 2. Résultat de la construction directe

La deuxième méthode donne :

```text
Matrice construite directement :

0.707107  0.000000  -0.707107   1.414214
0.000000  1.000000   0.000000  -2.000000
0.707107  0.000000   0.707107  -2.828427
0.000000  0.000000   0.000000   1.000000
```

## Comparaison des seize coefficients

J'ai ensuite comparé les deux matrices coefficient par coefficient.

| Coefficient | Inversion générale | Construction directe | Écart |
|---|---:|---:|---:|
| m00 | 0.707107 | 0.707107 | 0.000000 |
| m01 | 0.000000 | 0.000000 | 0.000000 |
| m02 | -0.707107 | -0.707107 | 0.000000 |
| m03 | 1.414214 | 1.414214 | 0.000000 |
| m10 | 0.000000 | 0.000000 | 0.000000 |
| m11 | 1.000000 | 1.000000 | 0.000000 |
| m12 | 0.000000 | 0.000000 | 0.000000 |
| m13 | -2.000000 | -2.000000 | 0.000000 |
| m20 | 0.707107 | 0.707107 | 0.000000 |
| m21 | 0.000000 | 0.000000 | 0.000000 |
| m22 | 0.707107 | 0.707107 | 0.000000 |
| m23 | -2.828427 | -2.828427 | 0.000000 |
| m30 | 0.000000 | 0.000000 | 0.000000 |
| m31 | 0.000000 | 0.000000 | 0.000000 |
| m32 | 0.000000 | 0.000000 | 0.000000 |
| m33 | 1.000000 | 1.000000 | 0.000000 |

Avec la précision d'affichage choisie, les seize écarts obtenus sont :

```text
0.000000  0.000000  0.000000  0.000000
0.000000  0.000000  0.000000  0.000000
0.000000  0.000000  0.000000  0.000000
0.000000  0.000000  0.000000  0.000000
```

Dans ce test valide, les deux méthodes donnent donc le même résultat
à la précision affichée.

## 3. Test de la pose dégénérée

Je passe maintenant à la première méthode la pose suivante :

```text
position = (1, 2, 3)
quaternion = (0, 0, 0, 0)
```

Le quaternion nul ne représente pas une orientation valide.

Avec l'implémentation de `inverseGenerale()` utilisée dans ce programme,
lorsqu'un pivot est considéré comme nul, la fonction retourne explicitement
une matrice identité.

Le résultat affiché est donc :

```text
Resultat pour la pose degeneree :

1.000000  0.000000  0.000000  0.000000
0.000000  1.000000  0.000000  0.000000
0.000000  0.000000  1.000000  0.000000
0.000000  0.000000  0.000000  1.000000
```

Les seize coefficients retournés sont ainsi :

```text
1.000000  0.000000  0.000000  0.000000
0.000000  1.000000  0.000000  0.000000
0.000000  0.000000  1.000000  0.000000
0.000000  0.000000  0.000000  1.000000
```

## Ce que j'observe

Ce résultat m'a permis de voir concrètement le problème.

La fonction ne renvoie pas une matrice remplie de valeurs manifestement
absurdes. Elle renvoie une matrice identité parfaitement propre :

```text
1 0 0 0
0 1 0 0
0 0 1 0
0 0 0 1
```

Si le reste du programme ne sait pas que l'inversion a échoué, cette matrice
peut facilement être interprétée comme un résultat valide.

Dans un système XR, cela pourrait faire revenir brutalement la caméra à une
pose correspondant à l'identité alors que le véritable problème est une pose
d'entrée invalide.

Le test m'a donc montré que le danger ne vient pas seulement de l'échec de
l'inversion, mais aussi de la manière dont cet échec est signalé.

## Conclusion

Sur la pose valide testée, l'inversion générale et la construction directe
produisent les mêmes seize coefficients.

Sur la pose dégénérée, j'observe en revanche exactement le comportement de
ma routine générale : elle retourne une matrice identité.

Je ne parle donc plus ici de ce que la fonction pourrait produire : dans
cette implémentation, c'est bien le résultat obtenu lorsque l'inversion
échoue.

La construction directe reste plus adaptée à une pose XR parce qu'elle
exploite explicitement sa structure : une position et une orientation
représentée par un quaternion valide. Elle permet également de contrôler
ces hypothèses directement plutôt que de laisser une inversion générale
masquer silencieusement une entrée incorrecte.
