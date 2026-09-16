# Démo 2 — L'inversion qui ment

Cette démonstration montre le danger d'une fonction générale d'inversion
qui renvoie silencieusement la matrice identité lorsqu'elle ne peut pas
inverser une matrice.

## Matrice dégénérée

Je prends volontairement une matrice non inversible :

```text
1  0  0  0
0  0  0  0
0  0  1  0
0  0  0  1
```

La deuxième ligne est entièrement nulle. La matrice est donc dégénérée et
ne possède pas d'inverse.

## Code C++

```cpp
#include <iostream>
#include <iomanip>
#include <cmath>

struct Mat4
{
    double m[4][4];
};

Mat4 identite()
{
    Mat4 I{};

    for (int i = 0; i < 4; ++i)
        I.m[i][i] = 1.0;

    return I;
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
            // Problème : échec silencieux.
            // La fonction renvoie simplement l'identité.
            return identite();
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

    Mat4 resultat{};

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            resultat.m[i][j] = aug[i][j + 4];

    return resultat;
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
    Mat4 degeneree{};

    degeneree.m[0][0] = 1.0;

    // Ligne 1 entièrement nulle

    degeneree.m[2][2] = 1.0;
    degeneree.m[3][3] = 1.0;

    Mat4 resultat = inverseGenerale(degeneree);

    std::cout << std::fixed << std::setprecision(1);

    std::cout << "Resultat de l'inversion :\n";
    afficher(resultat);

    return 0;
}
```

## Résultat

La matrice est dégénérée et ne devrait pas pouvoir être inversée.

Pourtant, la fonction renvoie :

```text
1.0 0.0 0.0 0.0
0.0 1.0 0.0 0.0
0.0 0.0 1.0 0.0
0.0 0.0 0.0 1.0
```

C'est la matrice identité.

Aucun message d'erreur n'est affiché.

## Ce que cela donnerait dans un casque

Si cette matrice était utilisée comme matrice de vue dans un casque de
réalité virtuelle, le problème serait particulièrement trompeur.

Au lieu de signaler que la pose est invalide, le système utiliserait
l'identité.

La caméra reviendrait alors à l'origine, sans translation et sans rotation.

L'utilisateur verrait donc brutalement le monde depuis la position d'origine,
mais le programme ne donnerait aucune explication sur la cause du problème.

## Conclusion

Cette démonstration montre pourquoi un échec silencieux est dangereux.

Une matrice identité est une matrice parfaitement valide. Elle peut donc
masquer le fait que l'inversion a réellement échoué.

Pour une pose XR, construire directement son inverse à partir de la position
et du quaternion permet de rendre les hypothèses plus explicites et d'éviter
ce type de comportement trompeur.
