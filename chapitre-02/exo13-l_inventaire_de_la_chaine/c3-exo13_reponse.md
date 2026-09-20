# Chapitre 02 — Exo 13 : l'inventaire de la chaîne

## La commande

Lancée dans PowerShell, sous Windows x86_64, depuis le dossier de l'exercice 12 :

```text
PS C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo12-votre_premier_fil> jenga info -v
```

## Le tableau `Available Toolchains`, en entier

```text
Available Toolchains
------------------------------------------------------------
Name       Family   Target OS   Arch     Env
==============================================
host-gcc   gcc      Windows     x86_64   mingw
mingw      gcc      Windows     x86_64   mingw
```

La section `System` de la même sortie précise la machine :

```text
Host OS: Windows
Host Architecture: x86_64
Host Environment: msvc
Host Triple: x86_64-pc-windows-msvc
Python: 3.14.6 (tags/v3.14.6:c63aec6, Jun 10 2026, 10:26:10) [MSC v.1944 64 bit (AMD64)]
Jenga version: 2.8.0
```

## Ce qui est présent

Une seule chaîne, vue deux fois. `host-gcc` et `mingw` désignent le même compilateur : le GCC de MSYS2 (`C:\msys64\ucrt64`, version 16.2.0 d'après les chemins des messages d'erreur de l'exercice 4). Il produit des programmes Windows x86_64 dans l'environnement `mingw`. C'est lui qui a construit tous les exercices du chapitre 2.

Attention à une ligne trompeuse : `Host Environment: msvc` ne veut pas dire que le compilateur de Visual Studio est installé. Dans le code de Jenga (`GetHostEnvironment`, dans `Jenga/Core/Platform.py`), cette valeur est fixée d'office à `msvc` pour tout hôte Windows, sans rien détecter. Aucune chaîne `msvc` n'apparaît d'ailleurs dans le tableau.

## Ce qui manque

J'ai comparé le tableau avec ce que Jenga cherche sur une machine Windows (fonction `DetectAll` de `Jenga/Core/Toolchains.py`).

| Chaîne cherchée par Jenga | Présente ? | Ce qu'elle permettrait |
|---|---|---|
| `host-gcc`, `mingw` (GCC MinGW) | **oui** | Windows x86_64 avec GCC |
| `msvc` (Visual Studio) | non | Windows avec le compilateur de Microsoft |
| `host-clang`, `clang-mingw`, `clang-cl` (Clang) | non | Windows avec Clang |
| `gcc-cross-linux` (chaîne croisée vers Linux) | non | construire pour Linux depuis Windows |
| `android-ndk` | non | Android, donc le casque Quest 2 |
| `emscripten` | non | le Web (WebAssembly) |
| `zig-*` | non | compilation croisée vers d'autres systèmes |

Conséquence directe : mon projet déclare `targetoses([TargetOS.WINDOWS, TargetOS.LINUX])`, mais aucune chaîne de ma machine ne vise Linux. La moitié Linux de mes fichiers de projet, celle de l'exercice 8, ne peut pas être construite ici.

## Ce qu'il faut installer, par ordre de priorité pour le cours

1. **Clang (LLVM).** C'est le plus urgent. Le moteur Nkentseu construit ses programmes Windows avec sa chaîne `nk-windows-clang-mingw`, qui appelle `clang` (`config/toolchain.jenga`), et ses cibles Linux et macOS avec `clang-native`. Sans Clang, les chapitres qui utilisent le moteur, dès le sprint 3 (NKWindow et NKEvent), risquent de ne pas se construire. Dans mon MSYS2, cela passe par le paquet Clang de l'environnement UCRT64, puis `jenga info -v` doit montrer une ligne `clang-mingw`.
2. **Android NDK.** Indispensable pour le casque Quest 2, qui tourne sous Android : le sprint 11 (« le vrai backend ») et le sprint 16 (« Bâtir et livrer »). Jenga le trouve par la variable d'environnement `ANDROID_NDK_ROOT` (ou `ANDROID_NDK_HOME`).
3. **WSL (Linux dans Windows).** Il me permettrait de vérifier moi-même la partie Linux de l'exercice 8, que je n'ai pas pu tester sur une vraie machine.
4. **Facultatif pour ce cours** : MSVC, Emscripten et Zig. Aucun sprint annoncé ne vise le Web, et la chaîne MinGW suffit pour Windows.

## Ce que j'en retiens

Le tableau ne montre que ce qui est présent. Rien n'y signale ce qui manque : pour savoir que l'Android NDK est absent, il faut savoir qu'il pourrait être là. C'est la même leçon qu'aux exercices 6 et 9, où l'outil se taisait aussi sur ce qu'il n'appliquait pas ou ne trouvait pas.
