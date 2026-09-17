# Démo 4 — Trois échelles, trois réactions

## Objectif

Cette démonstration consiste à présenter une même pièce virtuelle avec
différentes échelles et à observer la manière dont plusieurs personnes
perçoivent les dimensions de l'environnement.

L'objectif est surtout de voir si une mauvaise échelle peut être ressentie
par l'utilisateur, même lorsqu'aucun message d'erreur n'apparaît dans le
programme.

## Déroulement

J'ai fait observer la pièce à trois personnes : Hendrix, Erwan et Thomas.

Je ne leur ai pas annoncé le facteur d'échelle utilisé afin de ne pas
influencer leurs réponses.

Je leur ai simplement demandé :

**« En regardant cette pièce, quelle impression as-tu par rapport à sa
taille ? Est-ce que quelque chose te paraît normal, trop grand ou trop
petit ? »**

J'ai ensuite comparé leurs impressions.

## Résultats

| Participant | Échelle présentée | Impression principale |
|---|---:|---|
| Hendrix | 0,5 | Environnement trop petit, impression d'être trop grand |
| Erwan | 1 | Dimensions normales |
| Thomas | 2 | Environnement immense, impression d'être petit |

### Hendrix — facteur 0,5

Avec la première échelle, Hendrix a rapidement trouvé que les dimensions
de la pièce étaient étranges.

Il a notamment remarqué que les différents éléments de l'environnement
semblaient trop petits par rapport à lui.

Sa réaction peut se résumer ainsi :

> « Tout paraît petit autour de moi. J'ai l'impression d'être beaucoup
> trop grand pour cette pièce. »

La porte et la table lui ont également semblé plus petites que ce qu'il
attendait normalement.

### Erwan — facteur 1

La réaction d'Erwan a été beaucoup plus simple.

Il n'a pas immédiatement trouvé quelque chose d'anormal dans les
proportions de la pièce.

Il a expliqué :

> « Pour moi, ça paraît normal. Je ne vois pas vraiment quelque chose
> de bizarre dans les dimensions. »

Dans ce cas, la taille de la pièce et celle des objets lui semblaient
cohérentes avec sa propre taille.

### Thomas — facteur 2

Thomas a eu l'impression inverse de celle d'Hendrix.

Il a trouvé l'environnement beaucoup trop grand par rapport à lui et a
particulièrement remarqué les dimensions générales de la pièce.

Il a décrit son impression ainsi :

> « Là, tout me paraît énorme. La pièce est vraiment grande et j'ai
> l'impression d'être beaucoup plus petit à l'intérieur. »

La taille des objets renforçait donc chez lui cette impression d'être
petit par rapport à l'environnement.

## Comparaison

Les trois observations sont assez différentes.

Avec le facteur **0,5**, Hendrix a eu l'impression d'être trop grand dans
un environnement trop petit.

Avec le facteur **1**, Erwan n'a pas remarqué de problème particulier et
a considéré les proportions comme normales.

Avec le facteur **2**, Thomas a au contraire eu l'impression d'être petit
dans un environnement beaucoup trop grand.

On obtient donc :

**Échelle réduite → impression d'être grand**

**Échelle normale → proportions naturelles**

**Échelle agrandie → impression d'être petit**

## Ce que j'en retiens

Cette démonstration m'a permis de comprendre qu'une erreur d'échelle ne se
présente pas nécessairement comme une erreur visible dans le programme.

La scène peut être correctement affichée et tous les objets peuvent rester
à leur place, alors que l'utilisateur ressent immédiatement que quelque
chose n'est pas normal.

Le problème est donc avant tout perceptif.

Elle montre également pourquoi le développeur peut être mal placé pour
juger l'échelle de son propre environnement.

Lorsqu'on construit soi-même une scène, on connaît déjà les dimensions
utilisées et on finit par s'habituer à ce que l'on voit.

Une personne qui découvre la scène pour la première fois réagit plus
spontanément aux proportions qui lui semblent étranges.

## Conclusion

Les réactions de Hendrix, Erwan et Thomas montrent concrètement l'effet
d'un changement d'échelle.

La même pièce peut donner l'impression d'être minuscule, normale ou immense
selon le facteur appliqué.

En réalité virtuelle, travailler en mètres et conserver une échelle
cohérente est donc important pour que la taille du monde virtuel corresponde
aux attentes perceptives de l'utilisateur.
