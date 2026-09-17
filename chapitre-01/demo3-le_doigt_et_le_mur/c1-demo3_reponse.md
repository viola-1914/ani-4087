# Démo 3 — Le doigt et le mur

## Objectif

Cette démonstration permet de constater directement que l'œil gauche et
l'œil droit ne voient pas exactement la même image d'une scène.

J'ai réalisé l'expérience avec le doigt placé à deux distances différentes :
d'abord à environ 30 cm, puis beaucoup plus loin, à environ 3 m.

## Première observation : le doigt à 30 cm

Je place mon doigt à environ 30 cm devant mes yeux et je choisis un point
fixe du mur comme repère.

Sans déplacer la tête ni le doigt, je ferme alternativement l'œil gauche
puis l'œil droit.

Le résultat est très visible : le doigt semble changer de position par
rapport au point choisi sur le mur.

Pourtant, le doigt est resté exactement au même endroit.

Le changement vient donc du fait que les deux yeux observent la scène depuis
deux positions légèrement différentes.

## Deuxième observation : le doigt à environ 3 m

Je recommence ensuite avec le doigt beaucoup plus éloigné.

En fermant alternativement les deux yeux, je remarque encore une différence,
mais elle est nettement moins importante que lorsque le doigt était proche.

La comparaison est donc claire :

| Distance | Observation |
|---|---|
| Environ 30 cm | Décalage très visible entre les deux vues |
| Environ 3 m | Décalage beaucoup plus faible |

## Ce que montre l'expérience

Cette différence entre ce que voit l'œil gauche et ce que voit l'œil droit
correspond à la disparité binoculaire.

Elle est particulièrement importante pour les objets proches et devient
plus faible lorsque les objets sont éloignés.

C'est l'une des informations que le cerveau peut utiliser pour apprécier
la profondeur.

## Pourquoi faut-il dessiner la scène deux fois en VR ?

Cette expérience donne une réponse assez directe.

Si mes deux yeux ne voient pas exactement la même image dans le monde réel,
un casque VR ne doit pas non plus leur présenter exactement le même point
de vue.

Il faut donc produire :

- une vue calculée depuis la position de l'œil gauche ;
- une vue calculée depuis la position de l'œil droit.

Les deux images représentent la même scène, mais depuis deux positions
légèrement différentes.

## Conclusion

L'expérience du doigt rend le principe beaucoup plus concret.

À courte distance, le changement apparent de position du doigt entre les
deux yeux est évident. En l'éloignant, ce changement devient beaucoup moins
important.

Cela explique pourquoi une application de réalité virtuelle doit calculer
deux vues de la scène : une pour chaque œil.

Ce n'est donc pas simplement la même image affichée deux fois. Les deux
images doivent correspondre aux deux points de vue différents de nos yeux.
