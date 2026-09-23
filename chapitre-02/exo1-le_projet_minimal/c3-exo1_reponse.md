# Chapitre 02 — Exo 1 : le projet minimal

## Le fichier de projet

`MaSalle.jenga`, à la racine du dossier de l'exercice. Il contient le strict nécessaire pour que Jenga sache quoi construire :

```python
# MaSalle — le projet minimal (chapitre 02, exercice 1)
#
# Le strict necessaire pour que Jenga sache quoi construire :
# un espace de travail, une configuration, un projet, un langage, un fichier.

from Jenga import *

with workspace("MaSalleWks"):
    configurations(["Debug", "Release"])

    with project("MaSalle"):
        consoleapp()                 # un programme en ligne de commande
        language("C++")
        cppdialect("C++17")
        files(["src/main.cpp"])      # la liste complete des sources : un fichier
```

Ligne par ligne :

| Ligne | Ce qu'elle dit à Jenga |
|---|---|
| `from Jenga import *` | le fichier de projet est un vrai programme Python : cette ligne rend disponibles les fonctions de Jenga |
| `with workspace("MaSalleWks")` | ouvre l'espace de travail, le contenant de tous les projets |
| `configurations(["Debug", "Release"])` | les deux façons de construire : sans optimisation et avec |
| `with project("MaSalle")` | ouvre le projet, la chose à construire |
| `consoleapp()` | ce projet produit un programme en ligne de commande |
| `language("C++")` et `cppdialect("C++17")` | le langage et sa norme |
| `files(["src/main.cpp"])` | les sources à compiler : ici, un seul fichier |

Rien d'autre n'est déclaré : ni dossier de sortie, ni filtres, ni options de compilation. Jenga choisit des valeurs par défaut pour tout le reste, et c'est ce que montre la sortie de construction.

## Le programme

`src/main.cpp`. L'énoncé dit qu'une fonction `main` qui rend zéro suffit ; j'ai ajouté deux lignes affichées, pour voir aussi le programme tourner.

```cpp
// MaSalle — exercice 1 : mes premiers pas avec Jenga.
// Le programme n'a rien a faire : il salue, puis rend zero,
// c'est-a-dire « tout s'est bien passe ».

#include <cstdio>

int main()
{
    std::printf("MaSalle - mes premiers pas avec Jenga\n");
    std::printf("Le projet est construit, le programme tourne.\n");
    return 0;
}
```

`return 0` est la convention : zéro signifie « terminé sans erreur ». C'est ce code que Jenga lit pour écrire `termine normalement` à la fin de l'exécution.

## La sortie de `jenga build`

```text
C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo1-le_projet_minimal>jenga clean
C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo1-le_projet_minimal>jenga build

Loading workspace...

Configuration: Debug
Target:        Windows x86_64
Toolchain:     mingw

Build Order (1 projects):
  1. MaSalle [CONSOLE_APP]


╔══════════════════════════════════════════════════════════════════════════════════════════════╗
║  Project: MaSalle                                                         Kind: CONSOLE_APP  ║
╚══════════════════════════════════════════════════════════════════════════════════════════════╝

ℹ Found 1 source file(s)
✓   [1/1] Compiled: main.cpp
ℹ Linking...
✓ Built: Build\Bin\Debug-Windows\MaSalle\MaSalle.exe

┌──────────────────────────────────────────────────────────────────────────────────────────────┐
│  ✓ Build Successful                                                             Time: 0.33s  │
└──────────────────────────────────────────────────────────────────────────────────────────────┘

════════════════════════════════════════════════════════════════════════════════
                                BUILD COMPLETED
════════════════════════════════════════════════════════════════════════════════
Projects Built:  1/1
Time:           0.33s
Status:         ✓ SUCCESS
════════════════════════════════════════════════════════════════════════════════
```

La même sortie est enregistrée dans `jenga-build.txt`, à côté de ce fichier (`jenga build > jenga-build.txt 2>&1`).

Environnement : Windows x86_64, Jenga 2.8.0, chaîne `mingw` (GCC de MSYS2).

## Ce que la sortie apprend, et que le fichier ne dit pas

| Ligne | Ce qu'elle révèle |
|---|---|
| `Configuration: Debug` | aucune configuration n'était demandée : Jenga a pris la **première** de la liste |
| `Target: Windows x86_64` | ni système ni architecture ne sont déclarés : Jenga prend ceux de ma machine |
| `Toolchain: mingw` | Jenga a choisi seul le compilateur, parmi ceux qu'il a trouvés installés |
| `Build Order (1 projects)` | il calcule un ordre de construction, même pour un seul projet |
| `Found 1 source file(s)` | il confirme ce que `files` a listé |
| `Built: Build\Bin\Debug-Windows\MaSalle\MaSalle.exe` | je n'ai déclaré aucun dossier de sortie : ce chemin, `Build/Bin/<config>-<systeme>/<projet>/`, est celui que Jenga choisit par défaut |

Autrement dit, un fichier de projet minimal ne veut pas dire une construction sans réglages : il veut dire que **c'est Jenga qui décide à ma place**. Tout ce que je n'écris pas, il le choisit, et c'est la sortie de construction qui me dit ce qu'il a choisi.

## L'exécution

```text
C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo1-le_projet_minimal>jenga run MaSalle

  ▶  EXECUTION  —  MaSalle.exe
     C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo1-le_projet_minimal\Build\Bin\Debug-Windows\MaSalle\MaSalle.exe

MaSalle - mes premiers pas avec Jenga
Le projet est construit, le programme tourne.

  ◀  FIN D'EXECUTION  —  termine normalement  (0.10s)
```

`termine normalement` correspond au `return 0` du programme.

## Ce que je retiens

1. **Cinq déclarations suffisent** pour construire : un espace de travail, des configurations, un projet, un langage, une liste de fichiers.
2. **Le reste est décidé par défaut**, et la sortie de construction est le seul endroit où on le lit : configuration, système, architecture, chaîne d'outils, dossier de sortie.
3. **Le fichier de projet est du Python**, ce qui explique le `from Jenga import *` et l'indentation en blocs `with`.
