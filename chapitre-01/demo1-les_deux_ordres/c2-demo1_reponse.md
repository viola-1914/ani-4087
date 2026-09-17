# Démo 1 — Les deux ordres

Cette démonstration permet de montrer concrètement que l'ordre entre une
rotation et une translation change le résultat final.

Je suis parti du même point dans les deux cas avec :

- position initiale : `(0, 0, 0)` ;
- déplacement : 1 mètre vers l'avant ;
- rotation : 90 degrés vers la droite.

J'ai d'abord réalisé la démonstration avec un objet réel, puis j'ai comparé
ce que nous avions observé avec les résultats numériques du programme.

## Démonstration avec un objet réel

J'ai utilisé un stylo comme objet pour représenter sa position et son
orientation.

Avant d'effectuer les deux mouvements, j'ai demandé à Hendrix, Erwan et
Thomas ce qu'ils pensaient qu'il allait se passer.

Hendrix pensait d'abord que les deux ordres allaient donner le même résultat,
puisque la rotation et le déplacement étaient identiques.

Erwan a remarqué que si je tournais d'abord le stylo avant de l'avancer,
le déplacement devait se faire dans sa nouvelle direction.

Thomas hésitait surtout sur le deuxième cas. Après la manipulation, il a
compris que lorsqu'on avance d'abord puis qu'on tourne sur place, la rotation
finale ne change plus la position déjà atteinte.

## Premier cas : je tourne puis j'avance

Je suis parti du point initial.

J'ai d'abord tourné le stylo de **90 degrés vers la droite**.

Ensuite, je l'ai avancé de **1 mètre dans sa nouvelle direction**.

Le déplacement s'est donc fait vers la droite par rapport au point de départ.

Schématiquement :

```text
            Avant
              ↑

          O ─────────→ X
        départ        final
```

La rotation effectuée avant le déplacement a changé la direction dans
laquelle le stylo avançait.

## Deuxième cas : j'avance puis je tourne

J'ai replacé le stylo exactement au même point de départ.

Cette fois, je l'ai d'abord avancé de **1 mètre dans sa direction initiale**.

Une fois le déplacement terminé, je l'ai tourné de **90 degrés vers la
droite sur place**.

La rotation a donc changé son orientation, mais elle n'a pas déplacé le
stylo vers la droite.

Schématiquement :

```text
              X final
              ↑
              |
              |
            O départ
```

Les deux mouvements utilisent donc la même translation et la même rotation,
mais ils n'aboutissent pas à la même position finale.

## Ce que les réactions ont montré

La manipulation avec le stylo a rendu la différence beaucoup plus claire.

Hendrix pensait initialement que les deux ordres donneraient le même résultat
parce que les mêmes opérations étaient utilisées.

Erwan avait déjà remarqué que tourner avant d'avancer devait modifier la
direction du déplacement.

Thomas a surtout compris avec le deuxième mouvement que tourner après avoir
avancé ne peut pas modifier rétroactivement le déplacement déjà effectué.

La démonstration montre donc concrètement que ce n'est pas seulement la liste
des transformations qui compte : **leur ordre compte également**.

## Vérification avec le programme

J'ai ensuite repris le même principe dans le programme.

J'ai utilisé la convention suivante :

- origine : `(0, 0, 0)` ;
- direction initiale vers l'avant : axe `-Z` ;
- déplacement : `1 m` ;
- rotation : `90°` vers la droite autour de l'axe Y.

J'ai exécuté les deux ordres séparément.

### Tourner puis avancer

Le programme m'a donné :

```text
Tourner puis avancer :
x = 1.000
y = 0.000
z = 0.000
```

La position finale est donc :

`(1.000, 0.000, 0.000)`

Après la rotation de 90° vers la droite, l'avant initial dirigé selon `-Z`
est orienté vers `+X`.

Le déplacement de 1 mètre se fait donc dans cette nouvelle direction.

## Avancer puis tourner

Pour le deuxième ordre, le programme m'a donné :

```text
Avancer puis tourner :
x = 0.000
y = 0.000
z = -1.000
```

La position finale est donc :

`(0.000, 0.000, -1.000)`

Dans ce cas, le déplacement de 1 mètre est effectué avant la rotation.

L'objet avance donc dans sa direction initiale, selon `-Z`.

La rotation effectuée ensuite change son orientation, mais elle ne change
plus la position obtenue après le déplacement.

## Comparaison des deux résultats

Les deux résultats du programme sont :

| Ordre des opérations | Position finale |
|---|---|
| Tourner puis avancer | **(1.000, 0.000, 0.000)** |
| Avancer puis tourner | **(0.000, 0.000, -1.000)** |

Les deux positions sont clairement différentes.

On peut donc écrire :

`rotation puis translation ≠ translation puis rotation`

dans cette situation.

Le programme confirme numériquement ce que nous avons observé avec le stylo.

## Ce que j'en retiens

Cette démonstration m'a permis de voir la différence de deux manières.

Avec l'objet réel, la différence est directement visible : tourner avant de
se déplacer modifie la direction du déplacement.

Avec le programme, cette différence apparaît dans les coordonnées finales :

```text
(1.000, 0.000, 0.000)
```

contre :

```text
(0.000, 0.000, -1.000)
```

Le fait d'utiliser les mêmes opérations ne suffit donc pas pour obtenir le
même résultat. Leur ordre est essentiel.

## Conclusion

Cette démonstration montre concrètement que l'ordre des transformations est
important.

Quand je tourne avant d'avancer, le déplacement se fait dans la nouvelle
direction.

Quand j'avance avant de tourner, le déplacement est effectué dans la
direction initiale et la rotation suivante change seulement l'orientation
de l'objet.

La manipulation avec le stylo et les résultats numériques du programme
aboutissent donc à la même conclusion : **rotation puis translation et
translation puis rotation ne produisent généralement pas le même résultat**.
