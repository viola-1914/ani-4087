# Chapitre 03 — Exercice 7 : le pointeur caché

## Ce qui est demandé

Cacher le curseur et le confiner. Afficher à chaque image la position `x`, `y` et le
`rawDelta`. Bouger la souris jusqu'au bord, rendre les deux séries, et dire laquelle
continue de bouger — et pourquoi c'est celle-là qu'il faut.

## Le montage

```cpp
    fenetre.ShowMouse(false);           // 1) cacher
    fenetre.ClipMouseToClient(true);    // 2) confiner
```

```cpp
    while (tourne) {
        NkInput.NewFrame();             // AVANT de depiler : remet les deltas d'image a zero
        evenements.PollEvents();
        ++images;

        const int32 x = NkInput.MouseX();
        const int32 y = NkInput.MouseY();
        const int32 rdx = NkInput.MouseRawDeltaX();
        const int32 rdy = NkInput.MouseRawDeltaY();
        ...
    }
```

Trois précautions, toutes prises dans le fichier d'en-tête du moteur plutôt que devinées.

**`NkInput.NewFrame()` une fois par tour, avant le dépilement.** Sans lui, les deltas
d'image ne sont jamais remis à zéro. La démo `NkDemoDeltaSouris` du moteur en fait même
une case à cocher, pour montrer ce qui se passe quand on l'oublie.

**Le confinement est relâché en sortant.** `NkWindow.h` prévient qu'il **survit au
processus** : sans le `ClipMouseToClient(false)` final, la souris resterait prisonnière
d'un rectangle après la fermeture du programme. Même chose pour la visibilité, qui est un
compteur sur Win32 : un `false` exige un `true`.

**Seules les images où quelque chose bouge sont écrites**, les autres sont comptées. À
90 images par seconde, tout écrire donnerait des milliers de lignes illisibles.

## Les deux séries

### Pendant le mouvement : les deux avancent ensemble

```text
img 108 : x=499 y=573 | rawDelta=(5,-5)
img 109 : x=514 y=559 | rawDelta=(7,-6)
img 110 : x=520 y=553 | rawDelta=(6,-6)
img 111 : x=530 y=541 | rawDelta=(5,-6)
img 112 : x=537 y=536 | rawDelta=(6,-5)
img 113 : x=548 y=521 | rawDelta=(6,-7)
```

La position suit le mouvement, le `rawDelta` le décrit. Tant qu'on est loin des bords,
les deux disent la même chose de deux façons.

### Au bord : l'une s'arrête, l'autre continue

```text
img 203 : x=1279 y=162 | rawDelta=(4,0)
img 204 : x=1279 y=162 FIGEE | rawDelta=(4,0) <-- le bord
img 205 : x=1279 y=162 FIGEE | rawDelta=(5,0) <-- le bord
img 206 : x=1279 y=162 FIGEE | rawDelta=(7,0) <-- le bord
img 207 : x=1279 y=162 FIGEE | rawDelta=(6,0) <-- le bord
img 208 : x=1279 y=162 FIGEE | rawDelta=(5,0) <-- le bord
...
img 217 : x=1279 y=162 FIGEE | rawDelta=(5,0) <-- le bord
```

Quatorze images de suite, `x` reste cloué à 1279 pendant que le `rawDelta` rapporte 5 à 7
pixels par image. **La main n'a pas cessé de bouger ; la position, elle, n'a plus rien à
dire.**

Le même phénomène, plus violent, quand je pousse fort dans le coin :

```text
img 3416 : x=1225 y=675 FIGEE | rawDelta=(19,14) <-- le bord
img 3417 : x=1225 y=675 FIGEE | rawDelta=(20,15) <-- le bord
img 3418 : x=1225 y=675 FIGEE | rawDelta=(18,15) <-- le bord
```

et, dans l'autre sens :

```text
img 4228 : x=0 y=684 FIGEE | rawDelta=(-45,7) <-- le bord
img 4229 : x=0 y=684 FIGEE | rawDelta=(-41,6) <-- le bord
img 4230 : x=0 y=684 FIGEE | rawDelta=(-38,5) <-- le bord
```

Quarante-cinq pixels de mouvement réel dans une image, et une position qui ne bouge pas
d'un seul.

## Le bilan chiffré

```text
[exo7] Images : 4582 au total, 1368 muettes, 1514 avec position FIGEE et rawDelta non nul
[exo7] Cumul des |rawDelta| : X=19615 Y=11508
[exo7] Derniere position lue : x=188 y=0
[exo7] Curseur rendu visible et confinement relache.
```

| | Valeur |
|---|---|
| Images | 4582 |
| dont sans aucun mouvement | 1368 |
| **dont position figée alors que le `rawDelta` bouge** | **1514** |
| Part des images actives où la position ment | **47 %** |
| Cumul des déplacements bruts | 19 615 px en X, 11 508 en Y |
| Largeur disponible pour la position | 1280 px |

**Deux chiffres qui résument l'exercice.** Sur les 3214 images où il s'est passé quelque
chose, **une sur deux** avait une position figée : la moitié du mouvement de ma main
n'apparaît nulle part dans la série des positions. Et le cumul des déplacements bruts en
X, 19 615 pixels, vaut **quinze fois la largeur de la fenêtre** : j'ai déplacé la souris
bien plus loin que ce que la position pouvait exprimer.

## Laquelle continue, et pourquoi c'est celle-là qu'il faut

**C'est le `rawDelta` qui continue.** La position est une coordonnée **dans** la fenêtre :
elle est bornée par construction, et le confinement la borne une seconde fois. Arrivée au
bord, elle ne peut plus qu'annoncer 1279 ou 0. Le `rawDelta`, lui, est ce que le
périphérique a rapporté — combien la souris a bougé, pas où elle est. Il n'a pas de bord,
parce qu'un mouvement n'a pas de bord.

C'est celle-là qu'il faut dès qu'on pilote une caméra, une vue à la première personne,
une rotation. Trois raisons, dont deux se lisent directement dans mes séries.

**1. Sinon, la vue se bloque en plein mouvement.** Une caméra qui tourne d'après `x`
s'arrêterait de tourner aux images 204 à 217 pendant que ma main continue. Dans un jeu,
cela veut dire : je pousse la souris, je ne peux plus me retourner. Le curseur est caché,
l'utilisateur ne voit même pas pourquoi. Avec le `rawDelta`, la vue tourne de 5, 7, 6, 5
pixels comme si de rien n'était.

**2. Sinon, on perd la moitié du mouvement.** Mes 47 % d'images figées sont autant de
mouvements réels qu'une caméra pilotée par la position aurait ignorés. Ce n'est pas un cas
limite : c'est la moitié du temps.

**3. La position dépend de l'écran, le delta non.** La position est bornée par la fenêtre,
et sa borne change avec la taille de celle-ci — ma série le montre aussi, puisque les
valeurs plafonnent différemment en X et en Y (voir plus bas). Le `rawDelta` décrit le
geste, indépendamment de la taille de la fenêtre, de la résolution et de la place du
pointeur.

**Et c'est aussi pourquoi on cache et on confine.** Les trois vont ensemble : cacher, parce
qu'un curseur visible qui reste collé dans un coin ne veut plus rien dire ; confiner, pour
que la souris ne sorte pas de la fenêtre et n'aille pas cliquer ailleurs ; lire le delta,
parce qu'une fois confinée, la position ne peut plus rendre compte du mouvement. Retirer
l'un des trois casse les deux autres.

## Une observation que je ne sais pas expliquer

La position plafonne à **1279 en X** — c'est exactement 1280 − 1, la largeur demandée. En
revanche, elle ne dépasse jamais **684 en Y**, alors que j'ai demandé une hauteur de 720 :
il manque 35 pixels.

```text
img 4190 : x=0 y=684 | rawDelta=(-21,6)
img 4191 : x=0 y=684 FIGEE | rawDelta=(-21,6) <-- le bord
```

Mon hypothèse, que je donne comme telle : la fenêtre fait 1280 × 720 de zone client, mais
mon écran est moins haut que ce que cela demande une fois le cadre ajouté, et le
confinement, qui s'applique au rectangle visible, est tronqué en bas. Le confinement
serait donc borné par l'écran, pas par la fenêtre.

Je ne l'ai pas vérifié : il faudrait journaliser la position et la taille réelles de la
fenêtre, et comparer au rectangle de confinement que le système rapporte. C'est une mesure
à faire, pas une conclusion.

## Limites de ce rendu

- **Une seule exécution, une seule machine, une seule souris.** La cadence de rapport du
  pointeur et l'accélération de Windows influencent les valeurs de `rawDelta`.
- **Le plafond vertical à 684 n'est pas expliqué**, seulement constaté, avec une hypothèse
  à vérifier.
- **Le journal n'écrit pas toutes les images**, seulement celles où quelque chose bouge.
  Les 1368 images muettes sont comptées, pas détaillées.
- **Rien n'est piloté par ces deltas.** Les conséquences sur une caméra sont raisonnées à
  partir des séries, pas mesurées sur une vue qui tourne.
- **`rawDelta` n'a pas été comparé à `MouseDeltaThisFrameX`**, qui existe aussi dans
  `NkInput` et qui pourrait se comporter différemment au bord.

## Les fichiers de ce dossier

- `c4-exo7_reponse.md` : ce compte rendu ;
- `MaFenetre.jenga` : le workspace, inchangé depuis l'exercice 1 ;
- `src/main.cpp` : le curseur caché et confiné, les compteurs, et la libération en sortie ;
- `.gitignore` : `Build/`, `logs/`, les binaires et `NkentseuKit/` restent hors du dépôt.
