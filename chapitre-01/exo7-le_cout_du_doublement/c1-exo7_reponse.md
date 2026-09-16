# Exercice 7 — Le coût du doublement

Pour cette démonstration, j'ai considéré uniquement le temps nécessaire au rendu
de la scène, sans prendre en compte la logique du programme.

La mesure utilisée est la suivante :

- Temps d'un rendu : 4,2 ms
- Estimation pour deux rendus : 4,2 × 2 = 8,4 ms

En prenant comme référence un casque fonctionnant à 90 Hz, une image doit être
produite en environ 11,1 ms.

Le temps restant pour les autres opérations serait donc :

11,1 - 8,4 = 2,7 ms

## Interprétation

Le fait de rendre la scène deux fois, une fois pour chaque œil, utilise une grande
partie du temps disponible.

Avec un rendu de 4,2 ms pour un œil, les deux rendus prennent environ 8,4 ms.
Il ne reste alors que 2,7 ms pour les autres traitements.

Cela montre qu'en réalité virtuelle, il est important de réduire surtout le coût
du rendu graphique.

Pour améliorer les performances, il faudrait par exemple réduire la complexité
des objets affichés, limiter certains effets graphiques coûteux et éviter les
calculs inutiles pendant le rendu.

## Conclusion

Le doublement du rendu représente un coût important. Pour respecter le temps
disponible à chaque image, il faut donc optimiser le rendu afin de conserver
suffisamment de temps pour le reste du programme.
