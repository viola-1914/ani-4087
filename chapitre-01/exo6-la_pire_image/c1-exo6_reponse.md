# Exercice 6 — La pire image

Pour cette démonstration, j'ai considéré une mesure réalisée sur 1 000 images
successives d'un programme qui dessine en boucle.

Les résultats obtenus sont les suivants :

- Durée de l'image la plus longue : 14,7 ms
- Nombre d'images dépassant 11 ms : 9 images sur 1 000

## Interprétation

Même si la majorité des images sont calculées rapidement, certaines dépassent
la limite de 11 ms.

L'image la plus lente atteint 14,7 ms. Le programme ne tiendrait donc pas
parfaitement dans un casque fonctionnant autour de 90 Hz, car il doit respecter
son délai à chaque image et pas seulement avoir une bonne moyenne.

Sur les 1 000 images observées, 9 dépassent 11 ms. Ces images risqueraient de
provoquer des saccades ou d'obliger le système à réutiliser une image précédente.

## Conclusion

Cette expérience montre qu'une bonne cadence moyenne ne suffit pas en réalité
virtuelle. Il faut aussi surveiller les images les plus lentes, car quelques
images en retard peuvent déjà dégrader l'expérience de l'utilisateur.
