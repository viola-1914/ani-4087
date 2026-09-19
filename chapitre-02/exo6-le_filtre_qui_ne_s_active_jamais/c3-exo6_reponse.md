# Chapitre 02 — Exo 6 : le filtre qui ne s'active jamais

## Le principe

Le filtre porte sur le système cible (`system:...`). Ma machine est sous Windows et Jenga construit par défaut pour la plateforme hôte (Windows x86_64, chaîne `mingw`). Un filtre `system:Linux` y est donc toujours faux, et un filtre `system:Windows` toujours vrai.

À l'intérieur du filtre, je pose la définition `FILTRE_ACTIF`. Comme `jenga info` et `jenga build --verbose` n'affichent pas les définitions, c'est le programme lui-même qui dit s'il l'a reçue : `main.cpp` teste `FILTRE_ACTIF` avec `#ifdef` et affiche un message différent dans chaque cas. Le préprocesseur ne garde qu'une des deux lignes, donc la sortie du programme prouve ce que le compilateur a réellement reçu.

## Le fichier de projet — `exo6.jenga`

Version déposée (condition vraie) :

```python
from Jenga import *

with workspace("Exo6"):
    configurations(["Debug", "Release"])
    targetoses([TargetOS.WINDOWS, TargetOS.LINUX, TargetOS.MACOS])
    targetarchs([TargetArch.X86_64])

    with project("App"):
        consoleapp()
        language("C++")
        cppdialect("C++17")
        files(["src/main.cpp"])

        with filter("system:Windows"):
            defines(["FILTRE_ACTIF"])
```

Pour le cas 1 (condition fausse), seule la ligne du filtre change :

```python
        with filter("system:Linux"):
            defines(["FILTRE_ACTIF"])
```

## Le programme — `src/main.cpp`

```cpp
#include <iostream>

int main() {
#ifdef FILTRE_ACTIF
    std::cout << "FILTRE_ACTIF est defini : le filtre a ete applique." << std::endl;
#else
    std::cout << "FILTRE_ACTIF n'est pas defini : le filtre n'a pas ete applique." << std::endl;
#endif
    return 0;
}
```

## Les deux sorties du programme

Exécuté dans PowerShell, sur Windows x86_64, avec Jenga 2.8.0 et la chaîne `mingw`, depuis `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo6-le_filtre_qui_ne_s_active_jamais`.

Pour chaque cas : `jenga clean`, `jenga build`, puis `jenga run App`. Le `clean` garantit que `main.cpp` est bien recompilé avec les nouvelles définitions et qu'on n'exécute pas un ancien binaire. Dans les deux cas, le build réussit (`Build Successful`) et Jenga lance `Build\Bin\Debug-Windows\App\App.exe`, qui se termine normalement.

### Cas 1 — `filter("system:Linux")`, condition fausse

```
FILTRE_ACTIF n'est pas defini : le filtre n'a pas ete applique.
```

### Cas 2 — `filter("system:Windows")`, condition vraie

```
FILTRE_ACTIF est defini : le filtre a ete applique.
```

## Ce que j'en retiens

La définition du cas 1 n'est pas « perdue » par erreur : le filtre est évalué, sa condition est fausse, et tout ce qu'il contient est ignoré en silence. Aucun avertissement, aucune trace dans `jenga info` ni dans `jenga build --verbose`. Un filtre mal écrit ou mal ciblé ressemble donc exactement à un filtre qui fonctionne. La seule preuve fiable est dans le binaire produit : faire afficher au programme ce qu'il a reçu.