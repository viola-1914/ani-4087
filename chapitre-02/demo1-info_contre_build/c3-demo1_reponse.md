# Chapitre 02 — Démo 1 : `info` contre `build`

## L'erreur volontaire

Je pars de mon fichier de projet `MaSalle.jenga` (celui de l'exercice 12) et j'y glisse une faute de frappe réaliste : deux chiffres inversés dans le dialecte C++.

```python
        cppdialect("C++71")        # au lieu de "C++17"
```

Le fichier reste du Python parfaitement valide, et `"C++71"` ressemble à s'y méprendre à `"C++17"` quand on relit vite.

Environnement : Windows x86_64, Jenga 2.8.0, chaîne `mingw`, depuis `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\demo1-info_contre_build`.

## Ce que montre `jenga build`

```text
jenga clean
jenga build
```

```text
Configuration: Debug
Target:        Windows x86_64
Toolchain:     mingw

Build Order (1 projects):
  1. MaSalle [WINDOWED_APP]

ℹ Found 1 source file(s)

Compilation Error: main.cpp
x86_64-w64-mingw32-gcc.EXE: error: unrecognized command-line option '-std=c++71'; did you mean '-std=c++11'?

✗ ✗ Compilation failed: ...\demo1-info_contre_build\src\main.cpp

✗ Build Failed
Errors: 1  | Failed files: 1

Echecs (1) — a corriger :
  ✗ MaSalle
```

## Ce que montre `jenga info`

```text
jenga info
```

```text
========================= Jenga Workspace: MaSalleWks ==========================

Location: C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\demo1-info_contre_build
Entry file: C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\demo1-info_contre_build\MaSalle.jenga
Configurations: Debug, Release
Platforms: Windows
Target OSes: Windows, Linux
Target Architectures: x86_64


Projects
------------------------------------------------------------
Name      Kind          Language   Test   External
==================================================
MaSalle   WindowedApp   C++        No     No


Available Toolchains
------------------------------------------------------------
Name          Family        Target OS   Arch     Env
========================================================
host-gcc      gcc           Windows     x86_64   mingw
mingw         gcc           Windows     x86_64   mingw
android-ndk   android-ndk   Android     arm64    android


Daemon
------------------------------------------------------------
Status: Not running
```

## La question posée à la salle

> Voici les deux sorties pour le **même** fichier de projet, qui contient **une** erreur. Laquelle des deux désigne la cause ?

## La réponse de la salle

Pour cette démonstration, la salle était un assistant IA, à qui j'ai présenté les deux sorties comme à un public. Sa réponse :

> **C'est `jenga build`.** `jenga info` ne voit rien : il affiche le workspace, le projet et les chaînes disponibles, et tout y paraît normal, parce qu'il ne montre aucun réglage de compilation, ni le dialecte, ni les définitions, ni les options. Il ne peut donc pas désigner une erreur qu'il n'affiche pas.
>
> `jenga build`, lui, désigne la cause, à condition de lire la bonne ligne. Le titre accuse `main.cpp` (« Compilation Error: main.cpp », « Compilation failed: …\src\main.cpp »), alors que ce fichier n'y est pour rien. La vraie cause est dans la ligne du compilateur : `unrecognized command-line option '-std=c++71'`. L'option `-std=c++71` est la traduction directe de `cppdialect("C++71")` : c'est elle qui ramène au fichier de projet. Le compilateur propose même `-std=c++11`, ce qui confirme qu'il s'agit d'un numéro de norme mal écrit.

## Ce que la démo montre

| | `jenga info` | `jenga build` |
|---|---|---|
| Voit-il l'erreur ? | non : sortie identique à celle d'un projet sain | oui : le build échoue |
| Désigne-t-il la cause ? | non | oui, dans la ligne du compilateur |
| Où risque-t-on de se tromper ? | on croit que le projet est bon | on croit que `main.cpp` est fautif |

Deux leçons pour la classe :

1. **`jenga info` n'est pas un vérificateur.** Il décrit le workspace, pas les réglages de compilation. Qu'il n'affiche rien d'anormal ne prouve rien (c'était déjà le constat des exercices 9 et 14).
2. **Dans une erreur de build, le nom du fichier n'est pas toujours la cause.** Le fichier cité est simplement celui que le compilateur traitait quand il a échoué. Il faut lire le message du compilateur jusqu'au bout, et remonter de l'option (`-std=c++71`) au réglage qui l'a produite (`cppdialect`).

## Pour réparer

Remettre la ligne d'origine dans `MaSalle.jenga` :

```python
        cppdialect("C++17")
```

Le fichier déposé dans ce dossier garde volontairement `"C++71"`, pour que la démonstration puisse être rejouée.
