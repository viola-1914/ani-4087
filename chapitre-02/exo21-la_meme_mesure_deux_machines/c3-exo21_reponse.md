# Chapitre 02 — Exo 21 : la même mesure, deux machines

## La boucle

`src/main.cpp` contient une seule boucle de calcul, identique sur les deux machines : **500 tours**, et à chaque tour **1 000 000 de points 3D** transformés par une matrice 4×4 (une rotation qui change à chaque tour, plus une translation), soit 500 millions de points en tout. Elle est chronométrée avec `std::chrono::steady_clock`, du premier au dernier tour.

```cpp
static const int NB_POINTS = 1000000;   // points transformes par tour
static const int NB_TOURS  = 500;       // tours de boucle

    auto debut = std::chrono::steady_clock::now();
    for (int tour = 0; tour < NB_TOURS; ++tour) {
        ... // matrice du tour
        for (int i = 0; i < NB_POINTS; ++i)
            sortie[i] = Transformer(m, points[i]);
        temoin += sortie[tour * 1000].x;
    }
    auto fin = std::chrono::steady_clock::now();
```

Le programme rapporte le temps total, le temps par tour, et la part d'une image de casque (11 ms) qu'occupe un tour. Le « témoin », calculé à partir des résultats, empêche le compilateur de supprimer le calcul.

**Le réglage à une seconde.** Avec 300 tours, ma machine mettait 611 ms. J'ai porté la boucle à 500 tours pour approcher la seconde demandée.

Les deux versions sont construites en **Release** (`optimize("Speed")`), avec le même fichier source.

## Le rapport

**Sur ma machine** (Windows x86_64, chaîne `mingw`, `jenga run MaSalle --config Release`) :

```text
Calcul : 500 tours x 1000000 points = 500 millions de points
Temps total   : 1099 ms (1.10 s)
Par tour      : 2.197 ms
Budget casque : 11.0 ms -> un tour occupe 20 % de l'image
(temoin : -7336.9)
```

**Sur l'appareil** (TECNO SPARK 10C, Android 12, `arm64-v8a`, chaîne `android-ndk`, lu avec `adb logcat -d -s MaSalle`) :

```text
09-21 09:45:14.829 14116 14142 I MaSalle : Mesure en cours, patientez...
09-21 09:45:18.034 14116 14142 I MaSalle : Calcul : 500 tours x 1000000 points = 500 millions de points
09-21 09:45:18.034 14116 14142 I MaSalle : Temps total   : 3105 ms (3.11 s)
09-21 09:45:18.034 14116 14142 I MaSalle : Par tour      : 6.210 ms
09-21 09:45:18.034 14116 14142 I MaSalle : Budget casque : 11.0 ms -> un tour occupe 56 % de l'image
09-21 09:45:18.034 14116 14142 I MaSalle : (temoin : -7336.9)
```

| | Ma machine | Le téléphone | Rapport |
|---|---:|---:|---:|
| Temps total | 1 099 ms | 3 105 ms | × 2,8 |
| Par tour | 2,197 ms | 6,210 ms | × 2,8 |
| Part d'une image de 11 ms | 20 % | 56 % | |

Le témoin vaut **-7336.9 sur les deux machines** : elles ont bien fait exactement le même calcul, au bit près. La seule différence est le temps.

**Le même calcul prend 2,8 fois plus longtemps sur le téléphone que sur mon ordinateur.**

## Ce que cela implique pour mon budget de onze millisecondes

1. **Une mesure sur l'ordinateur ne dit rien du budget sur l'appareil.** Un tour de ce calcul occupe 20 % d'une image sur mon ordinateur, ce qui semble confortable ; sur le téléphone, il en occupe 56 %, plus de la moitié. Si j'avais décidé sur la mesure de l'ordinateur, j'aurais cru disposer de près de 9 ms libres, alors qu'il en reste moins de 5 sur l'appareil.

2. **Il faut diviser le budget par le rapport des deux machines.** Avec un facteur 2,8, tout calcul qui dépasse **11 ÷ 2,8 ≈ 3,9 ms sur mon ordinateur** dépasse les 11 ms sur le téléphone. Dès 2 tours de cette boucle par image (12,4 ms), le téléphone ne tient plus les 90 images par seconde, alors que l'ordinateur en tiendrait encore 5 (11 ms).

3. **Ces 11 ms ne sont pas toutes pour moi.** Dans une vraie image, il faut aussi le rendu des deux yeux, le compositeur, la lecture des capteurs. Un seul calcul qui prend déjà 56 % de l'image sur l'appareil laisse trop peu pour tout le reste.

4. **Il faut mesurer sur l'appareil visé.** Mon téléphone n'est pas un casque : le Quest 2 a un autre processeur, et son facteur par rapport à mon ordinateur sera différent. Le seul chiffre qui compte pour le budget est celui mesuré sur le casque lui-même, en Release, comme à l'exercice 10.

## Les limites de la mesure

- **Une seule exécution par machine.** Les temps varient d'un lancement à l'autre ; plusieurs mesures donneraient une moyenne et un écart.
- **Un seul type de calcul.** Ce facteur 2,8 vaut pour cette boucle de calcul en virgule flottante sur des données en mémoire. Un calcul dominé par les accès mémoire, ou le rendu par la carte graphique, aurait un autre rapport.
- **La chaleur.** Un téléphone ralentit quand il chauffe ; sur 3 secondes, l'effet est faible, mais une application qui tourne en continu dans un casque le subira.
- **Pas de casque.** Rien ici n'est mesuré sur le Quest 2.
