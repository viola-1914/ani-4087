# Démo 2 — L'inversion qui ment

## Objectif

Cette démonstration montre le danger d'une fonction générale d'inversion
qui renvoie silencieusement la matrice identité lorsqu'elle ne peut pas
inverser une matrice.

J'ai d'abord exécuté le programme avec une matrice volontairement
dégénérée. Ensuite, avant d'expliquer ce que ce résultat pourrait provoquer
dans un casque VR, j'ai demandé à Hendrix, Erwan et Thomas d'imaginer les
conséquences.

## Matrice dégénérée

Je prends volontairement la matrice suivante :

```text
1  0  0  0
0  0  0  0
0  0  1  0
0  0  0  1
```

La deuxième ligne est entièrement nulle.

Cette matrice est donc dégénérée et ne possède pas d'inverse.

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
            // la fonction renvoie simplement l'identité.
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

## Exécution du programme

J'ai exécuté le programme avec cette matrice dégénérée.

Le programme m'a donné :

```text
Resultat de l'inversion :
1.0 0.0 0.0 0.0
0.0 1.0 0.0 0.0
0.0 0.0 1.0 0.0
0.0 0.0 0.0 1.0
```

Le résultat est donc la matrice identité.

Le point qui m'a surtout marqué est qu'**aucun message d'erreur n'est
affiché**.

La fonction n'a pas réussi à inverser la matrice, mais elle renvoie quand
même une matrice qui semble parfaitement valide.

## Question posée avant la révélation

Avant d'expliquer ce que ce résultat donnerait dans un casque, j'ai demandé
à **Hendrix, Erwan et Thomas** de l'imaginer eux-mêmes.

Je leur ai posé la question suivante :

> « Si cette matrice était utilisée comme matrice de vue dans un casque VR,
> qu'est-ce que vous pensez qu'il se passerait ? »

Je n'ai pas donné immédiatement la réponse afin de recueillir d'abord leurs
suppositions.

## Réactions avant la révélation

Les trois hypothèses n'étaient pas exactement les mêmes.

### Hendrix

Hendrix pensait que la vue risquait de se retrouver à une mauvaise position,
puisque la transformation attendue n'était plus disponible.

Son idée était donc surtout qu'il y aurait un problème de positionnement de
la vue dans le casque.

### Erwan

Erwan pensait surtout que le mouvement de la tête ne serait plus correctement
reproduit.

Pour lui, l'utilisateur pourrait tourner ou déplacer la tête sans retrouver
le mouvement attendu dans l'image.

### Thomas

Thomas imaginait plutôt que le décor pourrait changer brutalement de
position, comme si le point de vue avait été replacé à une position par
défaut.

Les trois réponses se rapprochaient donc du problème, mais aucune ne disait
encore précisément pourquoi la matrice identité produirait ce comportement.

## Révélation

Après avoir recueilli leurs suppositions, j'ai repris le résultat affiché
par le programme :

```text
1.0 0.0 0.0 0.0
0.0 1.0 0.0 0.0
0.0 0.0 1.0 0.0
0.0 0.0 0.0 1.0
```

J'ai alors expliqué qu'il s'agit de la **matrice identité**.

C'est une matrice parfaitement valide.

Le problème n'est donc pas que le programme reçoive forcément une valeur
manifestement incorrecte comme un `NaN`.

Au contraire, il reçoit quelque chose de plausible.

C'est précisément ce qui rend l'échec trompeur.

## Ce que cela donnerait dans un casque

Si cette matrice identité était utilisée comme matrice de vue à la place de
la matrice attendue, la transformation correspondant à la pose de la caméra
serait perdue.

L'identité correspond à l'absence de transformation.

La caméra pourrait donc revenir à l'origine, sans la translation et la
rotation correspondant à la pose attendue.

On peut représenter le problème ainsi :

```text
Pose correcte
     |
     v
position + orientation de la tête
     |
     v
matrice de vue correcte


Matrice dégénérée
     |
     v
inversion impossible
     |
     v
identité renvoyée silencieusement
     |
     v
transformation attendue perdue
     |
     v
caméra ramenée à l'origine
```

Cela rejoint en partie les suppositions faites avant la révélation.

Hendrix avait imaginé un mauvais positionnement de la vue.

Erwan avait pensé à une mauvaise correspondance entre le mouvement de la
tête et celui de l'image.

Thomas avait imaginé un déplacement brutal du décor vers une position par
défaut.

La révélation permet de comprendre la cause commune possible de ces effets :
**la matrice identité a remplacé silencieusement la transformation qui aurait
dû représenter la vue.**

## Pourquoi cette erreur est difficile à détecter

Ce qui rend ce comportement dangereux n'est pas seulement que l'inversion
échoue.

Le problème est surtout que son échec est caché derrière une valeur
parfaitement plausible.

Le déroulement est :

```text
matrice non inversible
        |
        v
échec de l'inversion
        |
        v
identité renvoyée
        |
        v
aucune erreur signalée
        |
        v
le programme continue
```

Si la fonction renvoyait des valeurs manifestement invalides, le problème
serait plus facile à repérer.

Mais une matrice identité peut tout à fait apparaître dans un programme
normal.

Elle peut donc masquer le fait que l'inversion a réellement échoué.

## Ce que la démonstration m'a appris

Le fait de demander d'abord aux autres d'imaginer le résultat m'a permis de
voir que les conséquences ne sont pas immédiatement évidentes.

Les trois participants ont imaginé des manifestations différentes du
problème.

C'est seulement après avoir regardé précisément la matrice retournée que la
cause devient claire : la fonction ne signale pas son échec et remplace le
résultat attendu par l'identité.

La difficulté n'est donc pas seulement technique.

Pour l'utilisateur du casque, quelque chose peut sembler soudainement
incorrect dans la vue alors que, du côté du programme, aucune erreur
explicite n'apparaît.

## Lien avec les poses XR

Pour une pose XR, on connaît déjà la structure de la transformation :

- une position ;
- une orientation représentée par un quaternion.

On peut donc construire directement l'inverse de la pose en exploitant cette
structure.

Cette approche permet de rendre les hypothèses plus explicites et d'éviter
de dépendre aveuglément d'une fonction générale d'inversion qui peut masquer
son échec derrière une matrice valide.

## Conclusion

Cette démonstration a commencé par l'exécution réelle du programme sur une
matrice volontairement dégénérée.

L'inversion était impossible, mais le programme a renvoyé :

```text
1.0 0.0 0.0 0.0
0.0 1.0 0.0 0.0
0.0 0.0 1.0 0.0
0.0 0.0 0.0 1.0
```

sans afficher de message d'erreur.

Avant de donner l'explication, j'ai demandé à Hendrix, Erwan et Thomas
d'imaginer ce que cela pourrait produire dans un casque.

Hendrix pensait à une vue placée au mauvais endroit, Erwan à un mouvement de
tête qui ne serait plus correctement reproduit, et Thomas à un changement
brutal du point de vue vers une position par défaut.

Après ces suppositions, la révélation de la matrice identité permet de
comprendre le véritable piège.

Une matrice identité est une matrice parfaitement valide. Elle peut donc
masquer le fait que l'inversion a réellement échoué.

Si elle remplace la matrice de vue attendue, la transformation de la caméra
est perdue et la vue peut revenir à l'origine sans que le programme explique
directement pourquoi.

C'est ce caractère **silencieux et plausible** de l'erreur qui rend ce type
d'échec particulièrement dangereux dans une application XR.
