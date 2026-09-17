# Exercice 11 — Le monde à la mauvaise échelle

## Objectif

Dans cet exercice, j'ai étudié l'effet d'un changement d'échelle sur la
perception d'une salle et de son mobilier.

Le principe consiste à partir des dimensions normales d'une salle et à
multiplier toutes les dimensions par un même facteur.

Les dimensions de référence utilisées sont :

- salle : 6 m × 4 m × 2,80 m ;
- porte : 0,90 m × 2 m ;
- table : 1,20 m × 0,80 m × 0,80 m ;
- fenêtre : 1,50 m × 1,20 m.

J'ai utilisé trois facteurs différents : **0,5**, **1** et **2**.

Après avoir préparé le programme, j'ai fait l'expérience avec trois
participants : **Hendrix, Erwan et Thomas**.

## Code C++

```cpp
#include <iostream>
#include <iomanip>

int main()
{
    double facteur;

    std::cout << "Facteur d'echelle : ";
    std::cin >> facteur;

    double salleLongueur = 6.0;
    double salleLargeur = 4.0;
    double salleHauteur = 2.8;

    double porteLargeur = 0.9;
    double porteHauteur = 2.0;

    double tableLongueur = 1.2;
    double tableLargeur = 0.8;
    double tableHauteur = 0.8;

    double fenetreLargeur = 1.5;
    double fenetreHauteur = 1.2;

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Salle : "
              << salleLongueur * facteur << " x "
              << salleLargeur * facteur << " x "
              << salleHauteur * facteur << " m\n";

    std::cout << "Porte : "
              << porteLargeur * facteur << " x "
              << porteHauteur * facteur << " m\n";

    std::cout << "Table : "
              << tableLongueur * facteur << " x "
              << tableLargeur * facteur << " x "
              << tableHauteur * facteur << " m\n";

    std::cout << "Fenetre : "
              << fenetreLargeur * facteur << " x "
              << fenetreHauteur * facteur << " m\n";

    return 0;
}
```

## Les trois facteurs

J'ai utilisé les facteurs suivants :

- **0,5** : toutes les dimensions sont divisées par deux ;
- **1** : les dimensions restent normales ;
- **2** : toutes les dimensions sont multipliées par deux.

Pour la salle, le programme donne donc :

| Facteur | Dimensions de la salle |
|---:|---|
| 0,5 | 3,00 m × 2,00 m × 1,40 m |
| 1 | 6,00 m × 4,00 m × 2,80 m |
| 2 | 12,00 m × 8,00 m × 5,60 m |

## Conditions de l'expérience

J'ai repris l'expérience avec **trois personnes réelles : Hendrix, Erwan et
Thomas**.

Chacun a observé une version différente de la salle.

Je ne leur ai pas annoncé à l'avance le facteur qui avait été appliqué.

Le but était justement de ne pas influencer leur description.

Je leur ai demandé de regarder les dimensions générales de la salle et du
mobilier et de me dire simplement quelle impression l'environnement leur
donnait.

Les facteurs utilisés étaient :

| Participant | Facteur présenté |
|---|---:|
| Hendrix | 0,5 |
| Erwan | 1 |
| Thomas | 2 |

## Hendrix — facteur 0,5

Pour Hendrix, j'ai utilisé le facteur **0,5**.

La salle affichée correspondait donc à :

`3,00 m × 2,00 m × 1,40 m`

Hendrix a trouvé que l'environnement paraissait **trop petit**.

Son impression était surtout qu'il était **trop grand par rapport à la
pièce**.

C'est cette réaction qui m'a intéressé : au lieu de seulement constater que
les dimensions de la salle étaient réduites, il a aussi comparé
spontanément sa propre taille à celle de l'environnement.

## Erwan — facteur 1

Pour Erwan, j'ai utilisé le facteur **1**.

La salle gardait donc ses dimensions normales :

`6,00 m × 4,00 m × 2,80 m`

Erwan a trouvé l'environnement **normal**.

Il n'a pas signalé d'impression particulière d'être trop grand ou trop petit
par rapport à la salle.

Ce résultat sert de référence par rapport aux deux autres essais.

## Thomas — facteur 2

Pour Thomas, j'ai utilisé le facteur **2**.

La salle affichée correspondait donc à :

`12,00 m × 8,00 m × 5,60 m`

Thomas a trouvé l'environnement **très grand, presque immense**.

Par rapport à cet environnement agrandi, il avait plutôt l'impression
d'être **petit**.

Sa réaction était donc pratiquement l'inverse de celle observée avec Hendrix.

## Résultats des trois essais

Les observations recueillies peuvent être résumées ainsi :

| Participant | Facteur | Description recueillie |
|---|---:|---|
| Hendrix | 0,5 | L'environnement paraît trop petit ; impression d'être trop grand |
| Erwan | 1 | L'environnement paraît normal |
| Thomas | 2 | L'environnement paraît immense ; impression d'être petit |

On obtient donc :

```text
Facteur 0,5
→ environnement trop petit
→ impression d'être trop grand

Facteur 1
→ environnement normal
→ impression de taille normale

Facteur 2
→ environnement très grand
→ impression d'être petit
```

## Ce qui m'a marqué pendant le test

Ce qui m'a surtout intéressé est que les descriptions ne concernaient pas
uniquement les dimensions de la salle.

Avec le facteur 0,5, Hendrix a aussi décrit une impression d'être trop grand.

Avec le facteur 2, Thomas a au contraire décrit une impression d'être petit.

Erwan, avec le facteur 1, n'a pas remarqué ce type de problème.

Les changements de dimensions influencent donc la manière dont l'utilisateur
évalue sa propre taille par rapport à l'environnement.

## Analyse

Le programme ne déforme pas un objet particulier.

Toutes les dimensions sont multipliées par le même facteur.

Les proportions internes de la scène restent donc cohérentes.

Pourtant, les réactions montrent que l'impression produite n'est pas la même.

Avec une échelle réduite, l'utilisateur peut interpréter le résultat comme
s'il était devenu plus grand par rapport au monde.

À l'inverse, lorsque l'environnement est agrandi, il peut avoir l'impression
d'être devenu plus petit.

C'est donc la relation entre l'échelle du monde et celle de l'observateur qui
devient perceptivement incohérente.

## Pourquoi les trois personnes n'ont pas reçu le facteur à l'avance

Je n'ai pas annoncé les facteurs avant le test afin d'éviter de provoquer
une réponse attendue.

Dire à quelqu'un :

`la salle a été divisée par deux`

l'aurait déjà orienté vers l'idée d'une petite salle.

Je voulais plutôt recueillir son impression avant de révéler la valeur
utilisée.

Cela m'a permis de comparer leur perception avec le facteur réellement
appliqué.

## Ce que j'en tire

Les trois essais donnent une progression très claire :

```text
0,5 → Hendrix → monde trop petit → impression d'être trop grand

1   → Erwan   → monde normal → impression normale

2   → Thomas  → monde immense → impression d'être petit
```

Le résultat le plus intéressant est donc que le cerveau ne décrit pas
forcément uniquement le monde comme étant à la mauvaise échelle.

L'utilisateur peut également avoir l'impression que **sa propre taille a
changé par rapport au monde**.

Une scène peut donc être mathématiquement cohérente, avec tous ses objets
correctement proportionnés entre eux, tout en produisant une mauvaise
sensation d'échelle.

## Conclusion

J'ai utilisé un programme qui multiplie les dimensions d'une salle et de son
mobilier par un facteur d'échelle.

J'ai ensuite réalisé le test avec trois participants, sans leur annoncer à
l'avance le facteur présenté.

Les observations recueillies sont :

- **Hendrix, facteur 0,5** : environnement trop petit et impression d'être
  trop grand ;
- **Erwan, facteur 1** : environnement perçu comme normal ;
- **Thomas, facteur 2** : environnement immense et impression d'être petit.

Ces trois essais montrent que modifier l'échelle générale d'un environnement
change aussi la manière dont l'utilisateur perçoit sa propre taille par
rapport à ce monde.

En réalité virtuelle, il ne suffit donc pas que les objets soient
proportionnels entre eux. Leur échelle doit également rester cohérente avec
celle du corps de l'utilisateur.
