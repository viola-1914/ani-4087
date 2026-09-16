# Exercice 11 — Le monde à la mauvaise échelle

Pour cet exercice, j'ai repris les dimensions de la salle utilisée précédemment.

Dimensions normales :

- salle : 6 m × 4 m × 2,80 m ;
- porte : 0,90 m × 2 m ;
- table : 1,20 m × 0,80 m × 0,80 m ;
- fenêtre : 1,50 m × 1,20 m.

Le programme lit un facteur d'échelle et multiplie toutes les dimensions par
ce facteur.

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

Pour observer l'effet d'une mauvaise échelle, j'ai choisi trois facteurs :

- facteur 0,5 ;
- facteur 1 ;
- facteur 2.

Avec 0,5, tous les objets paraissent deux fois plus petits.

Avec 1, les dimensions restent celles de la salle de référence.

Avec 2, toutes les dimensions sont doublées.

## Trois descriptions

Les descriptions suivantes sont des témoignages simulés pour la démonstration.

### Personne 1 — facteur 0,5

« La pièce me paraît vraiment petite. La table semble très basse et la porte
donne l'impression d'avoir été construite pour quelqu'un beaucoup plus petit.
J'ai l'impression d'être devenu très grand par rapport à la salle. »

### Personne 2 — facteur 1

« La pièce me semble normale. La hauteur de la table et celle de la porte
correspondent à ce que j'ai l'habitude de voir dans une vraie pièce. Rien ne
me paraît particulièrement trop grand ou trop petit. »

### Personne 3 — facteur 2

« Tout me paraît immense. La table semble beaucoup trop haute et la porte
ressemble presque à une porte géante. J'ai plutôt l'impression d'être devenu
très petit dans la pièce. »

## Ce que j'en tire

Les trois descriptions montrent que l'échelle influence directement la
perception de l'environnement.

Lorsque le facteur est inférieur à 1, l'environnement paraît trop petit et
l'utilisateur peut avoir l'impression d'être devenu géant.

Lorsque le facteur est supérieur à 1, l'environnement paraît trop grand et
l'utilisateur peut avoir l'impression d'être devenu minuscule.

Avec un facteur égal à 1, les dimensions restent exprimées à leur taille
physique normale.

## Conclusion

En réalité virtuelle, travailler en mètres permet de conserver une relation
cohérente entre les dimensions du corps de l'utilisateur et celles du monde
virtuel.

Une erreur d'échelle peut donc être perceptible même lorsque la géométrie de
la scène est correcte.
