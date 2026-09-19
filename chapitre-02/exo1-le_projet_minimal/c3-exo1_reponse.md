# Exercice 1 — Le projet minimal

## Objectif et explication

Construire une application C++ minimale nommée `MaSalle` avec Jenga.
L'espace de travail `MaSalleWks` contient un projet `MaSalle`, en C++17,
avec deux configurations : Debug et Release.

`windowedapp()` reprend le type d'application du chapitre. Cette déclaration
ne crée pas de fenêtre : le programme se termine immédiatement avec le code 0.
`files(["src/**.cpp"])` sélectionne les sources C++ sous `src`.
`targetdir` et `objdir` séparent les sorties selon le système et la configuration.
Les objets sont également rangés par projet.
Les filtres activent les symboles en Debug et l'optimisation en Release.

Aucun module NK n'est utilisé par ce programme : il n'y a donc pas de
bibliothèque à ajouter avec `links`, ni de projet à déclarer avec `dependson`.
Les bibliothèques et le démarrage ordinaires du langage sont gérés par la chaîne de compilation.

## Fichiers à placer dans le dossier de l'exercice

- `MaSalle.jenga`
- `src/main.cpp`
- `c3-exo1_reponse.md` (ce compte rendu)
- `jenga-build.txt` (journal complet de la construction)

## Fichier `MaSalle.jenga`

```python
from Jenga import *

with workspace("MaSalleWks", location="."):
    configurations(["Debug", "Release"])

    with project("MaSalle"):
        windowedapp()
        language("C++")
        cppdialect("C++17")
        location(".")
        files(["src/**.cpp"])

        targetdir("build/bin/%{cfg.system}/%{cfg.buildcfg}")
        objdir("build/obj/%{cfg.system}/%{cfg.buildcfg}/%{prj.name}")

        with filter("config:Debug"):
            symbols(True)
            optimize("Off")

        with filter("config:Release"):
            symbols(False)
            optimize("Speed")
```

## Fichier `src/main.cpp`

```cpp
int main()
{
    return 0;
}
```

## Vérification et construction

Depuis le dossier `chapitre-02/exo1-le_projet_minimal`, avec Jenga installé
et accessible dans le PATH :

```sh
jenga info -v
jenga build
```

La compilation ci-dessous a réellement été effectuée dans l'environnement
Linux x86_64 de l'assistant, avec la chaîne `host-gcc`, en configuration Debug.
Ce journal ne prétend pas provenir de l'ordinateur de l'étudiant.

Version utilisée : Jenga 2.8.0, dépôt https://github.com/RihenUniverse/Jenga,
commit `7eb93ec423199ef75a043f9249e5f0ece3dfa209`.

## Sortie réelle de `jenga build`

```text

╔══════════════════════════════════════════════════════════════════╗
║                                                                  ║
║                ██╗███████╗███╗   ██╗ ██████╗  █████╗             ║
║                ██║██╔════╝████╗  ██║██╔════╝ ██╔══██╗            ║
║                ██║█████╗  ██╔██╗ ██║██║  ███╗███████║            ║
║           ██   ██║██╔══╝  ██║╚██╗██║██║   ██║██╔══██║            ║
║           ╚█████╔╝███████╗██║ ╚████║╚██████╔╝██║  ██║            ║
║            ╚════╝ ╚══════╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝            ║
║                                                                  ║
║             Multi-platform C/C++ Build System v2.8.0             ║
║                                                                  ║
╚══════════════════════════════════════════════════════════════════╝

Loading workspace...

Configuration: Debug
Target:        Linux x86_64
Toolchain:     host-gcc

Build Order (1 projects):
  1. MaSalle [WINDOWED_APP]


╔══════════════════════════════════════════════════════════════════════════════════════════════╗
║  Project: MaSalle                                                        Kind: WINDOWED_APP  ║
╚══════════════════════════════════════════════════════════════════════════════════════════════╝

ℹ Found 1 source file(s)
✓   [1/1] Compiled: main.cpp
ℹ Linking...
✓ Built: build/bin/Linux/Debug/MaSalle

┌──────────────────────────────────────────────────────────────────────────────────────────────┐
│  ✓ Build Successful                                                             Time: 0.04s  │
└──────────────────────────────────────────────────────────────────────────────────────────────┘

════════════════════════════════════════════════════════════════════════════════
                                BUILD COMPLETED                                 
════════════════════════════════════════════════════════════════════════════════
Projects Built:  1/1
Time:           0.04s
Status:         ✓ SUCCESS
════════════════════════════════════════════════════════════════════════════════

```

Code de sortie de `jenga build` : **0**.

Exécutable produit : `build/bin/Linux/Debug/MaSalle`.
Il a ensuite été exécuté : aucune sortie et code de retour **0**.
La compilation Windows et la configuration Release n'ont pas été testées ici.

## Références consultées

- [API Jenga : workspace, project, windowedapp, files, targetdir, objdir et filtres](https://github.com/RihenUniverse/Jenga/blob/7eb93ec423199ef75a043f9249e5f0ece3dfa209/Jenga/Core/Api.py)
- [Exemple officiel de programme C++ minimal](https://github.com/RihenUniverse/Jenga/blob/7eb93ec423199ef75a043f9249e5f0ece3dfa209/Jenga/Exemples/01_hello_console/01_hello_console.jenga)

## Emplacement de remise

Dans le dépôt public `viola-1914/ani-4087`, ce compte rendu doit se trouver à :

`chapitre-02/exo1-le_projet_minimal/c3-exo1_reponse.md`

Conserver `MaSalle.jenga` et `src/main.cpp` à leurs emplacements indiqués pour
permettre de reconstruire le projet. Les binaires et les fichiers objets
produits dans `build/` ne sont pas nécessaires dans le dépôt.
