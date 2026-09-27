# Chapitre 03 — Exercice 5 : la taille qui change

## Ce qui est demandé

Écouter `NkWindowResizeEvent` et afficher la nouvelle taille à chaque changement.
Redimensionner lentement, puis d'un coup. Rendre les deux séries de nombres, et dire ce
qu'on en conclut sur le nombre d'événements reçus.

## Le montage

Chaque ligne du journal porte quatre choses, pour que les séries soient comparables : le
numéro de l'événement **dans le geste en cours**, la nouvelle taille, l'ancienne — que
l'événement transporte aussi — et l'écart en millisecondes depuis l'événement précédent.

Le geste lui-même est encadré. Le dorsal Win32 émet `NkWindowResizeBeginEvent` quand on
attrape un bord et `NkWindowResizeEndEvent` quand on le lâche ; le programme les écoute,
remet le compteur à zéro au début et annonce le total à la fin. On compte donc **par
geste**, pas en vrac.

```cpp
evenements.AddEventCallback<NkWindowResizeEvent>([&](NkWindowResizeEvent *e) {
    const float64 maintenant = NkChrono::Now().milliseconds;
    const int64 ecart = (int64)(maintenant - precedent);
    precedent = maintenant;
    ++total; ++dansLeGeste;

    const char *sens = e->GotLarger() ? "+" : (e->GotSmaller() ? "-" : "=");
    logger.Info("[exo5] geste {0} evt {1} : {2}x{3} (avant {4}x{5}) {6} apres {7} ms",
                geste, dansLeGeste, e->GetWidth(), e->GetHeight(),
                e->GetPrevWidth(), e->GetPrevHeight(), sens, ecart);
});
```

**J'ai fait deux exécutions**, donc quatre gestes. La première a produit peu
d'événements ; la seconde, avec des déplacements bien plus amples, en a produit beaucoup
plus. C'est la comparaison des quatre qui donne la réponse, et elle n'est pas celle que
j'attendais après la première.

## Exécution A — les deux séries complètes

### Geste 1, tiré lentement

Début à 20:42:52,961, fin à 20:43:05,182 : **12,2 secondes, 63 événements.**

```text
geste 1 evt  1 : 1297x759 (avant 1280x720) + apres 1138 ms
geste 1 evt  2 : 1281x720 (avant 1281x720) = apres    3 ms
geste 1 evt  3 : 1300x759 (avant 1281x720) + apres   18 ms
geste 1 evt  4 : 1284x720 (avant 1284x720) = apres    1 ms
geste 1 evt  5 : 1301x759 (avant 1284x720) + apres   45 ms
geste 1 evt  6 : 1285x720 (avant 1285x720) = apres    1 ms
geste 1 evt  7 : 1302x759 (avant 1285x720) + apres  230 ms
geste 1 evt  8 : 1286x720 (avant 1286x720) = apres    0 ms
geste 1 evt  9 : 1303x759 (avant 1286x720) + apres   34 ms
geste 1 evt 10 : 1287x720 (avant 1287x720) = apres    2 ms
geste 1 evt 11 : 1305x759 (avant 1287x720) + apres  197 ms
geste 1 evt 12 : 1289x720 (avant 1289x720) = apres    2 ms
geste 1 evt 13 : 1306x759 (avant 1289x720) + apres   28 ms
geste 1 evt 14 : 1290x720 (avant 1290x720) = apres    2 ms
geste 1 evt 15 : 1307x759 (avant 1290x720) + apres   36 ms
geste 1 evt 16 : 1291x720 (avant 1291x720) = apres    1 ms
geste 1 evt 17 : 1309x759 (avant 1291x720) + apres  136 ms
geste 1 evt 18 : 1293x720 (avant 1293x720) = apres    3 ms
geste 1 evt 19 : 1310x759 (avant 1293x720) + apres  195 ms
geste 1 evt 20 : 1294x720 (avant 1294x720) = apres    2 ms
geste 1 evt 21 : 1311x759 (avant 1294x720) + apres  110 ms
geste 1 evt 22 : 1295x720 (avant 1295x720) = apres    2 ms
geste 1 evt 23 : 1312x759 (avant 1295x720) + apres   63 ms
geste 1 evt 24 : 1296x720 (avant 1296x720) = apres    2 ms
geste 1 evt 25 : 1313x759 (avant 1296x720) + apres    5 ms
geste 1 evt 26 : 1297x720 (avant 1297x720) = apres    2 ms
geste 1 evt 27 : 1314x759 (avant 1297x720) + apres   38 ms
geste 1 evt 28 : 1298x720 (avant 1298x720) = apres    2 ms
geste 1 evt 29 : 1315x759 (avant 1298x720) + apres  173 ms
geste 1 evt 30 : 1299x720 (avant 1299x720) = apres    3 ms
geste 1 evt 31 : 1318x759 (avant 1299x720) + apres  196 ms
geste 1 evt 32 : 1302x720 (avant 1302x720) = apres    3 ms
geste 1 evt 33 : 1319x759 (avant 1302x720) + apres    4 ms
geste 1 evt 34 : 1303x720 (avant 1303x720) = apres    2 ms
geste 1 evt 35 : 1320x759 (avant 1303x720) + apres    5 ms
geste 1 evt 36 : 1304x720 (avant 1304x720) = apres    1 ms
geste 1 evt 37 : 1321x759 (avant 1304x720) + apres  119 ms
geste 1 evt 38 : 1305x720 (avant 1305x720) = apres    3 ms
geste 1 evt 39 : 1322x759 (avant 1305x720) + apres  100 ms
geste 1 evt 40 : 1306x720 (avant 1306x720) = apres    2 ms
geste 1 evt 41 : 1323x759 (avant 1306x720) + apres  126 ms
geste 1 evt 42 : 1307x720 (avant 1307x720) = apres    4 ms
geste 1 evt 43 : 1324x759 (avant 1307x720) + apres   28 ms
geste 1 evt 44 : 1308x720 (avant 1308x720) = apres    4 ms
geste 1 evt 45 : 1326x759 (avant 1308x720) + apres  388 ms
geste 1 evt 46 : 1310x720 (avant 1310x720) = apres    2 ms
geste 1 evt 47 : 1327x759 (avant 1310x720) + apres 2841 ms
geste 1 evt 48 : 1311x720 (avant 1311x720) = apres    2 ms
geste 1 evt 49 : 1328x759 (avant 1311x720) + apres  423 ms
geste 1 evt 50 : 1312x720 (avant 1312x720) = apres    4 ms
geste 1 evt 51 : 1329x759 (avant 1312x720) + apres   18 ms
geste 1 evt 52 : 1313x720 (avant 1313x720) = apres    2 ms
geste 1 evt 53 : 1330x759 (avant 1313x720) + apres  342 ms
geste 1 evt 54 : 1314x720 (avant 1314x720) = apres    2 ms
geste 1 evt 55 : 1331x759 (avant 1314x720) + apres 1423 ms
geste 1 evt 56 : 1315x720 (avant 1315x720) = apres    3 ms
geste 1 evt 57 : 1332x759 (avant 1315x720) + apres  590 ms
geste 1 evt 58 : 1316x720 (avant 1316x720) = apres    3 ms
geste 1 evt 59 : 1333x759 (avant 1316x720) + apres   36 ms
geste 1 evt 60 : 1317x720 (avant 1317x720) = apres    2 ms
geste 1 evt 61 : 1334x759 (avant 1317x720) + apres  461 ms
geste 1 evt 62 : 1318x720 (avant 1318x720) = apres    2 ms
geste 1 evt 63 : 1334x759 (avant 1318x720) + apres 2514 ms
--- GESTE 1 : fin, 63 evenements ---
```

### Geste 2, tiré d'un coup

Début à 20:43:14,745, fin à 20:43:16,399 : **1,65 seconde, 7 événements.**

```text
geste 2 evt  1 : 1335x759 (avant 1318x720) + apres  240 ms
geste 2 evt  2 : 1319x720 (avant 1319x720) = apres    3 ms
geste 2 evt  3 : 1337x759 (avant 1319x720) + apres   10 ms
geste 2 evt  4 : 1321x720 (avant 1321x720) = apres    1 ms
geste 2 evt  5 : 1338x759 (avant 1321x720) + apres    8 ms
geste 2 evt  6 : 1322x720 (avant 1322x720) = apres    1 ms
geste 2 evt  7 : 1338x759 (avant 1322x720) + apres  666 ms
--- GESTE 2 : fin, 7 evenements ---

[exo5] Total : 70 evenements de redimensionnement en 2 geste(s).
```

## Exécution B — beaucoup plus de matière

Deuxième lancement, à 20:48. Cette fois j'ai tiré le bord franchement, sur une grande
distance. La série complète fait 369 lignes ; en voici les extraits qui comptent, et les
totaux.

```text
--- GESTE 1 : debut ---   (20:48:33.877)
geste 1 evt   1 : 1297x759 (avant 1280x720) + apres  529 ms
geste 1 evt   2 : 1281x720 (avant 1281x720) = apres    3 ms
geste 1 evt   9 : 1294x759 (avant 1282x720) + apres    4 ms
geste 1 evt  10 : 1278x720 (avant 1278x720) = apres    1 ms
geste 1 evt  11 : 1290x759 (avant 1278x720) + apres    6 ms
geste 1 evt  12 : 1274x720 (avant 1274x720) = apres    1 ms
       ...  (la cadence tient 5 a 7 ms pendant des centaines de lignes)  ...
geste 1 evt 299 : 1028x720 (avant 1028x720) = apres    3 ms
geste 1 evt 300 : 1043x759 (avant 1028x720) + apres  187 ms
geste 1 evt 301 : 1027x720 (avant 1027x720) = apres    2 ms
--- GESTE 1 : fin, 301 evenements ---   (20:48:37.580)

--- GESTE 2 : debut ---   (20:48:42.660)
geste 2 evt   1 : 1044x759 (avant 1027x720) + apres 1352 ms
geste 2 evt   2 : 1028x720 (avant 1028x720) = apres    2 ms
       ...
geste 2 evt  67 : 1096x759 (avant 1081x720) + apres  188 ms
geste 2 evt  68 : 1080x720 (avant 1080x720) = apres    2 ms
--- GESTE 2 : fin, 68 evenements ---   (20:48:45.668)

[exo5] Total : 369 evenements de redimensionnement en 2 geste(s).
```

## Les quatre gestes, côte à côte

| | Durée | Événements | Par seconde | Largeur client | Distance | Couples | Pixels par couple |
|---|---|---|---|---|---|---|---|
| A · geste 1 | 12,22 s | 63 | 5,2 | 1281 → 1318 | 37 px | 31 | **1,19** |
| A · geste 2 | 1,65 s | 7 | 4,2 | 1319 → 1322 | 3 px | 3 | **1,00** |
| B · geste 1 | 3,70 s | 301 | 81,3 | 1281 → 1027 | 254 px | 150 | **1,69** |
| B · geste 2 | 3,01 s | 68 | 22,6 | 1028 → 1080 | 52 px | 34 | **1,53** |

## Ce que je conclus sur le nombre d'événements

### 1. Ni la durée, ni la vitesse n'expliquent le nombre

Les deux premières colonnes se contredisent d'un geste à l'autre. Le geste A1 a duré
**12,2 secondes** et n'a produit que **63** événements ; le geste B1 a duré **3,7
secondes** et en a produit **301**, cinq fois plus en trois fois moins de temps. La
cadence va de 4,2 à 81,3 par seconde, soit un facteur **19** entre deux gestes du même
programme sur la même machine.

Après la seule exécution A, j'aurais conclu que le geste lent produit plus d'événements
que le rapide. L'exécution B montre que c'était une coïncidence : mon geste « lent »
était surtout **immobile**, et mon geste « d'un coup » très **court**.

### 2. Ce qui explique le nombre, c'est la distance parcourue

La dernière colonne, elle, tient : **1,00 · 1,19 · 1,53 · 1,69 pixel par couple
d'événements**, sur quatre gestes qui n'ont en commun ni durée, ni vitesse, ni sens —
B1 rétrécit la fenêtre, les trois autres l'agrandissent.

Autrement dit, la fenêtre émet **un couple d'événements à peu près à chaque pixel
traversé par le pointeur**. Ce ne sont ni le temps qui passe ni la vitesse de la main qui
déclenchent l'émission, mais le franchissement d'une nouvelle position. Un geste ample
en franchit beaucoup ; un geste hésitant, même long, très peu.

Les trous du geste A1 le confirment par l'absurde : 2841 ms, 2514 ms, 1423 ms sans un seul
événement. Ma main tenait le bord sans bouger. Le temps passait, rien n'était émis.

### 3. La moitié des événements ne dit rien de nouveau

Les lignes vont par paires, et la seconde de chaque paire porte `avant` **égal** à la
nouvelle taille : rien n'a changé entre les deux. `NkWindowEvent.h` a un nom pour cela —
`NK_NOT_CHANGE`, « dimensions inchangées (événement de confirmation) ».

Sur l'exécution A, **34 des 70 événements**, soit 49 %. Un programme qui reconstruirait sa
chaîne d'images à chaque événement ferait donc **deux fois le travail**, dont la moitié
pour rien.

### 4. Les deux lignes d'une paire ne parlent pas de la même chose

C'est la découverte de l'exercice, et je ne l'attendais pas. Dans chaque paire, la
première ligne annonce une taille plus grande que la seconde. J'ai mesuré l'écart sur les
**35 paires de l'exécution A** : il vaut **(16, 39) à chaque fois**, sans une exception.
Les extraits de l'exécution B donnent les mêmes 16 et 39.

```text
geste 1 evt  1 : 1297x759      geste 1 evt 61 : 1334x759
geste 1 evt  2 : 1281x720      geste 1 evt 62 : 1318x720
                 ↑ 16 de moins en largeur, 39 en hauteur — toujours
```

Ces deux nombres ne sortent pas de nulle part. Le wiki du module les cite comme la
signature d'un défaut célèbre du dépôt : « la fenêtre des éditeurs grossissait de **+16 px
en largeur et +39 en hauteur à chaque lancement** ». C'est l'épaisseur du cadre Win32 :
bordure et barre de titre. Le contrat du module est pourtant clair — `width`/`height`
désignent la zone **client**, la surface où l'on dessine, et c'est aussi la taille de la
chaîne d'images.

Mon interprétation, que je donne pour ce qu'elle est : **une des deux lignes rapporte la
taille de la fenêtre cadre compris, l'autre la taille client**. Les valeurs à 759 de
hauteur ne peuvent pas être une zone client, puisque j'ai demandé 720 au départ et que
c'est bien 720 qui apparaît dans l'autre ligne.

Conséquence directe : **la colonne « sens » n'est pas fiable**. Le `+` des lignes à 759
vient de `GotLarger()`, qui compare une taille cadre à une taille client — 16 et 39 pixels
d'écart, mécaniquement. La preuve est dans l'exécution B, où la fenêtre **rétrécissait**
de 1281 à 1027 pixels : la moitié des lignes affichent quand même `+`, du début à la fin.
Comparer deux nombres exprimés dans deux unités différentes donne toujours le même
résultat, et ce résultat ne veut rien dire.

### 5. L'appariement n'est pas garanti

L'exécution B contient trois endroits où deux lignes « cadre » se suivent sans ligne
« client » entre elles :

```text
geste 1 evt 264 : 1070x720 (avant 1070x720) = apres   1 ms
geste 1 evt 265 : 1086x759 (avant 1070x720) + apres 208 ms
geste 1 evt 266 : 1085x759 (avant 1070x720) + apres  63 ms
geste 1 evt 267 : 1069x720 (avant 1069x720) = apres   4 ms
```

Un code qui se fierait à une alternance stricte — « une ligne sur deux est la bonne » — se
tromperait ici. C'est une raison de plus de comparer la taille reçue à celle qu'on détient
déjà, plutôt que de compter les événements.

### 6. Ce qu'il faut en faire quand on écrit du vrai code

Trois règles, tirées directement de ces nombres.

- **Ne jamais faire de travail coûteux dans le rappel de redimensionnement.** Il peut être
  appelé **301 fois en 3,7 secondes**, soit une fois toutes les 12 millisecondes — le
  budget d'une image entière de casque.
- **Comparer avant d'agir.** La moitié des événements annoncent une taille déjà connue, et
  l'alternance n'est pas garantie.
- **Se servir de `Begin` et `End`.** Ils existent pour ça : on retient la dernière taille
  reçue pendant le geste, et on ne reconstruit qu'à la fin. Sur le geste B1, cela ferait
  **1 reconstruction au lieu de 301**.

## Limites de ce rendu

- **Deux exécutions, quatre gestes, une seule machine, une seule souris.** La valeur « un
  couple par pixel » dépend de la cadence de rapport du pointeur et de la résolution de
  l'écran ; elle n'a pas été éprouvée ailleurs.
- **La distance est mesurée sur la largeur seule.** Je tirais un bord vertical, la hauteur
  n'a jamais changé. Un geste en coin, qui modifie les deux dimensions, donnerait peut-être
  un autre rapport.
- **L'interprétation du (16, 39) est une déduction, pas une lecture du code.** Elle
  s'appuie sur la constance parfaite de l'écart et sur les deux nombres cités par le wiki ;
  je n'ai pas ouvert le dorsal Win32 pour voir quelle ligne émet quoi.
- **La série complète de l'exécution B n'est pas reproduite** : 369 lignes, dont seuls les
  extraits significatifs figurent ici. Les totaux et les bornes, eux, sont ceux du journal.
- **Rien n'est dessiné.** Les conséquences sur la chaîne d'images sont raisonnées, pas
  mesurées : ce programme n'a pas de rendu à reconstruire.

## Les fichiers de ce dossier

- `c4-exo5_reponse.md` : ce compte rendu ;
- `MaFenetre.jenga` : le workspace, inchangé depuis l'exercice 1 ;
- `src/main.cpp` : les rappels `Begin` / `Resize` / `End`, le comptage et le chronométrage ;
- `.gitignore` : `Build/`, `logs/`, les binaires et `NkentseuKit/` restent hors du dépôt.
