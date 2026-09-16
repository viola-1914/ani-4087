# Démo 1 — Le budget au tableau

Pour cette démonstration, j'ai représenté le budget motion-to-photon de
20 millisecondes sous la forme d'une barre.

L'objectif est de visualiser la place occupée par les différentes étapes
avant même de considérer le temps disponible pour notre propre code.

## Les cinq étapes

D'après les valeurs données dans le chapitre :

| Étape | Budget indiqué |
|---|---:|
| Capteurs | 1 à 2 ms |
| Transmission | 1 à 3 ms |
| Application / rendu | 5 à 11 ms |
| Composition | 1 à 2 ms |
| Affichage | 2 à 5 ms |

## Représentation au tableau

J'ai représenté les 20 ms par une barre :

`0 ms |------------------------------| 20 ms`

Une représentation possible avec des valeurs intermédiaires est :

`Capteurs | Transmission | Application/rendu | Composition | Affichage`

Par exemple :

`2 ms + 2 ms + 10 ms + 2 ms + 4 ms = 20 ms`

Ce découpage montre immédiatement que les vingt millisecondes ne sont pas
entièrement disponibles pour le programme.

## Ce qui reste pour le code

Une partie du temps est déjà consommée par les capteurs, la transmission,
le compositeur et l'affichage.

Le rendu de l'application doit donc travailler dans un budget très réduit.
Dans le chapitre, le temps réellement disponible pour notre code est de
l'ordre d'une dizaine de millisecondes.

À 90 Hz, une image entière ne dure par exemple qu'environ :

`1000 / 90 = 11,1 ms`

Le programme doit donc terminer son travail à temps pour chaque image.

## Réaction à retenir

Ce qui m'a surtout marqué dans cette représentation est que 20 ms peuvent
sembler être un temps raisonnable lorsqu'on les considère seules.

Mais une fois toutes les étapes placées sur la barre, on voit qu'une grande
partie de ce temps est déjà utilisée avant que notre code puisse terminer
le rendu.

Quelques millisecondes perdues peuvent donc suffire à dépasser le budget.

## Conclusion

La barre des vingt millisecondes permet de comprendre visuellement pourquoi
les performances sont si importantes en réalité virtuelle.

Il ne suffit pas d'avoir une bonne moyenne : chaque image doit respecter son
budget. Une seule étape trop lente peut retarder toute la chaîne et dégrader
l'expérience dans le casque.
