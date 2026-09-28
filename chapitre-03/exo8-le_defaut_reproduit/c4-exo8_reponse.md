# Chapitre 03 — Exercice 8 : le défaut reproduit

## Ce qui est demandé

Afficher `rawDeltaX` à chaque image, sans rien accumuler soi-même. Bouger la souris, puis
poser la main. Rendre les vingt lignes qui suivent l'arrêt, et dire en une phrase ce
qu'elles prouvent.

## Le montage

On lit et on écrit, rien d'autre : pas de somme, pas de filtre, pas de remise à zéro
maison.

```cpp
    while (tourne) {
        NkInput.NewFrame();
        evenements.PollEvents();
        ++images;

        const int32 rawX = NkInput.MouseRawDeltaX();         // ce que l'exercice demande
        const int32 imageX = NkInput.MouseDeltaThisFrameX();  // pour comparer
        ...
    }
```

**La seconde colonne n'est pas demandée**, mais elle fait toute la démonstration :
`MouseDeltaThisFrameX()` est le delta *de cette image*, celui que `NkInput.NewFrame()`
remet à zéro. C'est lui qui dit si ma main bouge vraiment. Sans lui, on pourrait croire que
`rawDeltaX` a raison.

Le programme détecte l'arrêt tout seul et marque les vingt lignes qui suivent, pour ne pas
avoir à les compter à la main. Il s'arme d'abord : il attend d'avoir vu vingt-cinq images
de mouvement réel avant de surveiller l'immobilité — sinon il se fermerait dès le
lancement, la souris n'ayant pas encore bougé. Je l'ai appris à mes dépens : les deux
premières exécutions ont duré moins d'une seconde.

## Les vingt lignes qui suivent l'arrêt

Exécution de 13:13:51. Le mouvement se termine à l'image 44 ; à partir de la 45, ma main
ne bouge plus.

```text
img 44 : rawDeltaX=2 | deltaImageX=1                          <-- derniere image avec mouvement
img 45 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 1/20
img 46 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 2/20
img 47 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 3/20
img 48 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 4/20
img 49 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 5/20
img 50 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 6/20
img 51 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 7/20
img 52 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 8/20
img 53 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 9/20
img 54 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 10/20
img 55 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 11/20
img 56 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 12/20
img 57 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 13/20
img 58 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 14/20
img 59 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 15/20
img 60 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 16/20
img 61 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 17/20
img 62 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 18/20
img 63 : rawDeltaX=2 | deltaImageX=0   <-- APRES L'ARRET 19/20
img 64 : rawDeltaX=0 | deltaImageX=0
```

## Ce qu'elles prouvent, en une phrase

**`rawDeltaX` n'est pas une mesure mais un souvenir : il garde la dernière valeur reçue
tant qu'aucun nouvel événement brut n'arrive, si bien qu'une caméra pilotée par lui
continuerait de tourner toute seule, à vitesse constante, alors que la main ne bouge
plus.**

## Ce que les autres exécutions confirment

Le phénomène n'est pas une coïncidence de cette exécution, et il peut durer bien plus
longtemps. Dans l'exécution de 13:13:31, `rawDeltaX` est resté figé à **−1 pendant 165
images consécutives**, des images 158 à 322, soit **près de deux secondes** :

```text
img 158 : rawDeltaX=-1 | deltaImageX=0
img 159 : rawDeltaX=-1 | deltaImageX=-1
img 160 : rawDeltaX=-1 | deltaImageX=0
       ... (162 lignes identiques) ...
img 322 : rawDeltaX=-1 | deltaImageX=0
img 323 : rawDeltaX=0  | deltaImageX=0
```

Deux secondes de rotation fantôme dans un jeu, c'est un demi-tour.

Et dans l'exécution de 13:12:51, les vingt-quatre premières images affichent
`rawDeltaX=-1` avant même que j'aie touché la souris : la valeur était déjà là, héritée de
l'ouverture de la fenêtre.

## Pourquoi, d'après le code du moteur

J'ai vérifié dans `NKEvent` plutôt que de supposer. Deux fonctions se partagent l'état de
la souris :

```cpp
    // Remet a zero le delta de l'image. A appeler UNE FOIS par image, au
    // debut -- via `NkInput.NewFrame()`.
    void BeginFrame() noexcept {
        frameDeltaX = 0;
        frameDeltaY = 0;
    }

    void OnRaw(int32 rdx, int32 rdy) noexcept {
        rawDeltaX = rdx;
        rawDeltaY = rdy;
    }
```

`NewFrame()` appelle `BeginFrame()`, qui remet à zéro **`frameDelta` et rien d'autre**.
`rawDeltaX` n'est écrit que par `OnRaw()`, c'est-à-dire seulement quand un événement brut
arrive. Plus d'événement, plus d'écriture : la dernière valeur reste en place
indéfiniment. C'est ce que montrent mes vingt lignes, et c'est pourquoi la seconde colonne
tombe à zéro alors que la première ne bouge pas.

**Le moteur connaît ce piège et le nomme.** Le commentaire de `NkEventDispatcher.cpp`, au
sujet du delta d'image, dit que le pire serait « de rendre silencieusement une valeur
périmée — exactement le piège de `MouseDeltaX()` qu'on répare ici ». La réparation a été
faite pour le delta d'image ; `rawDelta`, lui, a gardé le défaut.

## Ce qu'il faut en faire

- **Ne jamais piloter quoi que ce soit avec `rawDeltaX` seul.** Sa valeur ne dit pas
  « la souris bouge de 2 », elle dit « la dernière fois qu'elle a bougé, c'était de 2 ».
- **Le remettre à zéro soi-même à chaque image**, après l'avoir lu, ou ne l'utiliser que
  lorsque `MouseDeltaThisFrameX()` est non nul. Les deux reviennent à réparer le défaut
  côté appelant.
- **Ou utiliser `MouseDeltaThisFrameX()`**, qui est armé par `NewFrame()` et retombe à
  zéro tout seul — au prix de devoir appeler `NewFrame()`, que le moteur réclame déjà à
  voix haute si on l'oublie.
- **Et se méfier du rapprochement avec l'exercice 7.** J'y concluais que le `rawDelta`
  « continue » au bord, à juste titre. Mais il continue aussi *quand plus rien ne bouge* :
  les longues séries de `rawDelta=(-1,0)` de l'exercice 7 n'étaient pas toutes du
  mouvement réel. La conclusion de l'exercice 7 reste bonne — c'est bien le delta qu'il
  faut au bord —, mais à condition de le remettre à zéro.

## Limites de ce rendu

- **Quatre exécutions, une seule machine, une seule souris.**
- **La valeur figée dépend du dernier mouvement**, donc du geste : 2 ici, −1 ailleurs.
  Rien ne dit qu'elle ne pourrait pas être beaucoup plus grande après un geste rapide.
- **Le seuil d'arrêt est arbitraire** : vingt images immobiles, soit environ un cinquième
  de seconde. Une pause plus courte n'est pas détectée comme un arrêt.
- **Le défaut est constaté sur Win32 uniquement.** Le code lu est commun à toutes les
  plateformes — `OnRaw` et `BeginFrame` sont dans `NkEventState.h` —, donc il devrait s'y
  reproduire, mais je ne l'ai pas vérifié ailleurs.
- **Rien n'est piloté.** La « rotation fantôme » est déduite des nombres, pas observée sur
  une caméra qui tourne.

## Les fichiers de ce dossier

- `c4-exo8_reponse.md` : ce compte rendu ;
- `MaFenetre.jenga` : le workspace, inchangé depuis l'exercice 1 ;
- `src/main.cpp` : la lecture brute de `rawDeltaX`, la comparaison et la détection d'arrêt ;
- `.gitignore` : `Build/`, `logs/`, les binaires et `NkentseuKit/` restent hors du dépôt.
