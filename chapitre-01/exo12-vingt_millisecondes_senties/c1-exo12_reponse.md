# Exercice 12 — Vingt millisecondes senties

Pour cet exercice, j'ai réalisé un petit programme dans lequel un point affiché
à l'écran suit la position de la souris avec un retard réglable entre 0 et
200 millisecondes.

Le but est d'augmenter progressivement ce retard jusqu'au moment où
l'utilisateur commence à remarquer que le mouvement affiché ne suit plus
immédiatement sa souris.

## Seuils obtenus

Pour cette démonstration, voici cinq valeurs de test simulées :

| Personne | Retard à partir duquel une gêne est perceptible |
|---|---:|
| Personne 1 | 45 ms |
| Personne 2 | 55 ms |
| Personne 3 | 40 ms |
| Personne 4 | 65 ms |
| Personne 5 | 50 ms |

Les seuils ne sont pas identiques. Certaines personnes remarquent rapidement
le retard, alors que d'autres le tolèrent davantage.

## Comparaison avec le budget de 20 ms

Les seuils de cette démonstration sont supérieurs à 20 ms. Cela peut sembler
surprenant, mais le test est réalisé sur un écran ordinaire avec une souris.

Dans un casque de réalité virtuelle, la situation est différente : lorsque
l'utilisateur tourne la tête, son oreille interne détecte immédiatement le
mouvement. L'image affichée doit donc suivre ce mouvement avec un retard très
faible.

Si l'image du casque arrive trop tard, les informations données par la vision
peuvent ne plus correspondre correctement à celles données par l'oreille
interne.

C'est pourquoi le seuil acceptable doit être beaucoup plus bas dans un casque
que dans une simple expérience de suivi de souris sur un écran.

## Conclusion

Cette expérience montre qu'un retard peut devenir perceptible même sur un
écran ordinaire. En réalité virtuelle, les contraintes sont plus fortes,
car l'affichage doit rester cohérent avec les mouvements réels de la tête.

Le budget d'environ 20 ms pour le mouvement-vers-photon constitue donc une
contrainte importante pour maintenir une expérience confortable.
