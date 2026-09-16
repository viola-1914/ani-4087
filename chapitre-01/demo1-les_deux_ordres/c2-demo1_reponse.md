# Démo 1 — Les deux ordres

Cette démonstration permet de montrer que l'ordre entre une rotation et une
translation change le résultat final.

Je pars du même point dans les deux cas et je prends :

- position initiale : (0, 0, 0) ;
- déplacement : 1 mètre vers l'avant ;
- rotation : 90 degrés vers la droite.

## Démonstration avec un objet réel

J'ai représenté le mouvement avec un objet posé au même point de départ.

### Premier cas : je tourne puis j'avance

Je tourne d'abord l'objet de 90 degrés vers la droite.

Ensuite, je le fais avancer de 1 mètre dans sa nouvelle direction.

L'objet se retrouve donc sur le côté droit par rapport à sa position de départ.

### Deuxième cas : j'avance puis je tourne

Je repars exactement du même point.

Cette fois, je fais d'abord avancer l'objet de 1 mètre dans sa direction
initiale.

Ensuite, je le tourne de 90 degrés sur place.

Sa position finale reste donc devant le point de départ, même si son
orientation a changé.

## Illustration

Départ :

          Avant
            ↑
            |
            O

Tourner puis avancer :

            O départ
            |
            └────────→ X final

Avancer puis tourner :

            X final
            ↑
            |
            |
            O départ

Les deux mouvements commencent au même endroit et utilisent la même rotation
et la même distance, mais ils n'aboutissent pas au même point.

## Résultats du programme

Le programme de l'exercice précédent permet de comparer les deux ordres.

Pour un déplacement de 1 mètre et une rotation de 90 degrés, on obtient deux
positions différentes.

Cela confirme que :

`rotation puis translation`

n'est généralement pas équivalent à :

`translation puis rotation`

## Conclusion

Cette démonstration montre concrètement que l'ordre des transformations est
important.

Quand je tourne avant d'avancer, le déplacement se fait dans la nouvelle
direction. Quand j'avance avant de tourner, le déplacement est effectué dans
la direction initiale et la rotation suivante change seulement l'orientation
de l'objet.
