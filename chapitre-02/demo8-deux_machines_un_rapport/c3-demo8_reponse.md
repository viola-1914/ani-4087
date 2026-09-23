# Chapitre 02 — Démo 8 : deux machines, un rapport

## La boucle

Un seul calcul, le même sur les deux machines : **500 tours** de **1 000 000 de points 3D** transformés par une matrice 4×4, soit 500 millions de points. Il est chronométré avec `std::chrono::steady_clock`, du premier au dernier tour, et le programme calcule lui-même le temps par tour et la part d'une image de casque (11 ms).

Un « témoin », accumulé à partir des résultats, sert à deux choses : empêcher le compilateur de supprimer le calcul, et prouver devant la classe que les deux machines ont fait **exactement** le même travail.

Les deux versions sont construites en **Release**, à partir du même `src/main.cpp`.

## Premier temps : sur ma machine

```text
jenga clean --config Release
jenga build --config Release
jenga run MaSalle --config Release
```

```text
Calcul : 500 tours x 1000000 points = 500 millions de points
Temps total   : 1137 ms (1.14 s)
Par tour      : 2.275 ms
Budget casque : 11.0 ms -> un tour occupe 21 % de l'image
(temoin : -7336.9)
```

## Deuxième temps : sur l'appareil

TECNO SPARK 10C, Android 12, `arm64-v8a`. Pendant le calcul, l'écran reste noir plusieurs secondes, puis devient bleu-vert.

```text
C:\Android\platform-tools\adb.exe logcat -d -s MaSalle
09-23 15:00:13.744  6151  6185 I MaSalle : Mesure en cours, patientez...
09-23 15:00:16.724  6151  6185 I MaSalle : Calcul : 500 tours x 1000000 points = 500 millions de points
09-23 15:00:16.724  6151  6185 I MaSalle : Temps total   : 2937 ms (2.94 s)
09-23 15:00:16.724  6151  6185 I MaSalle : Par tour      : 5.873 ms
09-23 15:00:16.724  6151  6185 I MaSalle : Budget casque : 11.0 ms -> un tour occupe 53 % de l'image
09-23 15:00:16.724  6151  6185 I MaSalle : (temoin : -7336.9)
```

## Le rapport, écrit au tableau

| | Ma machine (Windows) | Le téléphone (Android) |
|---|---:|---:|
| Temps total | **1 137 ms** | **2 937 ms** |
| Par tour | **2,275 ms** | **5,873 ms** |
| Part d'une image de 11 ms | 21 % | 53 % |
| Témoin | -7336.9 | -7336.9 |

> **Rapport : 2 937 ÷ 1 137 ≈ 2,6**
>
> **Budget utile sur ma machine : 11 ÷ 2,6 ≈ 4,2 ms**

Le témoin est identique au dixième près sur les deux machines : même calcul, mêmes résultats, seule la vitesse change. **Le téléphone est 2,6 fois plus lent.**

Deux chiffres à retenir pour la suite :

- il reste **5,1 ms** libres sur l'appareil quand ce calcul tourne, contre 8,7 ms sur l'ordinateur ;
- **moins de deux tours** de cette boucle tiennent dans une image sur le téléphone (11 ÷ 5,873 ≈ 1,9), contre presque cinq sur l'ordinateur.

*(Mesuré à nouveau ce jour. À l'exercice 21, la même boucle donnait 1 099 ms et 3 105 ms, soit un rapport de 2,8. D'une séance à l'autre, le rapport varie un peu, selon ce que font les deux machines par ailleurs : une mesure isolée donne un ordre de grandeur, pas une constante.)*

## La question posée à la classe

> Une scène tourne à 11 ms sur un ordinateur portable, et 2,6 fois plus lentement sur l'appareil, donc à environ 29 ms : trois fois trop. **Que faut-il retirer de la scène pour tenir dans le budget ?**

## Les propositions notées

La classe était jouée par un assistant IA, à qui j'ai posé la question après avoir écrit le rapport au tableau. Voici les propositions telles qu'elles sont venues, avec ce que chacune coûte en contrepartie. Je ne les tranche pas ici : **on y reviendra au chapitre 16.**

| # | Proposition | Ce qu'elle fait gagner | Ce qu'elle coûte |
|---:|---|---|---|
| 1 | Réduire le nombre de triangles : maillages plus simples, niveaux de détail selon la distance | moins de sommets à transformer, c'est exactement le calcul mesuré ici | travail d'artiste, silhouettes moins fines de près |
| 2 | Réduire le nombre d'objets dessinés séparément, en les regroupant | chaque objet coûte un appel de dessin, même s'il est minuscule | objets moins faciles à animer ou à modifier un par un |
| 3 | Ne plus calculer l'éclairage en temps réel : le précalculer dans les textures | l'éclairage est souvent la part la plus lourde | plus de lumières qui bougent, ni de jour/nuit |
| 4 | Supprimer les ombres portées en temps réel, ou n'en garder qu'une | très coûteuses, elles redessinent la scène | perte de contact visuel entre les objets et le sol |
| 5 | Réduire les effets de particules : fumée, poussière, étincelles | beaucoup de surfaces transparentes superposées, très coûteuses | ambiance plus pauvre |
| 6 | Réduire la taille des textures et utiliser un format compressé | moins de mémoire lue à chaque image | images moins nettes de près |
| 7 | Simplifier les matériaux : moins de reflets, moins de couches | moins de calcul par pixel, et il y a beaucoup de pixels | rendu plus plat |
| 8 | Ne pas dessiner ce qu'on ne voit pas : hors du champ de vision, ou caché derrière un mur | gain énorme dans une salle fermée, comme MaSalle | code de tri à écrire, et à tester |
| 9 | Réduire les effets appliqués à l'image entière après coup | chacun repasse sur tous les pixels | image moins « léchée » |
| 10 | Calculer moins souvent ce qui n'a pas besoin de chaque image : physique, intelligence des personnages | on libère du temps sans rien retirer à l'écran | mouvements moins fluides si on exagère |
| 11 | Répartir le travail sur plusieurs fils d'exécution | les processeurs du casque ont plusieurs cœurs | code plus difficile, et sujet à des bogues nouveaux |
| 12 | Précalculer avant le lancement ce qui ne change pas | zéro coût par image | temps de chargement plus long, plus de mémoire |

**Deux remarques de la classe, à garder aussi :**

- « Les 11 ms ne sont pas toutes pour le calcul » : il faut y loger le rendu des **deux yeux**, le compositeur et la lecture des capteurs. Le budget réel pour la logique est bien plus petit que 11 ms.
- « Mesurez avant de couper » : on a tendance à retirer ce qui se voit le plus, pas ce qui coûte le plus. Sans mesure sur l'appareil, on sacrifie l'image pour rien.

## Ce que la démo montre

1. **Le même code, deux vitesses.** Le témoin identique le prouve : rien n'a changé dans le calcul, seule la machine change.
2. **Un chiffre mesuré sur l'ordinateur ne dit rien du casque.** 21 % d'une image ici, 53 % là-bas. Pour décider, il faut diviser par le rapport des deux machines, ou mieux, mesurer sur l'appareil visé.
3. **Le budget se tient en retirant, pas en espérant.** D'où la liste ci-dessus, qu'on relira au chapitre 16, quand il faudra vraiment choisir.
