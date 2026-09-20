# Chapitre 02 — Exo 10 : mesurer en Debug et en Release

## Le calcul mesuré

Le programme `src/main.cpp` simule le travail d'un moteur à chaque image : il transforme **1 000 000 de points 3D par une matrice 4×4** (une rotation qui change à chaque image, plus une translation). Il répète ce calcul sur **50 images** et chronomètre chacune avec `std::chrono::steady_clock`, uniquement autour de la boucle de transformation.

Il affiche ensuite le temps moyen par image, la meilleure et la pire image, et les compare au budget d'une image de casque, 11 ms. Une valeur « témoin », calculée à partir des résultats, est affichée à la fin : elle empêche le compilateur de supprimer en Release un calcul dont personne n'utiliserait le résultat. Le témoin vaut 1837.8 dans les deux configurations : elles ont bien fait le même calcul.

Les deux configurations sont réglées explicitement dans `exo10.jenga` :

```python
        with filter("config:Debug"):
            defines(["_DEBUG"])
            optimize("Off")
            symbols(True)

        with filter("config:Release"):
            defines(["NDEBUG"])
            optimize("Speed")
            symbols(False)
```

## Conditions de mesure

Windows x86_64, Jenga 2.8.0, chaîne `mingw`, PowerShell, depuis `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo10-mesurer_en_debug_et_en_release` :

```text
jenga clean
jenga build --config Debug
jenga run App --config Debug
jenga build --config Release
jenga run App --config Release
```

Chaque configuration n'a été lancée qu'une fois, sur mon ordinateur.

## Le rapport

Sortie en Debug :

```text
Configuration : Debug
Calcul        : 1000000 points transformes par image, 50 images
Temps moyen   : 10.688 ms par image
Meilleur      : 9.255 ms
Pire          : 12.793 ms
Budget casque : 11.0 ms -> tient dans le budget
(temoin : 1837.8)
```

Sortie en Release :

```text
Configuration : Release
Calcul        : 1000000 points transformes par image, 50 images
Temps moyen   : 2.928 ms par image
Meilleur      : 2.274 ms
Pire          : 5.715 ms
Budget casque : 11.0 ms -> tient dans le budget
(temoin : 1837.8)
```

| Configuration | Moyenne | Meilleure image | Pire image | Part du budget de 11 ms (moyenne) |
|---|---:|---:|---:|---:|
| Debug | 10,688 ms | 9,255 ms | 12,793 ms | 97 % |
| Release | 2,928 ms | 2,274 ms | 5,715 ms | 27 % |

Le même calcul est environ **3,7 fois plus rapide** en Release qu'en Debug.

## Laquelle m'aurait fait prendre une mauvaise décision ?

**La mesure Debug.** En moyenne, elle annonce 10,7 ms : le calcul occuperait 97 % d'une image de casque, et la pire image, à 12,8 ms, dépasse déjà le budget. Sur cette base, j'aurais conclu qu'il n'y a plus aucune marge, que le casque va sauter des images, et j'aurais décidé de simplifier ce calcul, de réduire le nombre de points, ou de renoncer à ajouter autre chose dans l'image.

Ce serait une mauvaise décision, parce que le programme qu'on livre sur un casque est compilé en Release. Là, le même calcul prend 2,9 ms en moyenne, soit 27 % du budget, et même la pire image (5,7 ms) laisse près de la moitié de l'image libre. Il reste de la place pour le rendu et le reste du travail.

Le piège est d'autant plus sournois ici que la ligne de conclusion du Debug dit « tient dans le budget » : en ne regardant que la moyenne, on croirait être juste à la limite, alors que la pire image montre déjà un dépassement. Dans les deux lectures, le Debug donne une image fausse de la situation réelle. Les décisions de performance doivent se prendre sur des mesures en Release, la configuration qu'on livre.

## Limites

Une seule exécution par configuration, sur une seule machine : les valeurs varient d'un lancement à l'autre (la pire image surtout). Je n'ai pas mesuré sur un vrai casque, dont le processeur peut être plus lent que celui de mon ordinateur.
