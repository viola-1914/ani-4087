# Chapitre 03 — Exercice 9 : le défaut corrigé

## Ce qui est demandé

Écrire l'accumulateur : un rappel sur `NkMouseRawEvent` qui ajoute, et une consommation
par image qui prend le total et remet à zéro. Reprendre la mesure de l'exercice 8, rendre
les deux séries côte à côte.

## Le remède, en deux morceaux

**1. Le rappel qui ajoute.** Pas `=`, mais `+=` : si deux événements bruts arrivent dans
la même image, les deux comptent.

```cpp
    int32 accumuleX = 0;        // rempli par le rappel, vide par la boucle
    int32 accumuleY = 0;

    evenements.AddEventCallback<NkMouseRawEvent>([&](NkMouseRawEvent *e) {
        accumuleX += e->GetDeltaX();
        accumuleY += e->GetDeltaY();
    });
```

**2. La consommation qui prend et vide dans le même geste.** C'est elle qui corrige le
défaut : quand aucun événement n'arrive, il ne reste pas un souvenir, il reste un zéro.

```cpp
    while (tourne) {
        NkInput.NewFrame();
        evenements.PollEvents();        // c'est ici que le rappel ajoute
        ++images;

        const int32 corrigeX = accumuleX;
        accumuleX = 0;                  // PRENDRE ET VIDER
        const int32 corrigeY = accumuleY;
        accumuleY = 0;

        const int32 brutX = NkInput.MouseRawDeltaX();   // la lecture non corrigee
        ...
    }
```

Les deux valeurs sont écrites côte à côte à chaque image : `brut` est la lecture de
l'exercice 8, `corrige` est l'accumulateur consommé. Le protocole est celui de l'exercice
précédent — bouger, puis poser la main — et le programme s'arrête seul après vingt images
immobiles.

## Les deux séries côte à côte

### Pendant le mouvement : les deux disent la même chose

Exécution de 13:27:12, images 33 à 44.

```text
img 33 : brut=13 | corrige=13
img 34 : brut=11 | corrige=11
img 35 : brut=9  | corrige=9
img 36 : brut=7  | corrige=7
img 37 : brut=5  | corrige=5
img 38 : brut=4  | corrige=4
img 39 : brut=3  | corrige=3
img 40 : brut=2  | corrige=2
img 41 : brut=3  | corrige=3
img 42 : brut=3  | corrige=3
img 43 : brut=3  | corrige=3
img 44 : brut=3  | corrige=3
```

Tant que les événements arrivent, la valeur périmée **est** la valeur fraîche : les deux
colonnes coïncident. C'est ce qui rend le défaut de l'exercice 8 si discret — il ne se
voit pas quand on bouge.

### Après l'arrêt : les vingt lignes

Même exécution, images 120 à 139. La main ne bouge plus.

```text
img 120 : brut=-1 | corrige=0   <-- APRES L'ARRET 1/20
img 121 : brut=-1 | corrige=0   <-- APRES L'ARRET 2/20
img 122 : brut=-1 | corrige=0   <-- APRES L'ARRET 3/20
img 123 : brut=-1 | corrige=0   <-- APRES L'ARRET 4/20
img 124 : brut=-1 | corrige=0   <-- APRES L'ARRET 5/20
img 125 : brut=-1 | corrige=0   <-- APRES L'ARRET 6/20
img 126 : brut=-1 | corrige=0   <-- APRES L'ARRET 7/20
img 127 : brut=-1 | corrige=0   <-- APRES L'ARRET 8/20
img 128 : brut=-1 | corrige=0   <-- APRES L'ARRET 9/20
img 129 : brut=-1 | corrige=0   <-- APRES L'ARRET 10/20
img 130 : brut=-1 | corrige=0   <-- APRES L'ARRET 11/20
img 131 : brut=-1 | corrige=0   <-- APRES L'ARRET 12/20
img 132 : brut=-1 | corrige=0   <-- APRES L'ARRET 13/20
img 133 : brut=-1 | corrige=0   <-- APRES L'ARRET 14/20
img 134 : brut=-1 | corrige=0   <-- APRES L'ARRET 15/20
img 135 : brut=-1 | corrige=0   <-- APRES L'ARRET 16/20
img 136 : brut=-1 | corrige=0   <-- APRES L'ARRET 17/20
img 137 : brut=-1 | corrige=0   <-- APRES L'ARRET 18/20
img 138 : brut=-1 | corrige=0   <-- APRES L'ARRET 19/20
img 139 : brut=-1 | corrige=0   <-- APRES L'ARRET 20/20
```

**Vingt lignes, deux colonnes, deux comportements.** À gauche, le défaut de l'exercice 8 :
une valeur qui persiste, −1 à chaque image, parce que plus personne n'écrit ce champ. À
droite, le remède : zéro, parce que rien n'a été ajouté et que la consommation précédente
a vidé le total.

Le même contraste apparaît dans les quatre autres exécutions, avec d'autres valeurs
figées : `brut=2` (13:26:46), `brut=3` (13:26:41), `brut=1` (13:27:01), `brut=−2`
(13:27:01). La valeur retenue est toujours celle du dernier événement reçu ; seule la
colonne de droite retombe à zéro.

## Une seconde découverte : la lecture brute perd aussi du mouvement

Je m'attendais à ce que l'accumulateur corrige la persistance. Je ne m'attendais pas à ce
qu'il en récupère davantage. Dans toutes mes exécutions, certaines images affichent
**`corrige` nettement supérieur à `brut`** :

```text
img 31 : brut=14 | corrige=52     (13:27:12)
img 32 : brut=13 | corrige=26
img 19 : brut=12 | corrige=23     (13:27:00)
img 39 : brut=5  | corrige=25     (13:26:46)
img 51 : brut=11 | corrige=61     (13:26:01)
```

L'explication est dans le `+=`. Quand plusieurs événements bruts arrivent pendant la même
image — parce que l'image a duré plus longtemps, ou que la souris rapporte plus vite que
la boucle ne tourne — `OnRaw()` écrase à chaque fois : **la lecture brute ne garde que le
dernier**. L'accumulateur, lui, les additionne tous.

Sur l'image 51 de l'exécution de 13:26:01, la lecture brute annonce 11 pixels quand la
main en a parcouru 61 : **cinquante pixels perdus en une seule image**. Le défaut de
l'exercice 8 n'est donc pas seulement « une valeur qui traîne après l'arrêt » ; c'est
aussi « une valeur qui en oublie quatre sur cinq quand ça va vite ».

## Ce que la correction garantit

- **Pas de mouvement fantôme.** Zéro événement donne zéro, et non le dernier souvenir.
- **Aucune perte quand plusieurs événements tombent dans la même image.** Le `+=` les
  additionne au lieu de les écraser.
- **La consommation est atomique du point de vue de l'appelant** : prendre et vider dans
  le même geste. Lire sans vider ramènerait le défaut ; vider sans lire perdrait l'image.
- **C'est réparé côté appelant.** Le moteur n'est pas modifié : n'importe quel programme
  peut appliquer ce remède sans toucher à `NKEvent`. Le vrai correctif serait de vider
  `rawDelta` dans `BeginFrame()`, comme le fait déjà `frameDelta`.

## Limites de ce rendu

- **Cinq exécutions, une seule machine, une seule souris.** Les valeurs figées et les
  paquets d'événements dépendent de la cadence de rapport du pointeur.
- **Le seuil d'arrêt reste arbitraire** : vingt images immobiles, environ un cinquième de
  seconde. Les séries montrent d'ailleurs plusieurs faux arrêts, remis à zéro par un
  frémissement de la main — la colonne `corrige` les détecte, ce qui est le comportement
  voulu.
- **L'accumulateur n'est pas protégé contre le débordement.** `int32` suffit largement
  ici, mais un total jamais consommé finirait par déborder.
- **Un seul fil.** Si le rappel était appelé depuis un autre fil que la boucle, il
  faudrait protéger l'accumulateur ; ce n'est pas le cas ici, `PollEvents()` appelant les
  rappels sur le fil de la boucle.
- **Rien n'est piloté.** Comme aux exercices 7 et 8, les conséquences sur une caméra sont
  raisonnées à partir des nombres.

## Les fichiers de ce dossier

- `c4-exo9_reponse.md` : ce compte rendu ;
- `MaFenetre.jenga` : le workspace, inchangé depuis l'exercice 1 ;
- `src/main.cpp` : le rappel qui ajoute, la consommation qui vide, et la comparaison ;
- `.gitignore` : `Build/`, `logs/`, les binaires et `NkentseuKit/` restent hors du dépôt.
