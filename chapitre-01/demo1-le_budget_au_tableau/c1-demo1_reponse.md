# Démonstration 1 — Le budget au tableau

## Objectif

Cette démonstration permet de visualiser comment les vingt millisecondes
disponibles entre le mouvement de la tête et l'affichage de l'image sont
réparties entre les différentes étapes du système.

Cette fois, je n'ai pas commencé par montrer directement la bonne répartition.

J'ai présenté une barre vide correspondant à **20 ms** à trois personnes :
Erwan, Hendrix et Thomas.

Je leur ai donné les cinq étapes :

- Capteurs ;
- Transmission ;
- Application et rendu ;
- Composition ;
- Affichage.

Je leur ai ensuite demandé de répartir eux-mêmes les 20 ms entre ces cinq
étapes.

Après leurs propositions, j'ai présenté la répartition de référence et j'ai
noté leurs réactions.

## Barre de départ

Au début de la démonstration, la barre était vide :

```text
0 ms                                              20 ms
|---------------------------------------------------|
|                                                   |
|---------------------------------------------------|
```

Les cinq étapes devaient être placées dans cette barre en respectant un total
de 20 ms.

## Proposition d'Erwan

Erwan a proposé la répartition suivante :

| Étape | Proposition d'Erwan |
|---|---:|
| Capteurs | 2 ms |
| Transmission | 2 ms |
| Application et rendu | 10 ms |
| Composition | 2 ms |
| Affichage | 4 ms |
| **Total** | **20 ms** |

Sa proposition donnait donc :

```text
Capteurs             : 2 ms
Transmission         : 2 ms
Application et rendu : 10 ms
Composition          : 2 ms
Affichage            : 4 ms
Total                : 20 ms
```

Erwan est arrivé à une répartition qui correspond à celle que j'ai ensuite
utilisée comme répartition de référence.

Après la révélation du budget, sa réaction a été :

> « Je pensais que 20 ms laissaient beaucoup de temps, mais chaque étape prend déjà une partie du budget. »

## Proposition de Hendrix

Hendrix a proposé :

| Étape | Proposition de Hendrix |
|---|---:|
| Capteurs | 1 ms |
| Transmission | 3 ms |
| Application et rendu | 10 ms |
| Composition | 2 ms |
| Affichage | 4 ms |
| **Total** | **20 ms** |

Sa répartition était donc :

```text
Capteurs             : 1 ms
Transmission         : 3 ms
Application et rendu : 10 ms
Composition          : 2 ms
Affichage            : 4 ms
Total                : 20 ms
```

Il a accordé un peu moins de temps aux capteurs et un peu plus à la
transmission.

Après avoir comparé sa proposition à la répartition de référence, Hendrix a
dit :

> « Donc le programme doit vraiment être rapide, parce que les autres étapes prennent déjà plusieurs millisecondes. »

## Proposition de Thomas

Thomas a proposé la répartition suivante :

| Étape | Proposition de Thomas |
|---|---:|
| Capteurs | 2 ms |
| Transmission | 2 ms |
| Application et rendu | 9 ms |
| Composition | 2 ms |
| Affichage | 5 ms |
| **Total** | **20 ms** |

Ce qui donne :

```text
Capteurs             : 2 ms
Transmission         : 2 ms
Application et rendu : 9 ms
Composition          : 2 ms
Affichage            : 5 ms
Total                : 20 ms
```

Thomas a donc laissé un peu moins de temps à l'application et au rendu et un
peu plus de temps à l'affichage.

Après la révélation, sa réaction a été :

> « Je comprends mieux pourquoi quelques millisecondes de retard peuvent suffire à dépasser le budget. »

## Comparaison des trois propositions

Les trois répartitions obtenues peuvent être comparées directement :

| Étape | Erwan | Hendrix | Thomas | Référence |
|---|---:|---:|---:|---:|
| Capteurs | 2 ms | 1 ms | 2 ms | **2 ms** |
| Transmission | 2 ms | 3 ms | 2 ms | **2 ms** |
| Application et rendu | 10 ms | 10 ms | 9 ms | **10 ms** |
| Composition | 2 ms | 2 ms | 2 ms | **2 ms** |
| Affichage | 4 ms | 4 ms | 5 ms | **4 ms** |
| **Total** | **20 ms** | **20 ms** | **20 ms** | **20 ms** |

Les trois personnes ont bien respecté le budget total de 20 ms, mais elles
n'ont pas toutes réparti ce temps de la même manière.

Erwan est arrivé à la même répartition que la référence.

Hendrix a déplacé 1 ms des capteurs vers la transmission.

Thomas a déplacé 1 ms de l'application et du rendu vers l'affichage.

Cette différence était intéressante parce qu'elle montre qu'avant de voir
la répartition, chacun ne donne pas nécessairement le même poids aux
différentes étapes.

## Répartition révélée

Après leurs propositions, j'ai présenté la répartition suivante :

| Étape | Durée retenue |
|---|---:|
| Capteurs | 2 ms |
| Transmission | 2 ms |
| Application et rendu | 10 ms |
| Composition | 2 ms |
| Affichage | 4 ms |
| **Total** | **20 ms** |

La barre complète devient :

```text
0 ms                                              20 ms
|---------------------------------------------------|
|Capteurs|Trans.|      Application / rendu      |Comp.|Affichage|
|  2 ms  | 2 ms |             10 ms             |2 ms |  4 ms   |
```

La partie la plus importante est l'application et le rendu, avec 10 ms.

Mais les autres étapes utilisent déjà ensemble :

`2 + 2 + 2 + 4 = 10 ms`

Il ne faut donc pas considérer les 20 ms comme du temps entièrement disponible
pour le code de l'application.

## Ce qu'il reste réellement pour le code

La démonstration m'a surtout permis de faire remarquer que le budget total
est partagé.

Les capteurs, la transmission, la composition et l'affichage consomment déjà
une partie des vingt millisecondes.

Dans la répartition utilisée ici, l'application et le rendu disposent
d'environ :

**10 ms**

Cela signifie que le programme doit effectuer son travail dans une fenêtre
de temps beaucoup plus petite que les 20 ms totales.

Une application qui utiliserait presque tout le budget de 20 ms pour son
propre calcul arriverait déjà trop tard, puisque les autres étapes ont
également besoin de temps.

## Réactions après la révélation

Les réactions des trois participants résument bien ce que la démonstration
leur a fait remarquer.

**Erwan :**

> « Je pensais que 20 ms laissaient beaucoup de temps, mais chaque étape prend déjà une partie du budget. »

**Hendrix :**

> « Donc le programme doit vraiment être rapide, parce que les autres étapes prennent déjà plusieurs millisecondes. »

**Thomas :**

> « Je comprends mieux pourquoi quelques millisecondes de retard peuvent suffire à dépasser le budget. »

Les trois réactions portent donc sur le même point : **20 ms paraît être une
durée disponible assez importante lorsqu'on la considère seule, mais elle
devient beaucoup plus contraignante lorsqu'on la découpe entre toutes les
étapes du système.**

## Ce que j'en retiens

Avant de présenter directement la bonne répartition, le fait de demander à
plusieurs personnes de remplir la barre a permis de voir que le budget n'est
pas réparti de manière évidente.

Même lorsque les propositions sont proches, déplacer seulement 1 ms d'une
étape vers une autre représente déjà une partie importante d'un budget total
de seulement 20 ms.

La démonstration m'a également permis de comprendre que le code ne possède
pas les vingt millisecondes à lui seul.

Il partage ce temps avec les capteurs, la transmission, la composition et
l'affichage.

## Conclusion

Cette démonstration a été réalisée avec Erwan, Hendrix et Thomas à partir
d'une barre vide de **20 ms**.

Chacun a proposé sa propre répartition des cinq étapes avant que je présente
la répartition de référence.

Les propositions étaient proches, mais pas identiques :

- Erwan a retrouvé la répartition de référence ;
- Hendrix a donné davantage de temps à la transmission et moins aux capteurs ;
- Thomas a donné davantage de temps à l'affichage et moins à l'application
  et au rendu.

Après la révélation, leurs réactions ont surtout porté sur le peu de temps
réellement disponible pour le programme.

Je retiens donc que le budget de 20 ms ne correspond pas au temps disponible
uniquement pour le code. Il doit être partagé entre toute la chaîne allant
du mouvement de la tête jusqu'à l'affichage de l'image.

C'est cette répartition qui rend le budget beaucoup plus contraignant qu'il
ne paraît au premier regard.
