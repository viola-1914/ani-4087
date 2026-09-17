# Exercice 11 — Le monde à la mauvaise échelle

## Objectif

Dans cet exercice, j'ai voulu observer ce qui se passe lorsque les dimensions
d'un environnement virtuel restent proportionnelles entre elles, mais que
l'échelle générale du monde est modifiée.

Pour cela, j'ai repris les dimensions de la salle utilisée précédemment.

Dimensions normales :

- salle : 6 m × 4 m × 2,80 m ;
- porte : 0,90 m × 2 m ;
- table : 1,20 m × 0,80 m × 0,80 m ;
- fenêtre : 1,50 m × 1,20 m.

Le programme lit un facteur d'échelle et multiplie toutes les dimensions
par ce facteur.

## Code C++

```cpp
#include <iostream>
#include <iomanip>

int main()
{
    double facteur;

    std::cin >> facteur;

    double salleLongueur = 6.0;
    double salleLargeur = 4.0;
    double salleHauteur = 2.8;

    double porteLargeur = 0.9;
    double porteHauteur = 2.0;

    double tableLongueur = 1.2;
    double tableLargeur = 0.8;
    double tableHauteur = 0.8;

    double fenetreLargeur = 1.5;
    double fenetreHauteur = 1.2;

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Salle : "
              << salleLongueur * facteur << " x "
              << salleLargeur * facteur << " x "
              << salleHauteur * facteur << " m\n";

    std::cout << "Porte : "
              << porteLargeur * facteur << " x "
              << porteHauteur * facteur << " m\n";

    std::cout << "Table : "
              << tableLongueur * facteur << " x "
              << tableLargeur * facteur << " x "
              << tableHauteur * facteur << " m\n";

    std::cout << "Fenetre : "
              << fenetreLargeur * facteur << " x "
              << fenetreHauteur * facteur << " m\n";

    return 0;
}
```

## Trois facteurs testés

Pour observer l'effet du changement d'échelle, j'ai utilisé trois facteurs :

- facteur 0,5 ;
- facteur 1 ;
- facteur 2.

Avec le facteur 0,5, toutes les dimensions sont divisées par deux.

Avec le facteur 1, les dimensions restent celles de la salle de référence.

Avec le facteur 2, toutes les dimensions sont doublées.

Par exemple, la salle de 6 m × 4 m × 2,80 m devient :

| Facteur | Dimensions de la salle |
|---:|---|
| 0,5 | 3,00 m × 2,00 m × 1,40 m |
| 1 | 6,00 m × 4,00 m × 2,80 m |
| 2 | 12,00 m × 8,00 m × 5,60 m |

## Contexte de l'expérience

J'ai repris l'expérience avec trois personnes : **Hendrix, Erwan et Thomas**.

Je leur ai présenté la pièce avec les trois facteurs d'échelle sans leur
annoncer à l'avance la valeur utilisée.

Je leur ai demandé d'observer la salle, la table, la porte et les autres
éléments, puis de me dire ce qui leur paraissait normal ou inhabituel dans
les dimensions de l'environnement.

Je leur ai également demandé quelle impression ils avaient de leur propre
taille par rapport à la pièce.

## Résultats obtenus

| Participant | Facteur | Impression principale |
|---|---:|---|
| Hendrix | 0,5 | La pièce lui paraît trop petite et il a l'impression d'être trop grand |
| Erwan | 1 | Les dimensions lui paraissent normales |
| Thomas | 2 | La pièce lui paraît immense et il a l'impression d'être petit |

### Hendrix — facteur 0,5

Avec le facteur 0,5, Hendrix a immédiatement trouvé que les proportions
étaient inhabituelles.

Il a surtout remarqué que la pièce et ses différents éléments semblaient
petits par rapport à lui. La table lui paraissait très basse et la porte
beaucoup plus petite que ce qu'il attendait.

Son impression générale était surtout d'être devenu **trop grand par rapport
à la salle**.

Ce qui m'a intéressé dans sa réaction est qu'il n'a pas seulement décrit
la pièce comme petite : il a aussi décrit sa propre taille comme trop grande
par rapport à l'environnement.

### Erwan — facteur 1

Avec le facteur 1, la réaction d'Erwan a été beaucoup plus simple.

Il n'a pas remarqué de problème particulier dans les proportions. La hauteur
de la table, la porte et les dimensions générales de la pièce lui semblaient
normales.

Il n'avait pas non plus l'impression d'être trop grand ou trop petit.

Cette observation constitue donc un bon point de comparaison avec les deux
autres facteurs.

### Thomas — facteur 2

Avec le facteur 2, Thomas a eu une impression pratiquement inverse de celle
d'Hendrix.

La pièce lui paraissait très grande et les objets semblaient beaucoup plus
imposants. La table lui semblait notamment trop haute par rapport à sa propre
taille.

Son impression générale était d'être devenu **petit par rapport à
l'environnement**.

Là encore, sa première interprétation ne consistait donc pas seulement à dire
que le monde avait changé de taille : il comparait spontanément sa propre
taille à celle du monde qui l'entourait.

## Comparaison des observations

Les trois essais donnent des impressions assez différentes :

- avec le facteur **0,5**, Hendrix trouve l'environnement trop petit et a
  l'impression d'être trop grand ;
- avec le facteur **1**, Erwan trouve les dimensions normales et ne remarque
  pas de problème d'échelle ;
- avec le facteur **2**, Thomas trouve l'environnement très grand et a
  l'impression d'être petit.

On peut résumer les observations ainsi :

**Échelle réduite → monde trop petit → impression d'être trop grand**

**Échelle normale → proportions cohérentes → impression normale**

**Échelle agrandie → monde trop grand → impression d'être trop petit**

## Ce que j'en tire

L'expérience m'a surtout permis de remarquer que les participants ne parlent
pas uniquement de la taille du monde.

Ils évaluent aussi leur propre taille par rapport à ce qu'ils voient.

Avec l'environnement réduit, Hendrix se sent trop grand. Avec
l'environnement agrandi, Thomas se sent au contraire trop petit.

Cela montre qu'une erreur d'échelle peut être ressentie même si la géométrie
de la scène reste cohérente et qu'aucun objet n'est techniquement mal placé.

Le programme peut donc fonctionner correctement tout en produisant une
impression perceptive incorrecte.

## Pourquoi tester avec d'autres personnes ?

Cette expérience montre aussi l'intérêt de faire observer la scène par
d'autres utilisateurs.

Lorsque je connais déjà les dimensions et le facteur que j'ai appliqué, je
sais à l'avance ce qui a changé.

Hendrix, Erwan et Thomas ont au contraire observé la pièce sans connaître
le facteur utilisé. Leurs premières impressions permettent donc de voir
comment l'échelle est réellement perçue par quelqu'un qui découvre la scène.

## Conclusion

Cette expérience m'a permis de vérifier concrètement l'effet d'un changement
d'échelle sur la perception d'un environnement virtuel.

Avec le facteur 0,5, Hendrix a eu l'impression d'être trop grand dans une
pièce devenue trop petite. Avec le facteur 1, Erwan a trouvé les proportions
normales. Avec le facteur 2, Thomas a au contraire eu l'impression d'être
petit dans un environnement devenu très grand.

Le résultat le plus intéressant est que le changement d'échelle influence
aussi la manière dont l'utilisateur perçoit sa propre taille par rapport
au monde.

En réalité virtuelle, travailler avec des dimensions cohérentes en mètres
est donc essentiel pour maintenir une relation crédible entre le corps de
l'utilisateur et l'environnement virtuel.
