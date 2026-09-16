# Exercice 2 — Le tableau des budgets

Pour cet exercice, j'ai repris les cinq étapes présentées dans le chapitre et j'ai essayé de trouver des mesures publiées pour chacune d'elles.

Pendant mes recherches, j'ai constaté qu'il est assez difficile de trouver une mesure séparée pour chaque étape. La plupart des études donnent plutôt la latence totale entre le mouvement de la tête et l'affichage de l'image. J'ai donc préféré signaler les valeurs que je n'ai pas pu trouver plutôt que d'en inventer.

| Étape | Valeur donnée dans le cours | Valeur mesurée trouvée | Remarque |
|---|---:|---:|---|
| Capteurs | 1 à 2 ms | Non trouvée séparément | Je n'ai pas trouvé de mesure fiable concernant uniquement les capteurs. |
| Transmission | 1 à 3 ms | Non trouvée séparément | Les sources consultées ne donnent pas clairement le temps de transmission seul. |
| Application / rendu | 5 à 11 ms | Non trouvée séparément | Le temps de rendu varie selon le matériel, le logiciel et la scène affichée. |
| Composition | 1 à 2 ms | Non trouvée séparément | Je n'ai pas trouvé de valeur mesurée qui permette d'isoler uniquement cette étape. |
| Affichage | 2 à 5 ms | Environ 0,33 à 2 ms pour la durée d'illumination | Warburton et ses collaborateurs donnent des valeurs allant de 0,33 ms pour le Valve Index à environ 2 ms pour l'Oculus Rift. |

## Ce que j'ai retenu de mes recherches

Les valeurs du cours donnent surtout une idée du temps disponible à chaque étape. Dans les recherches que j'ai consultées, les auteurs mesurent plus souvent l'ensemble du délai « mouvement vers photon » que chaque étape séparément.

Par exemple, Warburton et ses collaborateurs ont testé plusieurs casques de réalité virtuelle, notamment le HTC Vive, l'Oculus Rift, l'Oculus Rift S et le Valve Index. Ils montrent que la latence peut varier selon le casque et selon la manière dont le mouvement est effectué.

Cela m'a permis de comprendre qu'il n'est pas toujours possible d'attribuer une valeur précise à chaque étape de la chaîne. C'est pourquoi j'ai indiqué « non trouvée séparément » lorsque je n'avais pas de mesure suffisamment claire.

## Sources consultées

1. Warburton, M., Mon-Williams, M., Mushtaq, F. & Morehead, J. R. (2023).
   "Measuring motion-to-photon latency for sensorimotor experiments with virtual reality systems."
   Behavior Research Methods, 55, 3658–3678.
   DOI : 10.3758/s13428-022-01983-5

2. Zhao, J., Allison, R. S., Vinnikov, M. & Jennings, S. (2017).
   "Estimating the motion-to-photon latency in head mounted displays."
   IEEE Virtual Reality (VR).
   DOI : 10.1109/VR.2017.7892302

## Conclusion

Cette recherche m'a montré que les valeurs du tableau du cours sont utiles pour comprendre comment se répartit le temps dans un système de réalité virtuelle. Cependant, dans les publications consultées, toutes les étapes ne sont pas mesurées séparément.

J'ai donc préféré laisser certaines valeurs comme « non trouvées séparément » plutôt que de donner des chiffres que je ne pouvais pas vérifier.
