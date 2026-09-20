# Chapitre 02 — Exo 8 : deux systèmes, un fichier

## Le fichier de projet — `exo8.jenga`

```python
from Jenga import *

with workspace("Exo8"):
    configurations(["Debug", "Release"])
    targetoses([TargetOS.WINDOWS, TargetOS.LINUX])
    targetarchs([TargetArch.X86_64])

    with project("App"):
        consoleapp()
        language("C++")
        cppdialect("C++17")
        files(["src/main.cpp"])

        # ----- Windows : API Win32 -----
        with filter("system:Windows"):
            defines(["EXO8_WINDOWS"])
            links(["user32", "gdi32"])

        # ----- Linux : fils POSIX et mathematiques -----
        with filter("system:Linux"):
            defines(["EXO8_LINUX"])
            links(["pthread", "m"])
```

Chaque filtre pose une définition et ses bibliothèques :

- **Windows** : `user32` (fenêtres, écran, entrées) et `gdi32` (dessin 2D de l’API Win32).
- **Linux** : `pthread` (fils d’exécution POSIX) et `m` (la bibliothèque mathématique `libm`).

## Le programme — `src/main.cpp`

Le programme appelle réellement une fonction de chaque bibliothèque, pour que les liens déclarés servent à quelque chose, et il affiche quel filtre il a reçu :

- sous Windows, `GetSystemMetrics` (dans `user32`) donne la largeur de l’écran et `GetStockObject` (dans `gdi32`) récupère un pinceau blanc ;
- sous Linux, `pthread_create` et `pthread_join` (dans `pthread`) lancent un fil qui calcule `std::cbrt(27)` (dans `libm`).

Le code complet est dans `src/main.cpp`, à côté de ce fichier.

## Vérification sous Windows — sur ma machine

Windows x86_64, Jenga 2.8.0, chaîne `mingw`, PowerShell, depuis `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo8-deux_systemes_un_fichier\` :

```text
jenga clean
jenga build
jenga run App
```

Le build réussit (`Build Successful`, `Build\Bin\Debug-Windows\App\App.exe`). Sortie du programme :

```text
Systeme : Windows (filtre system:Windows applique)
user32 : largeur de l'ecran = 1366 px
gdi32  : pinceau blanc obtenu
```

## Vérification sous Linux — par mon camarade de classe

J’ai demandé à mon camarade de tester le projet sous Linux : il l’a construit et lancé dans son propre environnement Linux, **sans modifier le fichier**. Les empreintes MD5 de `exo8.jenga` et de `src/main.cpp` étaient identiques à celles de mes fichiers.

Environnement : Ubuntu 24.04.4 LTS, g++ 13.3.0, Jenga 2.8.0, chaîne `host-gcc`.

`jenga build` fonctionne sans option et cible directement la machine hôte :

```text
Configuration: Debug
Target:        Linux x86_64
Toolchain:     host-gcc

✓   [1/1] Compiled: main.cpp
ℹ Linking...
✓ Built: Build/Bin/Debug-Linux/App/App
✓ Build Successful
```

`jenga run App` échoue, parce qu’il part de la première cible déclarée (`WINDOWS`) au lieu du système hôte :

```text
Cannot create builder: No suitable toolchain found for Windows x86_64
```

`jenga run App --platform linux` lance bien le programme, qui affiche :

```text
Systeme : Linux (filtre system:Linux applique)
pthread + m : racine cubique de 27 calculee dans un fil = 3
```

Le filtre `system:Linux` s’applique donc, `EXO8_LINUX` arrive au programme, et `pthread` et `libm` sont trouvés à l’édition de liens. Le fichier n’a pas eu besoin d’être modifié ; seule la commande d’exécution demande `--platform linux`.

## Limites des vérifications

1. **Une seule distribution Linux a été testée.** Mon camarade a effectué les essais sous Ubuntu 24.04, avec glibc 2.39. Rien n’a été vérifié sur d’autres distributions ni avec Clang.

2. **Le caractère indispensable des bibliothèques Linux n’a pas été démontré.** Depuis glibc 2.34, les fonctions de `pthread` sont intégrées à la bibliothèque C, et `libm` est souvent liée d’office par `g++`. Mon camarade a constaté que le projet se construit aussi sans le `links(["pthread", "m"])`. Les liens Linux déclarés sont donc corrects, mais ces essais ne démontrent pas qu’ils sont indispensables dans cet environnement.

3. **Le caractère indispensable des bibliothèques Windows n’a pas été vérifié.** Je n’ai pas essayé de retirer `user32` ou `gdi32` pour voir si l’édition de liens échoue.