# Exercice 7 — Le coût du doublement

Dans cet exercice, j'ai repris le même principe de programme que dans
l'exercice précédent afin de mesurer uniquement le temps consacré au rendu,
sans prendre en compte la logique générale du programme.

L'objectif est d'abord de mesurer le coût d'un rendu, puis d'estimer ce que
coûterait ce rendu s'il devait être effectué deux fois, par exemple une fois
pour chaque œil en réalité virtuelle.

## Programme utilisé

J'ai effectué 1 000 mesures du travail représentant le rendu seul.

Le programme calcule :

- le temps moyen du rendu ;
- le temps maximum observé ;
- une estimation du coût de deux rendus ;
- le temps qui resterait dans un budget de 11,1 ms à 90 Hz.

```cpp
#include <iostream>
#include <chrono>
#include <iomanip>

int main()
{
    using Clock = std::chrono::high_resolution_clock;

    const int nombreMesures = 1000;

    double total = 0.0;
    double maximum = 0.0;

    volatile double travailRendu = 0.0;

    for (int image = 0; image < nombreMesures; ++image)
    {
        auto debut = Clock::now();

        // Travail représentant uniquement le rendu
        for (int i = 0; i < 100000; ++i)
        {
            travailRendu += i * 0.000001;
        }

        auto fin = Clock::now();

        double duree =
            std::chrono::duration<double, std::milli>(
                fin - debut
            ).count();

        total += duree;

        if (duree > maximum)
            maximum = duree;
    }

    double moyenne = total / nombreMesures;

    std::cout << std::fixed << std::setprecision(3);

    std::cout << "Nombre de mesures : "
              << nombreMesures << '\n';

    std::cout << "Temps moyen du rendu seul : "
              << moyenne << " ms\n";

    std::cout << "Temps maximum mesure : "
              << maximum << " ms\n";

    double deuxYeux = moyenne * 2.0;

    std::cout << "Estimation pour deux rendus : "
              << deuxYeux << " ms\n";

    std::cout << "Budget restant a 90 Hz : "
              << 11.1 - deuxYeux << " ms\n";

    return 0;
}
```

## Résultats obtenus

J'ai compilé et exécuté le programme sur ma machine.

La sortie obtenue est :

```text
Nombre de mesures : 1000
Temps moyen du rendu seul : 0.263 ms
Temps maximum mesure : 0.750 ms
Estimation pour deux rendus : 0.526 ms
Budget restant a 90 Hz : 10.574 ms
```

J'ai donc réellement mesuré un temps moyen de rendu de :

**0,263 ms**

Le temps maximum observé pendant les 1 000 mesures a été de :

**0,750 ms**

## Estimation pour deux rendus

Si je fais l'hypothèse simple que le coût du rendu est exactement doublé
lorsqu'il est effectué deux fois, j'obtiens :

`0,263 × 2 = 0,526 ms`

L'estimation du coût de deux rendus est donc :

**0,526 ms**

Pour un affichage à 90 Hz, je prends comme référence un budget d'environ
**11,1 ms par image**.

Le temps restant serait alors :

`11,1 - 0,526 = 10,574 ms`

Il resterait donc environ :

**10,574 ms**

pour les autres traitements.

## Comparaison avec ma première estimation

Dans ma première version, j'avais utilisé un temps de rendu de **4,2 ms**.

J'avais donc calculé :

`4,2 × 2 = 8,4 ms`

puis :

`11,1 - 8,4 = 2,7 ms`

Mais les **4,2 ms** n'étaient pas issus d'une mesure effectuée avec mon
programme.

Après avoir réalisé réellement la mesure sur 1 000 passages, j'ai obtenu un
temps moyen de **0,263 ms**.

Le résultat réel de mon expérience est donc très différent de ma première
estimation.

Cela montre pourquoi il faut commencer par mesurer le coût réel du rendu avant
de tirer une conclusion sur le budget disponible.

## Le coût est-il vraiment multiplié exactement par deux ?

Le calcul :

`0,263 × 2 = 0,526 ms`

est une **estimation simplifiée**.

Il suppose que rendre la scène pour deux yeux coûte exactement deux fois le
rendu d'une seule vue.

Cette hypothèse est utile pour obtenir un premier ordre de grandeur, mais ce
n'est pas une loi.

En réalité virtuelle, certaines opérations doivent effectivement être
effectuées pour chaque œil. Les deux yeux occupent des positions légèrement
différentes et doivent recevoir deux vues différentes de la scène.

Une partie du travail de rendu doit donc être répétée.

Cependant, tout le travail réalisé pendant une image n'est pas forcément
doublé.

Par exemple, certaines opérations peuvent être communes aux deux yeux :

- la logique générale du programme ;
- la mise à jour de l'état de la scène ;
- certains calculs de physique ;
- le chargement et la gestion des ressources ;
- certaines préparations des données avant le rendu.

Le facteur 2 représente donc ici une hypothèse volontairement simple pour
estimer le coût du rendu stéréoscopique.

Le coût réel dépendrait de la manière dont le moteur de rendu organise et
partage le travail entre les deux vues.

## Interprétation

Avec le programme très simple que j'ai mesuré, le rendu seul coûte en moyenne :

**0,263 ms**

Avec l'hypothèse d'un doublement parfait, les deux rendus coûteraient :

**0,526 ms**

Il resterait donc :

**10,574 ms**

sur un budget de 11,1 ms.

Dans mon expérience, le doublement du rendu ne suffit donc pas à mettre le
programme en difficulté, car le rendu de départ est extrêmement léger.

Cependant, le raisonnement serait très différent avec un rendu plus coûteux.

Par exemple, si le rendu d'un seul œil prenait déjà une grande partie du budget,
le fait de devoir produire une seconde vue pourrait faire dépasser le temps
disponible.

C'est pourquoi le coût du rendu doit être mesuré avant d'estimer l'effet du
doublement.

## Que faudrait-il réduire ?

Si le rendu devenait trop coûteux après la production des deux vues, c'est
principalement le **coût du rendu graphique** qu'il faudrait réduire.

On pourrait notamment :

- réduire la complexité des objets affichés ;
- diminuer certains effets graphiques coûteux ;
- éviter les opérations de rendu inutiles ;
- limiter le travail qui doit réellement être répété pour chaque œil ;
- partager entre les deux yeux les traitements qui peuvent l'être.

L'objectif serait de réduire la partie du travail qui augmente réellement
lorsqu'une seconde vue est produite, plutôt que de réduire inutilement des
traitements qui peuvent rester communs aux deux yeux.

## Conclusion

Après avoir réellement mesuré le rendu sur 1 000 passages, j'ai obtenu :

```text
Temps moyen du rendu seul : 0,263 ms
Temps maximum mesuré : 0,750 ms
```

En faisant l'hypothèse simplifiée que deux rendus coûtent exactement deux fois
plus cher qu'un seul, j'obtiens :

```text
Deux rendus : 0,526 ms
Budget restant à 90 Hz : 10,574 ms
```

Dans le cas de mon programme, le rendu est suffisamment léger pour que cette
estimation reste largement dans le budget de 11,1 ms.

L'expérience m'a surtout montré que la mesure doit venir avant l'estimation :
le coût de départ détermine complètement la conclusion.

Enfin, le facteur 2 ne doit pas être considéré comme une loi. Les deux yeux
nécessitent deux vues différentes, mais toutes les opérations d'une image ne
sont pas forcément répétées deux fois.

Si le coût devenait trop important, il faudrait donc optimiser en priorité le
travail graphique réellement effectué séparément pour chaque œil.
