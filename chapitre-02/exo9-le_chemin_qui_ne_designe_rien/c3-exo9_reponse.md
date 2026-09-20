# Chapitre 02 — Exo 9 : le chemin qui ne désigne rien

## Le montage

Le projet ne contient qu'un seul fichier source, `src/main.cpp`. Dans `exo9.jenga`, j'ai ajouté deux chemins qui ne désignent rien :

```python
    with project("App"):
        consoleapp()
        language("C++")
        cppdialect("C++17")
        # "src/modules/**.cpp" ne correspond a aucun fichier : le dossier n'existe pas.
        files(["src/main.cpp", "src/modules/**.cpp"])
        # "include_absent" n'existe pas sur le disque.
        includedirs(["include_absent"])
```

- le motif `src/modules/**.cpp` dans `files` : le dossier `src/modules` n'existe pas, le motif ne correspond à aucun fichier ;
- le dossier `include_absent` dans `includedirs` : il n'existe pas sur le disque.

Environnement : Windows x86_64, Jenga 2.8.0, chaîne `mingw`, PowerShell, depuis `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo9-le_chemin_qui_ne_designe_rien`.

## Ce que `jenga info` en dit

Rien. Voici la sortie complète, sans la bannière :

```text
============================ Jenga Workspace: Exo9 =============================

Location: C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo9-le_chemin_qui_ne_designe_rien
Entry file: C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo9-le_chemin_qui_ne_designe_rien\exo9.jenga
Configurations: Debug, Release
Platforms: Windows
Target OSes: Windows, Linux, macOS
Target Architectures: x86_64


Projects
------------------------------------------------------------
Name   Kind         Language   Test   External
==============================================
App    ConsoleApp   C++        No     No


Available Toolchains
------------------------------------------------------------
Name       Family   Target OS   Arch     Env
==============================================
host-gcc   gcc      Windows     x86_64   mingw
mingw      gcc      Windows     x86_64   mingw


Daemon
------------------------------------------------------------
Status: Not running
```

`jenga info` décrit le workspace, les projets et les chaînes disponibles, mais n'affiche ni la liste des fichiers, ni les motifs de `files`, ni les dossiers d'inclusion. Il ne peut donc rien signaler.

## Ce que `jenga build` en dit

Presque rien. Après `jenga clean`, puis `jenga build` :

```text
Configuration: Debug
Target:        Windows x86_64
Toolchain:     mingw

Build Order (1 projects):
  1. App [CONSOLE_APP]

ℹ Found 1 source file(s)
✓   [1/1] Compiled: main.cpp
ℹ Linking...
✓ Built: Build\Bin\Debug-Windows\App\App.exe
✓ Build Successful                                   Time: 0.49s
Projects Built:  1/1
Status:         ✓ SUCCESS
```

Aucune erreur, aucun avertissement. Le motif vide est ignoré en silence, et le dossier absent aussi : ni Jenga ni le compilateur ne le signalent. Le seul indice est la ligne `Found 1 source file(s)`.

## Contre-épreuve : le même projet sans les deux chemins

Pour vérifier que ces sorties ne contiennent vraiment aucune trace des deux erreurs, j'ai remplacé `files(...)` par `files(["src/main.cpp"])`, mis `includedirs(...)` en commentaire, puis relancé `jenga info`, `jenga clean` et `jenga build`.

- `jenga info` donne **exactement la même sortie**, ligne pour ligne.
- `jenga build` donne **la même sortie**, y compris `Found 1 source file(s)`. Seul le temps change (0.47 s au lieu de 0.49 s).

Autrement dit, rien dans ce que Jenga affiche ne permet de distinguer le projet fautif du projet propre.

J'ai ensuite remis le fichier dans son état fautif, celui déposé.

## Lequel m'aurait fait gagner du temps ?

`jenga build`, mais de peu. C'est la seule des deux commandes qui affiche une information liée à `files` : le nombre de sources trouvées. Si j'avais attendu des fichiers dans `src/modules`, lire `Found 1 source file(s)` au lieu de `Found 3` m'aurait mis sur la piste avant même que l'édition de liens échoue sur un symbole introuvable. `jenga info`, lui, ne m'aurait rien appris : il ne montre aucun chemin.

Pour le dossier d'inclusion, aucune des deux commandes ne m'aurait aidé. L'erreur n'apparaîtrait que le jour où un `#include` aurait besoin d'un en-tête censé s'y trouver, sous la forme « fichier introuvable » pointant vers le `#include`, pas vers la ligne fautive du `.jenga`.

La leçon : un chemin faux dans un fichier de projet ne produit pas d'erreur à l'endroit où il est écrit. Il faut lire ce que l'outil compte, et le comparer à ce qu'on attend.
