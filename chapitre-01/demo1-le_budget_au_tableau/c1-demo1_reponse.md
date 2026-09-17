# Démonstration 1 — Le budget au tableau

## Objectif

Cette démonstration permet de visualiser comment les vingt millisecondes
disponibles entre le mouvement de la tête et l'affichage de l'image sont
réparties entre les différentes étapes du système.

## Répartition du budget

J'ai représenté une barre correspondant à un budget total de 20 ms.

| Étape | Durée retenue |
|---|---:|
| Capteurs | 2 ms |
| Transmission | 2 ms |
| Application et rendu | 10 ms |
| Composition | 2 ms |
| Affichage | 4 ms |
| **Total** | **20 ms** |

Représentation simplifiée :

```text
0 ms                                              20 ms
|---------------------------------------------------|
|Capteurs|Trans.|      Application / rendu      |Comp.|Affichage|
|  2 ms  | 2 ms |             10 ms             |2 ms |  4 ms   |
