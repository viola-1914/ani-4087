# Chapitre 02 — Démo 3 : la coquille vide

## Le montage

Un seul en-tête, `compteur/include/compteur.h`, qui déclare deux classes différentes sous le même nom selon le define `COMPTEUR_COMPLET` :

```cpp
#pragma once

#ifdef COMPTEUR_COMPLET
// Avec le define : la classe complete, trois fonctions membres.
class Compteur {
public:
    Compteur();
    void incrementer();
    int  valeur() const;
private:
    int m_valeur;
};
#else
// Sans le define : une coquille vide, aucune fonction membre.
class Compteur {
};
#endif
```

Deux projets l'incluent, dans `Demo3.jenga` :

| Projet | Type | Define `COMPTEUR_COMPLET` | Ce qu'il voit de `Compteur` |
|---|---|---|---|
| `Compteur` (`compteur/src/compteur.cpp`) | bibliothèque statique | **non** | la coquille vide |
| `App` (`app/main.cpp`) | application console | **oui** : `defines(["COMPTEUR_COMPLET"])` | la classe complète |

La bibliothèque contient les définitions des fonctions, entourées du même `#ifdef COMPTEUR_COMPLET` que leurs déclarations. L'application crée un `Compteur`, l'incrémente deux fois et affiche sa valeur.

Environnement : Windows x86_64, Jenga 2.8.0, chaîne `mingw` (GCC 16.2.0 de MSYS2), depuis `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\demo3-la_coquille_vide`.

## Le même en-tête, avec et sans son define

L'option `-E` arrête le compilateur après le préprocesseur et montre ce qu'il reçoit vraiment ; `-P` retire les marques de ligne.

**Avec le define :**

```text
C:\msys64\ucrt64\bin\x86_64-w64-mingw32-g++.exe -E -P -DCOMPTEUR_COMPLET compteur\include\compteur.h

class Compteur {
public:
    Compteur();
    void incrementer();
    int valeur() const;
private:
    int m_valeur;
};
```

**Sans le define :**

```text
C:\msys64\ucrt64\bin\x86_64-w64-mingw32-g++.exe -E -P compteur\include\compteur.h

class Compteur {
};
```

(Les deux commandes affichent aussi l'avertissement `'#pragma once' in main file`, normal quand on donne un en-tête directement au compilateur.)

**À constater :** c'est le même fichier, mais ce n'est **pas la même classe**. La première a un constructeur, deux fonctions et un attribut ; la seconde n'a rien. Pour le compilateur, ce sont deux déclarations sans rapport, qui portent le même nom.

## Ce que fait le build

```text
jenga clean
jenga build
```

```text
Build Order (2 projects):
  1. Compteur [STATIC_LIB] →
  2. App [CONSOLE_APP] (depends: Compteur)

Project: Compteur                                                         Kind: STATIC_LIB
ℹ Found 1 source file(s)
✓   [1/1] Compiled: compteur.cpp
ℹ Linking...
✓ Built: Build\Lib\Debug-Windows\Compteur\Compteur.lib
✓ Build Successful

Project: App                                                             Kind: CONSOLE_APP
ℹ Found 1 source file(s)
✓   [1/1] Compiled: main.cpp
ℹ Linking...
Compilation Error: Link Failed
C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\demo3-la_coquille_vide\Build\Obj\Debug-Windows\App\main.obj: in function `main':
C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/demo3-la_coquille_vide/app/main.cpp:6:(.text+0x15): undefined reference to `Compteur::Compteur()'
C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/demo3-la_coquille_vide/app/main.cpp:7:(.text+0x21): undefined reference to `Compteur::incrementer()'
C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/demo3-la_coquille_vide/app/main.cpp:8:(.text+0x2d): undefined reference to `Compteur::incrementer()'
C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/demo3-la_coquille_vide/app/main.cpp:9:(.text+0x39): undefined reference to `Compteur::valeur() const'
collect2.exe: error: ld returned 1 exit status
✗ Link failed: Build\Bin\Debug-Windows\App\App.exe

✗ Build Failed   Errors: 5  | Failed files: 1
Projects Built:  1/2
```

(Les lignes du cadre d'erreur, que Jenga coupe à sa largeur, sont recollées ici sans rien changer au texte.)

Les **deux compilations réussissent** : `compteur.cpp` et `main.cpp` sont marqués `Compiled`. La bibliothèque elle-même est construite sans erreur. Seule l'**édition de liens** de l'application échoue.

## Ce que contient vraiment la bibliothèque

```text
C:\msys64\ucrt64\bin\nm.exe -C Build\Lib\Debug-Windows\Compteur\Compteur.lib

src_compteur.obj:
0000000000000000 b .bss
0000000000000000 d .data
0000000000000000 N .debug_line_str
0000000000000000 r .rdata$zzz
0000000000000000 t .text
```

`nm` liste les symboles d'une bibliothèque. Ici, il n'y a que des noms de sections, **aucun symbole `Compteur::`**. La bibliothèque est vide : compilée sans le define, elle a vu la coquille, et le `#ifdef` a retiré toutes les définitions.

Pour comparer : une fois le define ajouté à la bibliothèque (essai fait sous Linux, avec GCC), la même commande liste `Compteur::Compteur()`, `Compteur::incrementer()` et `Compteur::valeur() const`, et l'application affiche `Valeur du compteur : 2`.

## Pourquoi c'est le lieur, et non le compilateur, qui proteste

**Le compilateur ne voit jamais qu'un fichier à la fois.** Chaque `.cpp` est compilé seul, avec les en-têtes qu'il inclut et les defines de son projet :

- **`main.cpp`**, compilé **avec** le define, voit la classe complète. Il appelle `Compteur()`, `incrementer()` et `valeur()` : ces fonctions sont déclarées, donc les appels sont parfaitement valides. Le compilateur n'a pas besoin de leur code ; il laisse dans `main.obj` une **référence** à chacune, à résoudre plus tard.
- **`compteur.cpp`**, compilé **sans** le define, voit la coquille vide. Le `#ifdef` retire ses définitions, et il reste un fichier vide, lui aussi parfaitement valide.

Chacun des deux est cohérent **avec lui-même**. La contradiction n'existe qu'entre les deux, et **le compilateur ne les voit jamais ensemble**.

**Le lieur, lui, est le premier à réunir tous les morceaux.** Son travail est d'associer chaque référence laissée par `main.obj` à une définition trouvée dans un autre fichier objet ou dans une bibliothèque. Il cherche `Compteur::Compteur()`, `Compteur::incrementer()` et `Compteur::valeur() const` dans `Compteur.lib`, ne les trouve pas (`nm` le montre : elle est vide), et c'est lui qui écrit `undefined reference`.

**Ce que le lieur ne voit pas non plus.** Il ne dit pas que deux classes différentes portent le même nom : il ne connaît que des noms de symboles. Il proteste ici **par chance**, parce que la coquille n'a aucune fonction. Si la coquille avait eu les mêmes fonctions mais un contenu différent, par exemple un attribut de moins, tout se serait lié sans erreur, et le programme se serait trompé à l'exécution, sans aucun message. Un define qui change une classe doit donc être posé **de la même façon partout** où cette classe est utilisée.

## Le lien avec les exercices précédents

- **Exercice 5** : le programme n'avait pas le define et voyait la coquille. Il appelait des fonctions que la coquille ne déclarait pas, et c'est le **compilateur** qui protestait (`has no member named`).
- **Ici, c'est l'inverse** : le programme voit la classe complète, c'est la **bibliothèque** qui voit la coquille. Tout compile, et l'erreur n'apparaît qu'à l'**édition de liens**.
- **Exercice 11** : la démonstration XR du moteur pose explicitement `NK_RHI_VK_ENABLED`, parce que ce define est **local** au module NKRHI et ne se propage pas aux projets qui en dépendent. C'est exactement le piège de cette démo, dans un vrai moteur.

## Pour réparer

Poser le même define dans les deux projets. Dans `Demo3.jenga`, projet `Compteur` :

```python
        defines(["COMPTEUR_COMPLET"])
```

La ligne est laissée en commentaire dans le fichier déposé, pour que la démonstration puisse être rejouée.
