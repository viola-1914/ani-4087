# Exercice 8 — Le champ de vision asymétrique

Pour cet exercice, j'ai choisi le Meta Quest 3.

Pour l'œil gauche, les valeurs relevées sont approximativement :

- Gauche : -54°
- Droite : +40°
- Haut : +43,98°
- Bas : -54,27°

On constate donc que le champ de vision n'est pas parfaitement symétrique.
Par exemple, l'œil gauche voit davantage vers l'extérieur (54°) que vers
l'intérieur (40°).

## Source

HMD Geometry Database — Meta Quest 3 (Native 72 Hz) :
https://risa2000.github.io/hmdgdb/hmd_cfgs/MetaQuest3_Native_R72.html

La documentation OpenXR définit également le champ de vision d'un œil avec
quatre angles séparés : gauche, droite, haut et bas.

## Que se passerait-il avec un champ symétrique de même surface ?

Si on remplaçait directement ce champ asymétrique par un champ symétrique de
même surface sans tenir compte de la géométrie réelle de l'œil et des lentilles,
certaines zones ne correspondraient plus exactement au champ visible réel :
on pourrait rendre inutilement certaines parties de l'image et risquer d'en
rogner d'autres.

## Conclusion

Cette recherche montre pourquoi un casque fournit quatre angles séparés pour
chaque œil. Une seule valeur de champ de vision ne suffit pas toujours pour
représenter correctement ce que l'œil peut voir.
