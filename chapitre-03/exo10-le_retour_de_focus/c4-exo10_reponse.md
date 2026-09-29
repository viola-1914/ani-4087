# Chapitre 03 — Exercice 10 : le retour de focus

## Ce qui est demandé

Avec l'accumulateur en place, retirer la remise à zéro quand la fenêtre n'a pas le focus,
puis la remettre. Dans les deux cas : cliquer ailleurs, bouger la souris dix secondes,
revenir. Rendre ce qu'on observe, et le rapprocher du commentaire du simulateur.

## Le montage

Les deux versions sont dans le même programme, séparées par une touche, pour que la
comparaison porte sur la même machine et la même main.

```cpp
        if (aLeFocus || viderHorsFocus) {
            corrigeX = accumuleX;
            accumuleX = 0;          // PRENDRE ET VIDER
            accumuleY = 0;
        }
        // MODE A sans focus : on ne touche a rien. L'accumulateur grossit.
```

**Mode A** — la version dont on retire la remise à zéro : sans focus, on ne consomme pas,
et le total grossit dans son coin. **Mode B** — la remise à zéro remise en place : on
consomme à chaque image, focus ou non ; le total est jeté, mais il est vidé.

Le journal enregistre la perte et le retour du focus, et affiche à ce moment-là deux
choses : ce que la main a réellement parcouru dehors, et ce que l'accumulateur contient.

## Ce que j'observe

Trois exécutions, chacune avec un épisode en mode A et un en mode B.

| Mode | Durée hors focus | Chemin parcouru dehors | Accumulateur au retour | Effet |
|---|---|---|---|---|
| **A** | 48,1 s | 18 133 px | **555** | consommé en **une seule image** |
| **A** | 22,2 s | 15 177 px | **−309** | consommé en une seule image |
| **A** | 17,2 s | 7 188 px | **648** | consommé en une seule image |
| **B** | 31,6 s | 26 884 px | **0** | rien |
| **B** | 12,7 s | 7 105 px | **0** | rien |
| **B** | 16,3 s | 8 221 px | **0** | rien |

### Mode A : la décharge

```text
===== FOCUS RETROUVE (mode A) apres 17199 ms hors focus =====
la main a parcouru 7188 px dehors ; l'accumulateur contient 648
img 1976 : consomme 648
```

La ligne suivante est la première image après le retour du focus, et elle délivre **648
pixels d'un coup**. Les images ordinaires de la même exécution consomment 2, 4, 6,
parfois 20 : cette image-là en vaut **130**. Même chose dans les deux autres essais, avec
555 et −309.

### Mode B : rien du tout

```text
===== FOCUS RETROUVE (mode B) apres 16269 ms hors focus =====
la main a parcouru 8221 px dehors ; l'accumulateur contient 0
```

Le retour est silencieux, alors que la main avait parcouru **8221 pixels** dehors. Les
images qui suivent reprennent aux valeurs habituelles, −2, −4, −6. Aucune trace des
seize secondes passées ailleurs.

**Une seule ligne de code sépare les deux tableaux**, et ses effets ne se voient qu'au
moment précis où l'utilisateur revient.

## Ce que les nombres disent de plus

**Le saut n'est pas le chemin parcouru, c'est le déplacement net.** 648 pixels de saut
pour 7188 pixels parcourus : **9 %**. Sur les trois essais, entre 2 % et 9 %. La raison
est dans le signe : l'accumulateur additionne des deltas signés, donc un aller-retour
s'annule, tandis que mon compteur de chemin additionne des valeurs absolues. J'ai bougé
la souris dans tous les sens pendant une quarantaine de secondes, et l'essentiel s'est
compensé.

**Cela rend le défaut plus vicieux, pas moins.** Le saut dépend de *ce que la main a fait*
dehors, pas de combien de temps elle est restée. Un utilisateur qui déplace sa souris de
gauche à droite au hasard produira un petit saut ; celui qui la pousse franchement d'un
bord de l'écran à l'autre pour cliquer sur une autre fenêtre produira un saut proche du
trajet complet. Le pire cas n'est pas le plus long, c'est le plus rectiligne.

**Le temps hors focus n'explique rien non plus.** 48 secondes donnent 555, 17 secondes
donnent 648. Rien ne s'écoule, rien ne se dissipe : le total attend, aussi longtemps qu'il
faut.

## Le rapprochement avec le simulateur

Le commentaire est dans `NkXrSimulatorBackend.cpp`, au début de `WaitFrame()` :

```cpp
    // Consommer l'accumulateur souris À CHAQUE WaitFrame, même quand
    // on n'en fait rien : sinon les mouvements faits hors FOCUSED
    // frappent d'un coup au retour du focus.
    const int32 rawDX = mAccumRawDX;
    const int32 rawDY = mAccumRawDY;
    mAccumRawDX = 0;
    mAccumRawDY = 0;
```

Le simulateur fait donc le **mode B**, et il le fait *avant* de décider s'il s'en servira.
La lecture des entrées, elle, n'a lieu que quelques lignes plus bas, et seulement en état
`FOCUSED` :

```cpp
    if (!mFixedPose && mDesc.window != nullptr && mState == NkXrSessionState::NK_XR_STATE_FOCUSED) {
        mYawRad -= float32(rawDX) * kMouseSensitivityRad;
```

**Les deux gestes sont séparés, et c'est tout l'enseignement.** « Ignorer une entrée » et
« ne pas la consommer » se ressemblent dans l'intention et n'ont rien à voir dans les
faits : la première jette, la seconde accumule. Mon mode A ignorait l'entrée sans la
consommer ; le simulateur consomme d'abord, ignore ensuite.

**Et « frappent d'un coup » est littéral.** Dans le simulateur, ce total passe directement
dans `mYawRad -= rawDX * kMouseSensitivityRad` : mes 648 pixels deviendraient une rotation
instantanée de la tête, sur une seule image, dans un casque. C'est le genre de
téléportation visuelle qui provoque le mal des transports — un défaut de confort, pas
seulement de justesse.

**Une règle générale, qui dépasse la souris.** Tout tampon alimenté par un rappel et lu
par une boucle doit être vidé à chaque tour, même quand on n'en fait rien. Sinon il ne
disparaît pas : il attend.

## Un détail relevé au passage

À chaque perte de focus, le journal affiche la ligne **deux fois**, à une ou deux
millisecondes d'intervalle :

```text
[05:19:10.387] ===== FOCUS PERDU (mode B) : accumulateur = 0 =====
[05:19:10.393] ===== FOCUS PERDU (mode B) : accumulateur = 0 =====
```

C'est le cas des six pertes de focus de mes trois exécutions. Le retour, lui, n'est
annoncé qu'une fois. Le dorsal Win32 semble donc émettre `NkWindowFocusLostEvent` en
double. Sans conséquence pour cet exercice — le rappel est idempotent — mais un rappel qui
compterait quelque chose, ou qui mettrait un jeu en pause avec un compteur, verrait
double. Je le signale sans l'expliquer : je n'ai pas ouvert le dorsal pour vérifier d'où
vient le doublon.

## Limites de ce rendu

- **Trois exécutions, une seule machine, une seule souris.**
- **Les durées hors focus ne sont pas égales** entre les modes (48 s contre 32 s, 22 s
  contre 13 s). Cela ne change pas la conclusion — le mode B donne zéro quelle que soit
  la durée — mais les lignes du tableau ne sont pas des paires exactes.
- **Le « chemin parcouru » est une somme de valeurs absolues sur X seulement.** Il
  surestime le déplacement réel de la main et ignore Y.
- **Le doublon de `NkWindowFocusLostEvent` est constaté, pas expliqué.**
- **Rien n'est piloté.** La rotation de caméra et le mal des transports sont déduits du
  code du simulateur et de mes nombres, pas observés dans un casque.

## Les fichiers de ce dossier

- `c4-exo10_reponse.md` : ce compte rendu ;
- `MaFenetre.jenga` : le workspace, inchangé depuis l'exercice 1 ;
- `src/main.cpp` : l'accumulateur, la consommation conditionnelle au focus, et la bascule
  entre les deux modes ;
- `.gitignore` : `Build/`, `logs/`, les binaires et `NkentseuKit/` restent hors du dépôt.
