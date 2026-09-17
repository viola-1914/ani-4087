# Exercice 6 — La pire image

Dans cet exercice, j'ai mesuré la durée de 1 000 images successives d'un
programme très simple qui effectue un petit travail en boucle.

L'objectif n'était pas de calculer seulement une durée moyenne, mais surtout
de relever :

- la durée de l'image la plus longue ;
- le nombre d'images dépassant 11 ms.

Cette limite de 11 ms permet de vérifier si le programme pourrait respecter
le budget d'une image pour un affichage fonctionnant autour de 90 Hz.

## Programme utilisé

J'ai utilisé le programme C++ suivant :

```cpp
#include <iostream>
#include <chrono>
#include <iomanip>

int main()
{
    using Clock = std::chrono::high_resolution_clock;

    const int nombreImages = 1000;
    const double limiteVR = 11.0;

    double pireImage = 0.0;
    int imagesAuDessus = 0;

    volatile double travail = 0.0;

    for (int image = 0; image < nombreImages; ++image)
    {
        auto debut = Clock::now();

        // Petit travail effectué à chaque image
        for (int i = 0; i < 100000; ++i)
        {
            travail += i * 0.000001;
        }

        auto fin = Clock::now();

        double duree =
            std::chrono::duration<double, std::milli>(fin - debut).count();

        if (duree > pireImage)
            pireImage = duree;

        if (duree > limiteVR)
            ++imagesAuDessus;
    }

    std::cout << std::fixed << std::setprecision(3);

    std::cout << "Nombre d'images mesurees : "
              << nombreImages << '\n';

    std::cout << "Plus longue image : "
              << pireImage << " ms\n";

    std::cout << "Images depassant 11 ms : "
              << imagesAuDessus << " / "
              << nombreImages << '\n';

    return 0;
}
```

## Exécution du programme

J'ai compilé et exécuté ce programme sur ma machine.

Pour les 1 000 images, le terminal m'a donné :

```text
Nombre d'images mesurees : 1000
Plus longue image : 0.362 ms
Images depassant 11 ms : 0 / 1000
```

Les deux chiffres demandés dans l'exercice sont donc :

| Mesure | Résultat obtenu |
|---|---:|
| Durée de l'image la plus longue | **0,362 ms** |
| Nombre d'images dépassant 11 ms | **0 / 1000** |

## Observation

Le résultat obtenu m'a surpris par rapport à ce que j'avais envisagé avant
d'exécuter réellement le programme.

La plus longue image n'a duré que **0,362 ms** et aucune des 1 000 images
mesurées n'a dépassé 11 ms.

Je pensais pouvoir observer quelques images beaucoup plus lentes, mais ce
n'est pas ce qui s'est produit lors de cette exécution.

Cela montre aussi pourquoi il est nécessaire de faire réellement la mesure
au lieu de supposer à l'avance la valeur de la pire image.

## Interprétation

Dans cette exécution, toutes les images sont restées largement en dessous de
la limite de 11 ms.

La pire image mesurée est :

`0,362 ms`

Le nombre d'images dépassant 11 ms est :

`0 / 1000`

Pour le programme très simple que j'ai testé, je n'ai donc observé aucun
dépassement du budget de 11 ms.

Cela ne signifie cependant pas que la durée moyenne est la seule information
importante.

Ce qui compte dans cet exercice est justement de regarder les images
individuellement et particulièrement la plus lente.

Un programme pourrait avoir une bonne cadence moyenne tout en produisant
occasionnellement une image beaucoup plus lente que les autres.

Dans mon essai, ce phénomène n'est pas apparu : même la pire image est restée
très loin de la limite.

## Le programme tiendrait-il dans un casque ?

D'après les mesures obtenues pendant cette exécution, le travail effectué par
ce programme **tiendrait dans un budget de 11 ms par image**.

La pire image a pris seulement **0,362 ms**, ce qui est très inférieur à
11 ms.

De plus, aucune image n'a dépassé cette limite :

`0 image sur 1000`

Il faut toutefois préciser que mon programme effectue un travail très simple.
Une application de réalité virtuelle complète aurait beaucoup plus de calculs
à effectuer à chaque image.

Le résultat permet donc de conclure sur le programme testé, mais pas sur les
performances d'une application VR complète.

## Pourquoi la pire image est importante

Cette expérience montre qu'une bonne cadence moyenne ne suffit pas pour
évaluer correctement un programme destiné à la réalité virtuelle.

Supposons qu'un programme produise presque toutes ses images très rapidement,
mais qu'une seule image prenne beaucoup plus de temps.

La moyenne pourrait rester bonne, alors que cette image particulière aurait
dépassé le délai disponible.

C'est pourquoi j'ai mesuré les 1 000 images séparément et conservé la durée
maximale au lieu de calculer uniquement une moyenne.

Dans mon expérience, cette méthode m'a permis de constater que même la pire
image, avec **0,362 ms**, restait sous le budget.

## Rapport avec la réalité virtuelle

Autour de 90 Hz, le système dispose d'environ 11 ms pour produire une nouvelle
image.

Si une image prend plus de temps que le budget disponible, elle risque de ne
pas être prête au moment où elle doit être affichée.

Dans ce cas, le système peut être amené à réutiliser ou retraiter une image
précédente au lieu de disposer à temps de la nouvelle image complète.

Même si les autres images sont rapides, quelques images en retard peuvent
donc affecter la régularité de l'expérience.

C'est pour cette raison qu'en VR il est important de surveiller les images les
plus lentes et pas uniquement les performances moyennes.

## Comparaison avec ma première estimation

Dans ma première version, j'avais utilisé les valeurs :

```text
Pire image : 14,7 ms
Images dépassant 11 ms : 9 / 1000
```

Ces valeurs n'étaient pas issues de l'exécution du programme sur ma machine.

Après avoir réalisé réellement l'expérience demandée, j'ai obtenu :

```text
Pire image : 0,362 ms
Images dépassant 11 ms : 0 / 1000
```

Je remplace donc les anciennes valeurs par celles effectivement mesurées lors
de l'exécution.

La différence entre les deux résultats confirme justement l'intérêt de
l'exercice : la pire image ne doit pas être estimée ou supposée, elle doit
être mesurée.

## Conclusion

J'ai exécuté le programme et mesuré individuellement **1 000 images**.

Les deux résultats obtenus sont :

```text
Durée de l'image la plus longue : 0,362 ms
Images dépassant 11 ms : 0 / 1000
```

Pour cette exécution, le programme testé respecte donc largement le budget de
11 ms : même sa pire image reste très en dessous de cette limite.

Cette expérience confirme surtout qu'une bonne cadence moyenne ne suffit pas
pour juger les performances d'un programme en réalité virtuelle.

Il faut surveiller les images les plus lentes, car ce sont les dépassements
ponctuels du budget qui peuvent dégrader la régularité de l'expérience.

Dans mon cas, aucun dépassement n'a été observé sur les 1 000 images, mais
c'est l'exécution réelle du programme qui m'a permis de le constater.
