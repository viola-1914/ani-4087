# Chapitre 02 — Exo 4 : la dépendance retirée

## Le montage

Trois fichiers source dans le projet `App` :

- `main.cpp` appelle `somme_des_carres()`, déclarée dans `moduleA.h`
- `moduleA.cpp` définit `somme_des_carres()`, qui appelle elle-même `carre()`, déclarée dans `moduleB.h`
- `moduleB.cpp` définit `carre()`

`moduleA` dépend donc de `moduleB`. Avec les trois fichiers dans la liste, le build réussit et le programme affiche `25`.

On retire ensuite `moduleB.cpp` de la liste des fichiers du projet (`exo4.jenga`) :

```python
with project("App"):
    consoleapp()
    language("C++")
    cppdialect("C++17")
    files(["src/main.cpp", "src/moduleA.cpp"])   # "src/moduleB.cpp" retiré
```

Commandes lancées dans PowerShell (Windows x86_64, toolchain `mingw` : GCC 16.2.0 de MSYS2 UCRT64) :

```
jenga clean
jenga build
```

## Le message d'erreur exact

Jenga affiche l'erreur dans un cadre de largeur fixe qui coupe les lignes longues ; les voici recollées, sans rien changer au texte :

```
C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo4-la_dependance_retiree\Build\Obj\Debug-Windows\App\src_moduleA.obj: in function `somme_des_carres(int, int)':
C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/exo4-la_dependance_retiree/src/moduleA.cpp:3:(.text+0x17): undefined reference to `carre(int)'
C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/16.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:/Users/HP/Documents/GitHub/ani-4087/chapitre-02/exo4-la_dependance_retiree/src/moduleA.cpp:3:(.text+0x23): undefined reference to `carre(int)'
collect2.exe: error: ld returned 1 exit status
```

Jenga conclut par `✗ Link failed: Build\Bin\Debug-Windows\App\App.exe`.

## L'étape concernée : l'édition de liens

L'erreur appartient à **l'édition de liens**, pour trois raisons visibles dans la sortie :

1. **Les étapes précédentes ont réussi.** Jenga affiche `Compiled: main.cpp` et `Compiled: moduleA.cpp` : le préprocesseur a trouvé `moduleB.h` (qui est toujours sur le disque), la compilation et l'assemblage ont produit `src_moduleA.obj` sans erreur. L'échec survient seulement après `Linking...`.
2. **C'est l'éditeur de liens qui parle.** Le message vient de `ld.exe`, l'éditeur de liens de la chaîne MinGW, et `collect2.exe: error: ld returned 1 exit status` confirme que c'est lui qui a échoué ; Jenga titre d'ailleurs l'erreur `Link Failed`.
3. **La nature de l'erreur.** `undefined reference to 'carre(int)'` signifie que le symbole est *déclaré* (le compilateur l'a accepté grâce à `moduleB.h`) mais qu'aucun fichier objet fourni à l'éditeur de liens n'en contient la *définition*, puisque `moduleB.obj` n'a jamais été produit. L'éditeur de liens ne peut donc pas résoudre les deux appels à `carre()` (aux décalages `+0x17` et `+0x23` dans `src_moduleA.obj`).

À retenir : retirer l'en-tête `moduleB.h` aurait provoqué une erreur de compilation ; retirer le fichier source `moduleB.cpp` de la liste laisse passer la compilation et fait échouer l'édition de liens.