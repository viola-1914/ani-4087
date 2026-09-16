# Exercice 12 — La borne de cent millisecondes

Pour cet exercice, j'ai étudié l'extrapolation d'une pose de tête qui tourne
initialement à 90 degrés par seconde.

L'extrapolation suppose que cette vitesse reste constante.

Pour obtenir une pose réelle de comparaison, j'ai simulé le mouvement par
petits pas de temps. Dans cette simulation, la tête commence progressivement
à ralentir. Cela permet de voir comment l'erreur augmente lorsque l'on
extrapole trop loin.

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
    // 90 degrés par seconde
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

## Résultats

Avec une vitesse initiale de 90 degrés par seconde, la prédiction à vitesse
constante donne :

| Durée | Angle extrapolé |
|---:|---:|
| 10 ms | 0,9° |
| 20 ms | 1,8° |
| 50 ms | 4,5° |
| 100 ms | 9,0° |
| 200 ms | 18,0° |
| 500 ms | 45,0° |
| 1 s | 90,0° |

Lorsque la vraie tête ralentit, l'écart entre cette prédiction et la pose
réelle augmente avec la durée.

À très courte durée, par exemple 10 ou 20 ms, l'hypothèse de vitesse constante
reste assez proche du mouvement réel.

À 100 ms, l'erreur commence déjà à devenir significative.

À 500 ms ou une seconde, continuer à supposer que la tête tourne exactement
à la même vitesse conduit à une prédiction très éloignée de la vraie pose.

## Pourquoi limiter à 100 ms ?

L'extrapolation n'est pas une connaissance exacte du futur. Elle suppose
simplement que le mouvement actuel va continuer.

Sur quelques millisecondes, cette approximation est raisonnable.

Mais plus la durée augmente, plus l'utilisateur a le temps de ralentir,
d'accélérer ou de changer de direction. Une prédiction lointaine basée sur
une vitesse constante devient donc de moins en moins fiable.

La borne de 100 ms constitue ainsi une protection contre une extrapolation
excessive.

## Conclusion

Cette expérience montre que l'erreur d'extrapolation augmente avec le temps.

Une prédiction courte peut améliorer la correspondance entre la pose utilisée
pour le rendu et la position future de la tête. En revanche, extrapoler trop
loin peut produire une pose moins réaliste que la pose de départ.

C'est pourquoi il est raisonnable de borner l'extrapolation à environ
100 millisecondes.
