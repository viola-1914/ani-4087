# Chapitre 02 — Exo 5 : le define qui manque

## Le montage

L'en-tête `src/compteur.h` déclare la classe `Compteur` de deux façons selon le define `AVEC_COMPTEUR` :

```cpp
#pragma once

#ifdef AVEC_COMPTEUR
class Compteur {
public:
    void incrementer() { ++m_valeur; }
    int valeur() const { return m_valeur; }
private:
    int m_valeur = 0;
};
#else
class Compteur {};
#endif
```

Le programme `src/main.cpp` l'emploie :

```cpp
#include <iostream>
#include "compteur.h"

int main() {
    Compteur c;
    c.incrementer();
    c.incrementer();
    std::cout << "Valeur du compteur : " << c.valeur() << std::endl;
    return 0;
}
```

Le define est posé dans `exo5.jenga` par `defines(["AVEC_COMPTEUR"])`. Pour le retirer, je commente cette ligne.

Les constructions ont été faites dans PowerShell, sur Windows x86_64, avec Jenga 2.8.0 et la chaîne `mingw` (GCC de MSYS2). Avant chaque construction, je lance `jenga clean`, puis `jenga build` (configuration Debug par défaut).

## Message 1 — avec le define

La construction réussit :

```
ℹ Found 1 source file(s)
✓   [1/1] Compiled: main.cpp
ℹ Linking...
✓ Built: Build\Bin\Debug-Windows\App\App.exe
✓ Build Successful                                   Time: 0.55s
Projects Built:  1/1
Status:         ✓ SUCCESS
```

## Message 2 — sans le define

La compilation de `main.cpp` échoue. Jenga coupe les lignes longues dans son cadre ; les voici recollées, sans rien changer au texte :

```
C:\Users\HP\Downloads\exo5-le_define_qui_manque\chapitre-02\exo5-le_define_qui_manque\src\main.cpp: In function 'int main()':
C:\Users\HP\Downloads\exo5-le_define_qui_manque\chapitre-02\exo5-le_define_qui_manque\src\main.cpp:6:7: error: 'class Compteur' has no member named 'incrementer'
    6 |     c.incrementer();
      |       ^~~~~~~~~~~
C:\Users\HP\Downloads\exo5-le_define_qui_manque\chapitre-02\exo5-le_define_qui_manque\src\main.cpp:7:7: error: 'class Compteur' has no member named 'incrementer'
    7 |     c.incrementer();
      |       ^~~~~~~~~~~
C:\Users\HP\Downloads\exo5-le_define_qui_manque\chapitre-02\exo5-le_define_qui_manque\src\main.cpp:8:47: error: 'class Compteur' has no member named 'valeur'
    8 |     std::cout << "Valeur du compteur : " << c.valeur() << std::endl;
      |                                               ^~~~~~
```

Jenga conclut par `✗ Compilation failed` et `Errors: 3`.

## Ce qui se passe

Sans le define, c'est le **préprocesseur** qui choisit la branche `#else` : le compilateur ne reçoit que `class Compteur {};`. L'erreur est signalée à l'étape de **compilation**, mais sa cause se situe une étape plus tôt. Le message ne mentionne ni `AVEC_COMPTEUR`, ni `#ifdef`, ni `compteur.h` : il parle seulement de membres manquants, alors qu'en ouvrant l'en-tête on voit bien `incrementer()` et `valeur()` écrits noir sur blanc.

## Lequel j'aurais su diagnostiquer sans cet exercice

Le premier, sans hésiter : un build qui réussit, il n'y a rien à diagnostiquer.

Le second, non. Face à « `Compteur` n'a pas de membre `incrementer` », j'aurais ouvert `compteur.h`, vu la méthode déclarée, et cherché une faute de frappe, un mauvais `#include` ou un autre fichier du même nom. Je n'aurais pas pensé qu'un define absent pouvait faire disparaître le corps de la classe avant même la compilation. Désormais, quand le compilateur ne voit pas ce que je lis dans un en-tête, je vérifierai d'abord les `#ifdef` qui l'entourent et les defines passés au build.