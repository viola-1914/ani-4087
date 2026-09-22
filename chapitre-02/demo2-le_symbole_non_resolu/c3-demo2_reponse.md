# Chapitre 02 — Démo 2 : le symbole non résolu

## Le montage

Le projet `MaSalleDemo2.jenga` compte trois modules :

| Module Rôle                                  |                                                                                   |
| -------------------------------------------- | --------------------------------------------------------------------------------- |
| `geometrie` (`geometrie.h`, `geometrie.cpp`) | les calculs : distance, longueur d'un mur, surface, périmètre, centre d'une pièce |
| `salle` (`salle.h`, `salle.cpp`)             | décrit la pièce, en appelant **cinq** fonctions de `geometrie`                    |
| `main.cpp`                                   | appelle `salle`, et une fonction de `geometrie`                                   |

**La dépendance retirée :** `src/geometrie.cpp` est enlevé de la liste des fichiers.

```python
        # Liste complete : files(["src/main.cpp", "src/salle.cpp", "src/geometrie.cpp"])
        files(["src/main.cpp", "src/salle.cpp"])

```

L'en-tête `geometrie.h` reste là : tout **compile**, puisque chaque fonction est déclarée. Mais plus rien ne les **définit**.

Environnement : Windows x86_64, Jenga 2.8.0, chaîne `mingw` (GCC 16.2.0 de MSYS2), depuis `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\demo2-le_symbole_non_resolu`.

## Le mur de texte, en entier

```text
jenga clean
jenga build

```

La sortie, sans aucune coupure (le fichier `sortie-build.txt`, dans ce dossier, la contient aussi telle que Jenga l'a écrite) :

```text
Loading workspace...

Configuration: Debug
Target:        Windows x86_64
Toolchain:     mingw

Build Order (1 projects):
  1. MaSalle [CONSOLE_APP]


╔══════════════════════════════════════════════════════════════════════════════════════════════╗
║  Project: MaSalle                                                         Kind: CONSOLE_APP  ║
╚══════════════════════════════════════════════════════════════════════════════════════════════╝

ℹ Found 2 source file(s)
✓   [1/2] Compiled: main.cpp
✓   [2/2] Compiled: salle.cpp
ℹ Linking...
╔══════════════════════════════════════════════════════════════════════════════════════════════╗
║                                Compilation Error: Link Failed                                ║
╠══════════════════════════════════════════════════════════════════════════════════════════════╣
║ C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin ║
║ /ld.exe:                                                                                     ║
║ C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\demo2-le_symbole_non_resolu\Build\Obj\Debu ║
║ g-Windows\MaSalle\src_main.obj: in function `main':                                          ║
║ C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/demo2-le_symbole_non_resolu/src/main.cpp:9 ║
║ :(.text+0x7d): undefined reference to `geometrie::Distance(geometrie::Point,                 ║
║ geometrie::Point)'                                                                           ║
║ C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin ║
║ /ld.exe:                                                                                     ║
║ C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\demo2-le_symbole_non_resolu\Build\Obj\Debu ║
║ g-Windows\MaSalle\src_salle.obj: in function `DecrireSalle()':                               ║
║ C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/demo2-le_symbole_non_resolu/src/salle.cpp: ║
║ 8:(.text+0x8f): undefined reference to `geometrie::LongueurMur(geometrie::Point,             ║
║ geometrie::Point)'                                                                           ║
║ C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin ║
║ /ld.exe:                                                                                     ║
║ C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/demo2-le_symbole_non_resolu/src/salle.cpp: ║
║ 9:(.text+0xdf): undefined reference to `geometrie::LongueurMur(geometrie::Point,             ║
║ geometrie::Point)'                                                                           ║
║ C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin ║
║ /ld.exe:                                                                                     ║
║ C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/demo2-le_symbole_non_resolu/src/salle.cpp: ║
║ 10:(.text+0x110): undefined reference to `geometrie::AirePolygone(geometrie::Point const*,   ║
║ int)'                                                                                        ║
║ C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin ║
║ /ld.exe:                                                                                     ║
║ C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/demo2-le_symbole_non_resolu/src/salle.cpp: ║
║ 11:(.text+0x141): undefined reference to `geometrie::Perimetre(geometrie::Point const*,      ║
║ int)'                                                                                        ║
║ C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin ║
║ /ld.exe:                                                                                     ║
║ C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/demo2-le_symbole_non_resolu/src/salle.cpp: ║
║ 12:(.text+0x177): undefined reference to `geometrie::Centre(geometrie::Point const*, int)'   ║
║ collect2.exe: error: ld returned 1 exit status                                               ║
╚══════════════════════════════════════════════════════════════════════════════════════════════╝
✗ Link failed: Build\Bin\Debug-Windows\MaSalle\MaSalle.exe

┌──────────────────────────────────────────────────────────────────────────────────────────────┐
│  ✗ Build Failed                                                                 Time: 4.70s  │
│ Errors: 7  | Failed files: 1                                                                 │
└──────────────────────────────────────────────────────────────────────────────────────────────┘

════════════════════════════════════════════════════════════════════════════════
                                  BUILD FAILED
════════════════════════════════════════════════════════════════════════════════
Projects Built:  0/1
Failed:         1
Errors:         7
Time:           4.71s
Status:         ✗ FAILURE
════════════════════════════════════════════════════════════════════════════════

Echecs (1) — a corriger :
  ✗ MaSalle

```

Le cadre de l'erreur fait à lui seul **34 lignes** de texte, pour une seule cause.

## La question posée aux deux camarades

> Dans ce mur, **une seule information** désigne le module qui manque. Laquelle ?

## La réponse des deux camarades

Pour cette démonstration, j'ai présenté la sortie à deux camarades, comme à un public. Leur réponse :

> **Le préfixe** **`geometrie::`** **des symboles non résolus.** Les six `undefined reference` visent `geometrie::Distance`, `geometrie::LongueurMur` (deux fois), `geometrie::AirePolygone`, `geometrie::Perimetre` et `geometrie::Centre`. Toutes les fonctions manquantes appartiennent au même espace de noms, `geometrie` : c'est le module `geometrie` que l'édition de liens n'a pas reçu. Son en-tête a suffi à compiler, mais son fichier `geometrie.cpp`, qui contient les définitions, n'est pas dans la liste des fichiers.
>
> Tout le reste est du bruit, ou pire, une fausse piste :
>
> - **les chemins de** **`ld.exe`**, répétés six fois, ne disent que le nom de l'outil qui se plaint : l'éditeur de liens ;
> - **les fichiers cités,** **`main.cpp`** **et** **`salle.cpp`****, avec leurs numéros de ligne**, sont ceux qui **appellent** les fonctions, pas celui qui manque ; ils sont en bon état ;
> - **les décalages** comme `(.text+0x7d)` sont des positions dans le code machine, inutiles ici ;
> - **`Errors: 7`** compte six références indéfinies plus la ligne de `collect2`, pour une seule cause ;
> - **`Echecs (1) — a corriger : ✗ MaSalle`** désigne le projet entier, pas le module.
>
> Il faut donc passer par-dessus les chemins, les lignes et les compteurs, et ne lire que ce qui suit `undefined reference to` : le nom du symbole. Son préfixe est le nom du module qui manque.

## Ce que la démo montre

| Dans le mur Ce que ça désigne vraiment Utile pour trouver le module ?  |                                                 |                                        |
| ---------------------------------------------------------------------- | ----------------------------------------------- | -------------------------------------- |
| `…/bin/ld.exe:`                                                        | l'outil qui signale l'erreur (éditeur de liens) | non, seulement l'étape                 |
| `src_main.obj`, `src_salle.obj`, `main.cpp:9`, `salle.cpp:8` à `12`    | là où les fonctions sont **appelées**           | non, fausse piste                      |
| `(.text+0x7d)`                                                         | une adresse dans le code machine                | non                                    |
| ``undefined reference to `geometrie::Distance(...)'``                  | **le symbole introuvable**                      | **oui : le préfixe** **`geometrie::`** |
| `Errors: 7`, `✗ MaSalle`                                               | un décompte et le projet                        | non                                    |

1. **Une erreur d'édition de liens se lit par les noms des symboles**, pas par les noms de fichiers. Les fichiers cités sont les utilisateurs de la fonction ; le module coupable, lui, n'apparaît jamais, justement parce qu'il est absent.
2. **Plusieurs erreurs pour une seule cause.** Six références indéfinies, sept erreurs comptées : si l'on corrige ligne par ligne, on croit avoir six problèmes. Le préfixe commun montre qu'il n'y en a qu'un. C'est pour cette raison que je range mon code dans des espaces de noms qui portent le nom du module.
3. **C'est la même panne qu'à l'exercice 4**, en plus grand : l'en-tête suffit à compiler, la définition manque à l'édition de liens.

## Pour réparer

Remettre le fichier dans la liste, dans `MaSalleDemo2.jenga` :

```python
        files(["src/main.cpp", "src/salle.cpp", "src/geometrie.cpp"])

```

Le programme se construit alors et affiche la description de la pièce (murs de 5 et 4 m, surface de 20 m², périmètre de 18 m). Le fichier déposé garde volontairement la liste incomplète, pour que la démonstration puisse être rejouée.