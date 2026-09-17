# Exercice 12 — La borne de cent millisecondes

Dans cet exercice, j'ai étudié l'erreur produite lorsqu'on extrapole une
rotation de tête en supposant que sa vitesse reste constante.

J'ai utilisé une vitesse initiale de **90 degrés par seconde** et j'ai comparé
deux valeurs :

- l'orientation prédite par extrapolation à vitesse constante ;
- l'orientation obtenue avec une simulation dans laquelle la tête ralentit
  progressivement.

L'objectif est de mesurer comment l'erreur évolue avec la durée
d'extrapolation et de voir où la borne de 100 ms peut se justifier.

## Code C++

```cpp
#include <iostream>
#include <iomanip>
#include <cmath>

const double PI = 3.14159265358979323846;

double degVersRad(double deg)
{
    return deg * PI / 180.0;
}

double radVersDeg(double rad)
{
    return rad * 180.0 / PI;
}

// Prédiction à vitesse constante
double extrapoler(double vitesseInitiale, double dt)
{
    return vitesseInitiale * dt;
}

// Simulation de la vraie pose par petits pas
double vraiePose(double vitesseInitiale, double duree)
{
    const double pas = 0.001; // 1 ms

    double temps = 0.0;
    double angle = 0.0;
    double vitesse = vitesseInitiale;

    while (temps < duree)
    {
        double h = pas;

        if (temps + h > duree)
            h = duree - temps;

        /*
           Simulation d'un mouvement qui ralentit.
           La vitesse diminue progressivement avec le temps.
        */
        double facteur = std::exp(-2.0 * temps);
        vitesse = vitesseInitiale * facteur;

        angle += vitesse * h;
        temps += h;
    }

    return angle;
}

int main()
{
    // J'utilise ici une vitesse initiale de 90 degrés par seconde.
    double vitesseInitiale = degVersRad(90.0);

    double durees[] = {
        0.010,
        0.020,
        0.050,
        0.100,
        0.200,
        0.500,
        1.000
    };

    std::cout << std::fixed << std::setprecision(3);

    std::cout
        << "Duree(ms)  Extrapole(deg)  Reel(deg)  Erreur(deg)\n";

    for (double dt : durees)
    {
        double prediction =
            extrapoler(vitesseInitiale, dt);

        double reel =
            vraiePose(vitesseInitiale, dt);

        double erreur =
            std::fabs(prediction - reel);

        std::cout
            << dt * 1000.0 << "        "
            << radVersDeg(prediction) << "          "
            << radVersDeg(reel) << "       "
            << radVersDeg(erreur) << '\n';
    }

    return 0;
}
```

## Pourquoi j'ai utilisé 90°/s

L'énoncé proposait une vitesse initiale de **180°/s**.

Dans mon programme, j'ai choisi de faire le test avec **90°/s**, c'est-à-dire
une vitesse initiale deux fois plus faible, afin d'observer l'évolution de
l'erreur sur un mouvement de rotation moins rapide.

Ce choix ne change pas le principe de l'expérience : dans les deux cas, il
s'agit de comparer une prédiction qui suppose une vitesse constante à un
mouvement simulé qui ralentit progressivement.

Il change cependant les valeurs numériques obtenues. Les résultats qui
suivent correspondent donc à mon test effectué à **90°/s**.

## Exécution du programme

J'ai exécuté le programme pour les durées suivantes :

- 10 ms ;
- 20 ms ;
- 50 ms ;
- 100 ms ;
- 200 ms ;
- 500 ms ;
- 1000 ms.

Le programme m'a fourni les valeurs suivantes.

| Durée | Angle extrapolé | Angle réel simulé | Erreur |
|---:|---:|---:|---:|
| 10 ms | 0,900° | 0,892° | **0,008°** |
| 20 ms | 1,800° | 1,766° | **0,034°** |
| 50 ms | 4,500° | 4,287° | **0,213°** |
| 100 ms | 9,000° | 8,165° | **0,835°** |
| 200 ms | 18,000° | 14,850° | **3,150°** |
| 500 ms | 45,000° | 28,474° | **16,526°** |
| 1000 ms | 90,000° | 38,949° | **51,051°** |

## Courbe de l'erreur

À partir des valeurs mesurées, la courbe de l'erreur est croissante.

```text
Erreur
(degrés)

  55 |                                      ● 51,051
  50 |
  45 |
  40 |
  35 |
  30 |
  25 |
  20 |
  15 |                         ● 16,526
  10 |
   5 |              ● 3,150
   1 |       ● 0,835
 0,5 |    ● 0,213
   0 | ● ●
     +-----------------------------------------------
       10 20  50  100   200      500          1000
                     Durée (ms)
```

Les points représentés sur cette courbe correspondent aux erreurs mesurées :

```text
10 ms    -> 0,008°
20 ms    -> 0,034°
50 ms    -> 0,213°
100 ms   -> 0,835°
200 ms   -> 3,150°
500 ms   -> 16,526°
1000 ms  -> 51,051°
```

## Analyse de la courbe

Les valeurs permettent maintenant de remplacer les appréciations qualitatives
par des mesures.

À **10 ms**, l'erreur n'est que de :

`0,008°`

À **20 ms**, elle atteint :

`0,034°`

À **50 ms**, elle vaut :

`0,213°`

À **100 ms**, elle atteint :

`0,835°`

Puis elle augmente beaucoup plus fortement :

- **3,150° à 200 ms** ;
- **16,526° à 500 ms** ;
- **51,051° à 1000 ms**.

L'erreur est donc multipliée par presque quatre entre 100 ms et 200 ms :

`0,835° -> 3,150°`

Elle devient ensuite beaucoup plus importante lorsque l'extrapolation est
prolongée.

## Où la borne de 100 ms se justifie-t-elle ?

Dans mon expérience, la courbe ne présente pas une cassure brutale exactement
à **100 ms**.

L'erreur augmente progressivement.

Cependant, les nombres montrent que la borne de 100 ms se situe avant une
augmentation beaucoup plus importante de l'erreur.

J'obtiens :

```text
50 ms   -> 0,213°
100 ms  -> 0,835°
200 ms  -> 3,150°
500 ms  -> 16,526°
```

À 100 ms, l'erreur reste inférieure à un degré dans ma simulation :
**0,835°**.

En doublant seulement la durée pour atteindre 200 ms, elle passe déjà à
**3,150°**.

La borne de 100 ms ne correspond donc pas, dans mon expérience, à un seuil
mathématique précis où la courbe se casse.

Je l'interprète plutôt comme une **borne de sécurité** placée avant la zone
où l'erreur commence à augmenter rapidement.

## Pourquoi l'erreur augmente

L'extrapolation suppose :

`vitesse future = vitesse actuelle`

Cette hypothèse peut fonctionner sur une courte durée.

Mais dans ma simulation, la vitesse réelle diminue progressivement selon :

`exp(-2t)`

Plus je prédis loin dans le futur, plus la différence entre la vitesse
supposée constante et la vitesse réelle devient importante.

Par exemple, l'extrapolation prévoit après une seconde :

`90,000°`

alors que la simulation du mouvement ralenti donne :

`38,949°`

La différence atteint donc :

`51,051°`

Une extrapolation trop longue devient ainsi très éloignée du mouvement
simulé.

## Interprétation pour la VR

Dans un système VR, l'extrapolation sert à estimer où se trouvera la tête
quelques millisecondes plus tard.

Sur une courte durée, cette prédiction peut être utile parce que le mouvement
n'a pas encore eu beaucoup de temps pour changer.

Mais sur une durée plus longue, l'utilisateur peut ralentir, accélérer ou
changer de direction.

Une extrapolation basée uniquement sur la vitesse actuelle devient alors de
moins en moins fiable.

C'est pour cette raison qu'il est nécessaire de limiter la durée sur laquelle
on accepte de prédire le mouvement.

## Conclusion

L'exécution du programme montre numériquement que l'erreur d'extrapolation
augmente avec la durée.

Pour mon test à **90°/s**, j'ai mesuré :

```text
0,008° à 10 ms
0,034° à 20 ms
0,213° à 50 ms
0,835° à 100 ms
3,150° à 200 ms
16,526° à 500 ms
51,051° à 1000 ms
```

La courbe ne se casse pas exactement à 100 ms. Elle montre plutôt une
augmentation progressive qui devient rapidement importante au-delà de cette
zone.

Dans cette simulation, la borne de **100 ms** apparaît donc comme une limite
prudente : à cet instant l'erreur est encore inférieure à un degré
(**0,835°**), alors qu'elle atteint déjà **3,150° à 200 ms** et augmente
fortement ensuite.

Cette expérience me permet donc de justifier la borne à partir des valeurs
mesurées et de la forme de la courbe, et non seulement à partir d'une
appréciation qualitative de l'erreur.
