# Démo 4 — Le retard réglable

## Objectif

Cette démonstration consiste à faire varier le retard entre le mouvement
effectué avec la souris et la réaction visible à l'écran.

Le but est de déterminer à partir de quel moment le décalage devient
perceptible pour différents utilisateurs.

## Déroulement

Le programme est d'abord lancé avec un retard très faible.

La personne déplace ensuite la souris normalement pendant que le retard
est augmenté progressivement, par paliers.

Je ne lui annonce pas la valeur utilisée afin de ne pas influencer sa
réponse.

Je lui demande simplement de signaler le moment où elle commence à avoir
l'impression que l'affichage ne suit plus immédiatement son mouvement.

## Résultats de l'expérience

J'ai retenu pour chaque participant le premier retard qu'il signale comme
réellement perceptible.

| Participant | Premier retard perceptible |
|---|---:|
| Personne 1 | [à compléter] ms |
| Personne 2 | [à compléter] ms |
| Personne 3 | [à compléter] ms |

### Réactions recueillies

**Personne 1 :**

> [Noter ici sa réaction réelle.]

**Personne 2 :**

> [Noter ici sa réaction réelle.]

**Personne 3 :**

> [Noter ici sa réaction réelle.]

## Analyse

Les trois personnes ne détectent pas nécessairement le retard exactement
au même moment.

Cette expérience montre donc qu'un seuil de perception ne doit pas être
considéré comme une valeur identique pour tous les utilisateurs.

Elle montre également qu'un retard qui semble encore acceptable pour une
personne peut déjà être gênant pour une autre.

## Rapport avec le budget d'une image

En réalité virtuelle, les durées disponibles sont particulièrement faibles.

À 90 Hz, une image revient toutes les :

1000 / 90 = **11,1 ms**

À 120 Hz :

1000 / 120 = **8,3 ms**

On comprend alors qu'un retard de plusieurs dizaines de millisecondes peut
représenter plusieurs périodes d'affichage.

Par exemple, un retard de 30 ms correspond à environ :

30 / 11,1 = **2,7 images à 90 Hz**

Le retard total ressenti ne dépend cependant pas uniquement du programme :
les capteurs, la transmission, le rendu, la composition et l'affichage
participent également à la latence globale.

## Conclusion

Cette démonstration rend la notion de latence beaucoup plus concrète.

Le problème n'est pas seulement de savoir si un programme finit par produire
la bonne image. Il faut qu'il la produise suffisamment rapidement pour que
la réponse visuelle reste cohérente avec le mouvement de l'utilisateur.

Les différences observées entre les participants montrent aussi pourquoi
il est utile de tester une application avec plusieurs personnes plutôt que
de se fier uniquement à la perception du développeur.
