# Démo 4 — Le retard réglable

Cette démonstration consiste à faire varier progressivement le retard entre
le mouvement de la souris et le déplacement de l'objet affiché à l'écran.

Le but est de déterminer à partir de quel retard une personne commence à
sentir que l'affichage ne répond plus immédiatement à son mouvement.

## Déroulement

Le programme est d'abord utilisé avec un retard nul.

Le retard est ensuite augmenté progressivement pendant que la personne
continue à déplacer la souris.

On lui demande de signaler le moment où elle commence à sentir que le
mouvement affiché arrive en retard.

## Seuils obtenus

Pour cette démonstration, j'utilise les valeurs simulées suivantes :

| Personne | Premier retard perceptible |
|---|---:|
| Personne 1 | 25 ms |
| Personne 2 | 35 ms |
| Personne 3 | 50 ms |

Ces valeurs montrent que les trois personnes ne commencent pas à percevoir
le retard exactement au même moment.

La première personne est plus sensible au retard que la troisième.

## Ce que cela montre

La perception de la latence varie donc selon les utilisateurs.

Un système ne doit pas être conçu uniquement pour une personne qui tolère
facilement un retard important.

Il faut chercher à maintenir la latence suffisamment basse pour que
l'expérience reste confortable également pour les utilisateurs plus
sensibles.

## Rapport avec le budget d'une image

En réalité virtuelle, le budget disponible pour produire une image est très
court.

À 90 Hz, par exemple :

`1000 / 90 = 11,1 ms`

Une nouvelle image doit donc être produite environ toutes les 11,1 ms.

À 120 Hz, ce temps descend encore :

`1000 / 120 = 8,3 ms`

Un retard de plusieurs dizaines de millisecondes peut donc représenter
plusieurs images de retard.

Cela explique pourquoi quelques millisecondes supplémentaires sont importantes
dans une application de réalité virtuelle.

## Conclusion

Cette démonstration montre que la latence peut devenir perceptible assez
rapidement et que le seuil varie d'une personne à l'autre.

Pour un casque de réalité virtuelle, il faut donc maintenir un budget
temporel très strict et éviter les images exceptionnellement lentes.

L'objectif n'est pas seulement d'obtenir une bonne moyenne, mais de produire
chaque image à temps afin que l'affichage reste cohérent avec les mouvements
de l'utilisateur.
