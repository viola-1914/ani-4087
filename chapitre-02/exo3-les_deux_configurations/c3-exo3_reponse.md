# Exercice 3 — Les deux configurations

## Les quatre nombres mesurés

| Configuration | Taille de l'exécutable (octets) | Temps de construction (secondes) |
|---|---:|---:|
| Debug | 42 196 | 4,01 |
| Release | 38 780 | 0,25 |

## Conditions et méthode

Les constructions ont été réalisées sur mon ordinateur Windows, avec Jenga
2.8.0 et la chaîne `mingw`, pour la cible Windows x86_64. Le projet est
`MaSalle`, de type `WindowedApp`, avec un seul fichier source `main.cpp`
contenant une fonction `main` qui retourne zéro.

Les commandes ont été exécutées dans :

```text
C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo1-le_projet_minimal
```

J'ai lancé le nettoyage, puis la construction Debug et enfin la construction
Release, dans cet ordre :

```powershell
jenga clean --all
jenga build --config Debug
(Get-Item ".\build\bin\Windows\Debug\MaSalle.exe").Length
jenga build --config Release
(Get-Item ".\build\bin\Windows\Release\MaSalle.exe").Length
```

Les tailles sont les longueurs des fichiers `.exe`, en octets, retournées par
PowerShell. Les temps sont ceux de la ligne `Time:` du bilan final de Jenga ;
ils ne comprennent pas la commande de nettoyage et ne sont pas des temps
d'exécution du programme. Chaque configuration n'a été mesurée qu'une fois.

## Extraits des résultats observés

### Debug

```text
Configuration: Debug
Target:        Windows x86_64
Toolchain:     mingw

ℹ Found 1 source file(s)
✓   [1/1] Compiled: main.cpp
ℹ Linking...
✓ Built: build\bin\Windows\Debug\MaSalle.exe

Projects Built:  1/1
Time:           4.01s
Status:         ✓ SUCCESS
```

Résultat de la commande PowerShell mesurant la taille :

```text
42196
```

### Release

```text
Configuration: Release
Target:        Windows x86_64
Toolchain:     mingw

ℹ Found 1 source file(s)
✓   [1/1] Compiled: main.cpp
ℹ Linking...
✓ Built: build\bin\Windows\Release\MaSalle.exe

Projects Built:  1/1
Time:           0.25s
Status:         ✓ SUCCESS
```

Résultat de la commande PowerShell mesurant la taille :

```text
38780
```

## Comparaison et interprétation

### Taille

L'exécutable Release est plus petit de **3 416 octets** :

```text
42 196 - 38 780 = 3 416 octets
```

Cela représente une réduction d'environ **8,10 %** par rapport à Debug.
Dans notre fichier de projet, Debug active les symboles de débogage et désactive
l'optimisation, tandis que Release désactive ces symboles et active
l'optimisation `Speed`. La réduction observée est cohérente avec ces réglages,
sans permettre d'attribuer précisément chaque octet économisé à l'un ou l'autre.
Le programme étant minimal, les éléments de démarrage et de liaison contribuent
aussi à la taille totale.

### Temps de construction

Lors de cet essai, Release a demandé **3,76 secondes de moins** que Debug :

```text
4,01 - 0,25 = 3,76 secondes
```

Le rapport des temps affichés est **16,04** : la durée Debug mesurée vaut
16,04 fois la durée Release mesurée. Ce constat ne signifie pas que Release
se construit toujours plus vite. Debug a été construit en premier ; la mise
en cache des outils et des fichiers par le système, l'activité de l'antivirus
ou la charge de la machine peuvent contribuer à l'écart. Le journal seul ne
permet pas d'en déterminer la cause exacte.

Les deux journaux indiquent une compilation de `main.cpp` et une édition de
liens. La mesure Release n'est donc pas simplement présentée comme un projet
déjà à jour. Néanmoins, une seule mesure par configuration ne constitue pas
une comparaison statistique des performances de construction.

## Bilan

Les deux constructions ont réussi. Pour cet essai, Debug produit un exécutable
de **42 196 octets en 4,01 s**, et Release un exécutable de **38 780 octets en
0,25 s**. Ces résultats concernent la taille des exécutables et leur construction,
pas la vitesse d'exécution de l'application.
