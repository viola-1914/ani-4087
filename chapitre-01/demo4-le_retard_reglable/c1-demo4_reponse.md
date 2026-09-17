## Résultats de l'expérience

J'ai fait essayer le programme à trois personnes : Hendrix, Erwan et Thomas.
Le retard a été augmenté progressivement sans leur annoncer la valeur
appliquée.

J'ai retenu le moment où chacun a commencé à signaler spontanément un
décalage entre son mouvement et la réponse visible à l'écran.

| Participant | Premier retard perceptible |
|---|---:|
| Hendrix | 25 ms |
| Erwan | 40 ms |
| Thomas | 55 ms |

### Réactions recueillies

**Hendrix — 25 ms**

Hendrix a été le premier à remarquer le changement. Il m'a expliqué en
substance :

> « Je commence déjà à sentir un petit décalage. Quand je bouge rapidement
> la souris, le curseur ne suit plus exactement ma main. »

Il a donc détecté assez rapidement que la réponse à l'écran n'était plus
tout à fait immédiate.

**Erwan — 40 ms**

Au début, Erwan ne signalait rien de particulier. Lorsque le retard est
devenu plus important, sa réaction a été proche de celle-ci :

> « Au début je ne voyais pas vraiment de différence, mais maintenant oui.
> J'ai l'impression que le curseur arrive légèrement après mon mouvement. »

Chez lui, le décalage est donc devenu perceptible plus tard que chez Hendrix.

**Thomas — 55 ms**

Thomas a toléré davantage de retard avant de le signaler. Il a surtout
remarqué le phénomène pendant les mouvements rapides :

> « Sur les petits mouvements, je ne remarque presque rien. Par contre,
> quand je vais vite de gauche à droite, là je vois clairement que ça traîne
> derrière. »

Son observation montre que la vitesse du mouvement peut également rendre
le retard beaucoup plus évident.

## Comparaison des résultats

Les trois personnes n'ont donc pas signalé le retard exactement au même
moment :

- Hendrix : 25 ms ;
- Erwan : 40 ms ;
- Thomas : 55 ms.

La moyenne des trois seuils est :

(25 + 40 + 55) / 3 = **40 ms**

L'écart entre le seuil le plus faible et le plus élevé est de :

55 - 25 = **30 ms**

Ce résultat m'a surtout montré que la perception du retard varie d'une
personne à l'autre. Hendrix l'a remarqué assez rapidement, alors que Thomas
avait besoin d'un retard plus important avant de le trouver évident.

## Rapport avec le budget d'une image

À 90 Hz, une nouvelle image doit être produite environ toutes les :

1000 / 90 = **11,1 ms**

Les seuils relevés représentent donc approximativement :

- 25 ms / 11,1 ms = **2,3 images** ;
- 40 ms / 11,1 ms = **3,6 images** ;
- 55 ms / 11,1 ms = **5 images**.

Cela m'a permis de mieux comprendre pourquoi quelques dizaines de
millisecondes constituent déjà une durée importante dans une chaîne
d'affichage VR.

Le retard total ne vient d'ailleurs pas uniquement du programme. Les
capteurs, la transmission, le rendu, la composition et l'affichage
participent également à la latence globale.

## Conclusion

Cette expérience m'a permis de constater concrètement que le retard n'est
pas perçu exactement de la même façon par tous les utilisateurs.

Hendrix a signalé le décalage le plus tôt, Erwan à un niveau intermédiaire
et Thomas surtout lorsque le mouvement devenait rapide et que le retard
était plus important.

Cela explique pourquoi une application VR ne devrait pas être évaluée
uniquement par son développeur. Plusieurs utilisateurs peuvent avoir des
seuils de perception différents.

La démonstration rend finalement beaucoup plus concrète la contrainte des
quelques millisecondes disponibles pour produire une image en réalité
virtuelle.
