# Chapitre 02 — Démo 4 : le même projet, deux machines

## Le projet

Le projet de l'exercice 8, `exo8.jenga` et `src/main.cpp`, recopiés tels quels dans ce dossier. Il contient deux filtres :

```python
        # ----- Windows : API Win32 -----
        with filter("system:Windows"):
            defines(["EXO8_WINDOWS"])
            links(["user32", "gdi32"])

        # ----- Linux : fils POSIX et mathematiques -----
        with filter("system:Linux"):
            defines(["EXO8_LINUX"])
            links(["pthread", "m"])
```

Le programme appelle une fonction de chaque bibliothèque (`GetSystemMetrics` de `user32`, `GetStockObject` de `gdi32` sous Windows ; `pthread_create` et `cbrt` sous Linux) et affiche quel filtre il a reçu.

## Les deux machines, et « sans modifier une ligne »

| | Machine 1 | Machine 2 |
|---|---|---|
| Système | **Windows** x86_64 | **Linux** : Ubuntu 26.04.1 LTS, x86_64 |
| Compilateur | GCC 16.2.0 de MSYS2 (`mingw`) | g++ 15.2.0 (`host-gcc`) |
| Jenga | 2.8.0 | 2.8.0 |
| Dossier du projet | `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\demo4-le_meme_projet_deux_machines` | `/home/hp/ani-4087/chapitre-02/demo4-le_meme_projet_deux_machines` |

Je n'ai pas de deuxième ordinateur sous Linux. J'ai donc installé **Ubuntu sous WSL** (le sous-système Linux de Windows) sur mon propre ordinateur : c'est un vrai système Linux, avec son propre compilateur et ses propres bibliothèques, qui tourne à côté de Windows sur le même matériel. J'y ai installé g++ et Jenga, puis **cloné mon dépôt depuis GitHub** et construit le projet tel qu'il y est, sans le modifier :

```text
sudo apt install -y g++ git python3-pip
git clone https://github.com/RihenUniverse/Jenga.git
pip install -e ./Jenga --break-system-packages
git clone https://github.com/viola-1914/ani-4087.git
cd ani-4087/chapitre-02/demo4-le_meme_projet_deux_machines
```

La preuve que ce sont les mêmes fichiers, octet pour octet :

| Fichier | Empreinte MD5 sous Windows (`certutil -hashfile`) | Empreinte MD5 sous Linux (`md5sum`) |
|---|---|---|
| `exo8.jenga` | `3ac58e2ba1aa903d56d9798890124b70` | `3ac58e2ba1aa903d56d9798890124b70` |
| `src/main.cpp` | `ee53bf0e44cb74a4c2cd5b7bb93a11e4` | `ee53bf0e44cb74a4c2cd5b7bb93a11e4` |

Sur les deux machines, la même commande : `jenga clean`, puis `jenga build`. Les deux réussissent (`Build Successful`).

## Pourquoi `jenga info` ne peut pas le montrer

`jenga info` décrit le workspace, les projets et les chaînes disponibles, mais n'affiche ni définitions ni bibliothèques : sa sortie ne dépend pas des filtres (je l'avais montré à l'exercice 14, où elle restait identique, que la condition d'un filtre soit vraie ou fausse). Il faut donc interroger **le résultat** de la construction.

## Faire parler la construction

### 1. Le programme lui-même

**Windows** (`jenga run App`) :

```text
Systeme : Windows (filtre system:Windows applique)
user32 : largeur de l'ecran = 1366 px
gdi32  : pinceau blanc obtenu
```

**Linux** (`jenga run App --platform linux`, exécuté depuis `/home/hp/ani-4087/chapitre-02/demo4-le_meme_projet_deux_machines/Build/Bin/Debug-Linux/App/App`) :

```text
Systeme : Linux (filtre system:Linux applique)
pthread + m : racine cubique de 27 calculee dans un fil = 3
```

Le même `main.cpp` a produit deux programmes différents : chaque version ne contient que la branche du `#if` correspondant à la définition reçue.

### 2. Les définitions reçues

**Windows** (`jenga compile-flags`, puis `type .jenga\compileflags.jcdb`) :

```text
compiler        C:\msys64\ucrt64\bin\x86_64-w64-mingw32-g++.EXE
msvc    0
project App
std     c++17
def     EXO8_WINDOWS
```

**Linux** (`jenga compile-flags --platform linux-x86_64`, puis `cat .jenga/compileflags.jcdb`) :

```text
compiler        /usr/bin/g++
msvc    0
project App
dir     /home/hp/ani-4087/chapitre-02/demo4-le_meme_projet_deux_machines
std     c++17
def     EXO8_LINUX
```

### 3. Les bibliothèques liées dans l'exécutable

**Windows** (`objdump -p Build\Bin\Debug-Windows\App\App.exe | findstr /C:"DLL Name"`) :

```text
        DLL Name: GDI32.dll
        DLL Name: KERNEL32.dll
        DLL Name: api-ms-win-crt-environment-l1-1-0.dll
        DLL Name: api-ms-win-crt-heap-l1-1-0.dll
        DLL Name: api-ms-win-crt-locale-l1-1-0.dll
        DLL Name: api-ms-win-crt-math-l1-1-0.dll
        DLL Name: api-ms-win-crt-private-l1-1-0.dll
        DLL Name: api-ms-win-crt-runtime-l1-1-0.dll
        DLL Name: api-ms-win-crt-stdio-l1-1-0.dll
        DLL Name: api-ms-win-crt-string-l1-1-0.dll
        DLL Name: libstdc++-6.dll
        DLL Name: USER32.dll
```

**Linux** (`readelf -d Build/Bin/Debug-Linux/App/App | grep NEEDED`) :

```text
 0x0000000000000001 (NEEDED)             Shared library: [libstdc++.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libm.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
```

### 4. Les fonctions importées

**Windows** (`objdump -p ... | findstr "GetSystemMetrics GetStockObject"`) :

```text
        000082a0  <none>  02bf  GetStockObject
        00008428  <none>  01bf  GetSystemMetrics
```

**Linux** (`nm -D --undefined-only Build/Bin/Debug-Linux/App/App`, filtré) :

```text
                 U cbrt@GLIBC_2.2.5
                 U pthread_create@GLIBC_2.34
                 U pthread_join@GLIBC_2.34
```

## Les deux constructions côte à côte

| Ce que montre la construction | Windows | Linux |
|---|---|---|
| Filtre appliqué | `system:Windows` | `system:Linux` |
| Définition reçue | `EXO8_WINDOWS` | `EXO8_LINUX` |
| Bibliothèques du filtre, dans l'exécutable | `USER32.dll`, `GDI32.dll` | `libm.so.6` (`m`) ; `pthread` : voir plus bas |
| Fonctions importées | `GetSystemMetrics`, `GetStockObject` | `pthread_create`, `pthread_join`, `cbrt` |
| Ce qu'affiche le programme | largeur de l'écran, pinceau blanc | racine cubique calculée dans un fil |

**Même projet, même fichier, pas une ligne changée : les filtres appliqués ne sont pas les mêmes.** Chaque machine n'a gardé que le bloc de son système ; le bloc de l'autre système a été ignoré, sans laisser la moindre trace dans le résultat.

## Un détail que révèle la construction

Sous Linux (Ubuntu 26.04.1, glibc 2.43), `libpthread` n'apparaît pas dans la liste `NEEDED`, alors que le filtre demande `links(["pthread", "m"])`. Les fonctions `pthread_create` et `pthread_join` sont bien importées, mais depuis `libc.so.6` : leur suffixe `@GLIBC_2.34` le montre. Depuis la glibc 2.34, les fils POSIX font partie de la bibliothèque C elle-même. Le lien vers `pthread` demandé par le filtre est donc accepté mais n'ajoute rien sur cette machine. C'était une des limites relevées à l'exercice 8, et c'est la construction, pas `jenga info`, qui le rend visible.

## Ce que la démo montre

1. **Un fichier de projet peut porter plusieurs systèmes à la fois**, et chaque machine n'en applique qu'une partie.
2. **Pour savoir ce qui a été appliqué, il faut interroger ce qui a été produit** : le programme lui-même, les définitions reçues (`compile-flags`), les bibliothèques et les fonctions importées de l'exécutable (`objdump` sous Windows, `readelf` et `nm` sous Linux).
3. **Le résultat en dit parfois plus que le fichier de projet**, comme ici pour `pthread` : le filtre demande une bibliothèque que l'exécutable n'utilise finalement pas.
