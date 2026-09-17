# Démo 2 — L'inversion qui ment

## Objectif

Cette démonstration montre le danger d'une fonction générale d'inversion
qui renvoie silencieusement la matrice identité lorsqu'elle ne peut pas
inverser une matrice.

L'objectif est également de voir si ce comportement est facile à anticiper
lorsqu'on imagine ses conséquences dans un casque de réalité virtuelle.

## Matrice dégénérée

Je prends volontairement une matrice non inversible :

```text
1  0  0  0
0  0  0  0
0  0  1  0
0  0  0  1
```

La deuxième ligne est entièrement nulle.

La matrice est donc dégénérée et ne possède pas d'inverse.

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
            // Échec silencieux :
            // la fonction renvoie l'identité.
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

    // Deuxième ligne entièrement nulle

    degeneree.m[2][2] = 1.0;
    degeneree.m[3][3] = 1.0;

    Mat4 resultat = inverseGenerale(degeneree);

    std::cout << std::fixed << std::setprecision(1);

    std::cout << "Resultat de l'inversion :\n";

    afficher(resultat);

    return 0;
}
```

## Exécution

J'ai exécuté le programme avec cette matrice dégénérée.

La fonction d'inversion a renvoyé :

```text
Resultat de l'inversion :
1.0 0.0 0.0 0.0
0.0 1.0 0.0 0.0
0.0 0.0 1.0 0.0
0.0 0.0 0.0 1.0
```

Il s'agit de la matrice identité.

Le point important est qu'**aucun message d'erreur n'est affiché**.

La fonction échoue donc à inverser la matrice, mais elle renvoie malgré tout
une matrice qui paraît parfaitement valide.

## Question posée avant de donner l'explication

Je n'ai pas commencé par expliquer les conséquences.

J'ai d'abord montré que l'inversion d'une matrice non inversible venait de
produire l'identité.

J'ai ensuite posé la question :

> « Si cette matrice était utilisée comme matrice de vue dans un casque VR,
> qu'est-ce que vous pensez qu'il se passerait ? »

Le but était de recueillir les suppositions avant de révéler la réponse.

## Suppositions recueillies

### Hendrix

> « [Écrire ici exactement ce qu'Hendrix a répondu.] »

### Erwan

> « [Écrire ici exactement ce qu'Erwan a répondu.] »

### Thomas

> « [Écrire ici exactement ce que Thomas a répondu.] »

Ces réponses sont importantes parce que le comportement d'un échec silencieux
n'est pas forcément celui auquel on pense immédiatement.

Après avoir recueilli les suppositions, j'ai expliqué ce que signifie
réellement la matrice retournée par le programme.

## La révélation

Le résultat n'est ni une matrice remplie de valeurs invalides, ni un message
d'erreur.

C'est :

```text
1 0 0 0
0 1 0 0
0 0 1 0
0 0 0 1
```

c'est-à-dire une **matrice identité parfaitement valide**.

C'est justement ce qui rend cet échec trompeur.

Un résultat manifestement invalide, par exemple contenant des `NaN`, pourrait
attirer rapidement l'attention.

Ici, au contraire, le programme continue avec une matrice qui ressemble à
un résultat normal.

## Ce que cela donnerait dans un casque

Si cette identité était utilisée comme matrice de vue, la transformation
attendue de la caméra serait perdue.

L'identité correspond à l'absence de transformation.

La caméra se retrouverait donc à l'origine, sans translation et sans
rotation correspondant à la pose qui aurait dû être utilisée.

Du point de vue de l'utilisateur, le changement pourrait être brutal :

```text
Pose correcte
     ↓
position + orientation de la tête
     ↓
vue correcte


Échec de l'inversion
     ↓
identité renvoyée silencieusement
     ↓
position/orientation attendues perdues
     ↓
caméra ramenée à l'origine
```

Le problème est particulièrement difficile à comprendre parce que la
fonction d'inversion ne dit jamais :

```text
Erreur : matrice non inversible
```

Le reste du programme reçoit simplement une matrice valide.

## Pourquoi l'identité est trompeuse

Le danger ne vient donc pas seulement du fait que la fonction échoue.

Le véritable problème est qu'elle **cache son échec derrière une valeur
plausible**.

On a :

```text
matrice dégénérée
       ↓
inversion impossible
       ↓
identité renvoyée
       ↓
aucune erreur signalée
       ↓
le programme continue
```

Cela peut faire chercher le problème au mauvais endroit.

On pourrait croire que la pose, le suivi de tête ou une autre partie du
programme est incorrecte alors que l'origine du problème est simplement
l'échec de l'inversion.

## Ce que la démonstration m'a fait comprendre

Avant cette expérience, on pourrait penser qu'une inversion impossible
provoquerait forcément une erreur évidente.

Ce programme montre que ce n'est pas nécessairement le cas.

La fonction peut produire quelque chose de mathématiquement valide tout en
ayant échoué dans ce qu'on lui demandait.

Dans le cas présent, le résultat est particulièrement trompeur parce que
l'identité signifie une transformation parfaitement normale.

## Lien avec les poses XR

Pour une pose XR, on connaît la structure de la transformation :

- une position ;
- une orientation représentée par un quaternion.

On peut donc construire directement l'inverse de cette pose en utilisant
cette structure au lieu de dépendre aveuglément d'une inversion générale de
matrice qui pourrait masquer son échec.

Cela permet aussi de rendre les hypothèses et les cas invalides plus
explicites.

## Conclusion

Cette démonstration m'a permis d'observer un échec particulièrement
trompeur.

J'ai fourni à la fonction une matrice volontairement dégénérée :

```text
1  0  0  0
0  0  0  0
0  0  1  0
0  0  0  1
```

L'inversion était impossible.

Pourtant, le programme a affiché :

```text
1.0 0.0 0.0 0.0
0.0 1.0 0.0 0.0
0.0 0.0 1.0 0.0
0.0 0.0 0.0 1.0
```

sans aucun message d'erreur.

Le danger est donc qu'une matrice identité est parfaitement plausible et
peut masquer complètement l'échec de l'inversion.

Si elle est ensuite utilisée comme matrice de vue, la transformation attendue
est perdue et la caméra peut revenir à l'origine, sans translation ni
rotation correspondant à la pose attendue.

C'est pourquoi un échec silencieux de ce type est particulièrement dangereux
dans une application XR : **le programme peut continuer à fonctionner tout
en utilisant une transformation incorrecte, sans expliquer directement
l'origine du problème.**
