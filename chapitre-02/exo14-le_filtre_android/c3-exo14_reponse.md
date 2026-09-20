# Chapitre 02 — Exo 14 : le filtre Android

## Le filtre ajouté

Dans `MaSalle.jenga`, le fichier de projet de l'exercice 12, j'ai ajouté le filtre Android, sur le modèle des applications Android du dépôt Nkentseu (`Mou.jenga`, `NkCameraDemos.jenga`) :

```python
        # ----- Android (chapitre 2, exercice 14) : le casque Quest 2 --------------
        with filter("system:Android"):
            usetoolchain("android-ndk")
            defines(["MASALLE_ANDROID", "MASALLE_QUEST"])
            links(["android", "log", "EGL", "GLESv3"])
            androidapplicationid("com.mafo.masalle")
            androidminsdk(24)
            androidtargetsdk(34)
            androidabis(["arm64-v8a"])      # le Quest 2 est un appareil ARM 64 bits
            androidnativeactivity(True)
            androidscreenorientation("landscape")
```

- **Bibliothèques** : `android` (l'application native), `log` (les journaux), `EGL` et `GLESv3` (l'affichage OpenGL ES).
- **Définitions** : `MASALLE_ANDROID` et `MASALLE_QUEST`, pour que le code sache qu'il tourne sur le casque.
- **Paquet** : identifiant d'application, versions d'Android minimale et visée, architecture `arm64-v8a` (celle du Quest 2), activité native, orientation paysage.

Pour que ce filtre puisse un jour s'activer, j'ai aussi ajouté `TargetOS.ANDROID` aux systèmes cibles et `TargetArch.ARM64` aux architectures du workspace.

Environnement : Windows x86_64, Jenga 2.8.0, chaîne `mingw`, PowerShell, depuis `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo14-le_filtre_android`.

## Première moitié : `jenga info` ne dit rien

J'ai lancé `jenga info` avec la condition fausse (`filter("system:Android")`, fausse sur ma machine Windows), puis avec la condition vraie (le même bloc écrit `filter("system:Windows")`). Les sorties sont **identiques, ligne pour ligne** :

```text
========================= Jenga Workspace: MaSalleWks ==========================

Location: C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo14-le_filtre_android
Entry file: C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo14-le_filtre_android\MaSalle.jenga
Configurations: Debug, Release
Platforms: Windows
Target OSes: Windows, Linux, Android
Target Architectures: x86_64, arm64


Projects
------------------------------------------------------------
Name      Kind          Language   Test   External
==================================================
MaSalle   WindowedApp   C++        No     No


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

`jenga info` n'affiche ni les définitions ni les bibliothèques d'un projet : il ne peut pas dire si un filtre s'applique.

## Ce qui, en revanche, répond

La commande `jenga compile-flags` accepte un `--platform` et écrit dans `.jenga\compileflags.jcdb` les définitions réellement appliquées au projet pour cette plateforme. Je l'ai lancée pour ma machine, dans les deux états du filtre :

Condition **vraie** (`filter("system:Windows")`) : les deux définitions du bloc apparaissent.

```text
std     c++17
def     WIN32_LEAN_AND_MEAN
def     _UNICODE
def     UNICODE
def     MASALLE_ANDROID
def     MASALLE_QUEST
def     _DEBUG
```

Condition **fausse** (`filter("system:Android")`, la version déposée) : elles ont disparu.

```text
std     c++17
def     WIN32_LEAN_AND_MEAN
def     _UNICODE
def     UNICODE
def     _DEBUG
```

Là où `jenga info` donnait deux sorties identiques, `jenga compile-flags` montre la différence au premier coup d'œil.

Pour la vraie plateforme Android, la même commande s'arrête chez moi :

```text
jenga compile-flags --platform android-arm64
{"error": "builder: No suitable toolchain found for Android arm64"}
```

C'est cohérent avec l'exercice 13 : l'Android NDK n'est pas installé sur ma machine.

## Deuxième moitié : comment je vérifierais qu'il s'active pour Android

Je lancerais `jenga compile-flags --platform android-arm64` puis `Get-Content .jenga\compileflags.jcdb`, et le filtre s'active bien pour Android si les lignes `def MASALLE_ANDROID` et `def MASALLE_QUEST` y apparaissent ; sur ma machine, cette vérification attend l'installation de l'Android NDK.
