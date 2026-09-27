# Chapitre 03 — Exercice 6 : état contre événement

## Ce qui est demandé

Deux compteurs sur la touche Espace. Le premier s'incrémente à chaque image où la touche
est tenue, lu par `NkInput.IsKeyDown`. Le second s'incrémente à chaque `NkKeyPressEvent`.
Appuyer une seconde, relâcher, rendre les deux nombres et expliquer l'écart.

## Le montage

```cpp
    while (tourne) {
        evenements.PollEvents();
        ++tours;

        if (NkInput.IsKeyDown(NkKey::NK_SPACE))
            ++compteurEtat;         // UNE fois par tour, tant que la touche est tenue

        NkClock::Sleep((int64)10);
    }
```

```cpp
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() != NkKey::NK_SPACE) return;
        ++compteurEvenement;
        ...
    });
```

**Deux compteurs de plus**, que l'énoncé ne demande pas mais sans lesquels l'écart se
constate sans s'expliquer : l'auto-répétition du système (`NkKeyRepeatEvent`, que le
moteur distingue de l'appui) et les relâchements (`NkKeyReleaseEvent`, qui confirment
qu'un seul appui physique a eu lieu).

**Et le nombre de tours de boucle**, indispensable : sans lui, le compteur d'état est un
nombre sans unité. La boucle dort 10 ms par tour ; rien n'est dessiné, un « tour » n'est
donc pas une image rendue mais un passage dans la boucle.

## Deux exécutions

La première fois, j'ai relâché trop tôt : 241 ms au lieu d'une seconde. Plutôt que de
jeter la mesure, je l'ai gardée et j'ai fait un appui franchement long. Les deux
**encadrent** la seconde demandée, ce qui vaut mieux qu'un point unique.

### Exécution A — appui bref, 241 ms

```text
[21:19:39.327] [exo6] APPUI  #1 (etat = 0, tour 845)
[21:19:39.569] [exo6] RELACHE #1 apres 241 ms | etat = 22 | appuis = 1 | repetitions = 0
[21:19:49.953] [exo6] Duree du programme : 19827 ms, 1820 tours de boucle (91 par seconde)
[21:19:49.958] [exo6] 1) compteur ETAT      (IsKeyDown)        : 22
[21:19:49.959] [exo6] 2) compteur EVENEMENT (NkKeyPressEvent)  : 1
[21:19:49.960] [exo6]    pour memoire, auto-repetitions        : 0
[21:19:49.960] [exo6]    pour memoire, relachements            : 1
```

### Exécution B — appui long, 6939 ms

```text
[21:28:59.760] [exo6] APPUI  #1 (etat = 0, tour 913)
[21:29:06.700] [exo6] RELACHE #1 apres 6939 ms | etat = 622 | appuis = 1 | repetitions = 213
[21:29:50.192] [exo6] Duree du programme : 60463 ms, 5541 tours de boucle (91 par seconde)
[21:29:50.198] [exo6] 1) compteur ETAT      (IsKeyDown)        : 622
[21:29:50.199] [exo6] 2) compteur EVENEMENT (NkKeyPressEvent)  : 1
[21:29:50.199] [exo6]    pour memoire, auto-repetitions        : 213
[21:29:50.200] [exo6]    pour memoire, relachements            : 1
```

## Les deux nombres

| | Durée de l'appui | **État** (`IsKeyDown`) | **Événement** (`NkKeyPressEvent`) | Rapport |
|---|---|---|---|---|
| A | 241 ms | **22** | **1** | 22 × |
| B | 6939 ms | **622** | **1** | 622 × |

Un appui, deux réponses : 22 et 1, puis 622 et 1. Le second compteur ne bouge pas d'une
exécution à l'autre ; le premier est multiplié par vingt-huit.

## L'explication de l'écart

### Le compteur d'état n'est pas un compteur d'appuis, c'est un chronomètre

Il ne compte pas des actions : il compte **les tours de boucle pendant lesquels la touche
se trouvait enfoncée**. Divisons :

```text
A :  241 ms / 22  = 10,95 ms par tour
B : 6939 ms / 622 = 11,16 ms par tour
```

Les deux donnent la même valeur, et cette valeur est la période de ma boucle — 91 tours
par seconde, soit 11 ms, ce que le programme rapporte par ailleurs. Autrement dit :

```text
compteur d'etat  =  duree de l'appui  /  periode de la boucle
```

Ce n'est pas une propriété de la touche, c'est une propriété de **ma boucle**. Si je
retirais le `Sleep(10)`, la boucle tournerait mille fois plus vite et le même appui
donnerait des dizaines de milliers. Le nombre 622 ne décrit pas mon doigt : il décrit
mon doigt *mesuré avec cette boucle-là*.

### Le compteur d'événements compte ce qui est arrivé

Il vaut 1 dans les deux cas, parce qu'il s'est produit un seul appui physique — ce que le
compteur de relâchements confirme, à 1 lui aussi. Il ne dépend ni de la durée de l'appui,
ni de la vitesse de la boucle. Si le programme avait été gelé une seconde entière, comme
à l'exercice 2, l'événement aurait attendu dans la file et aurait été compté quand même,
une seule fois, au dépilement suivant.

**Les deux compteurs ne répondent pas à la même question.** L'état répond à « la touche
est-elle enfoncée *maintenant* ? », l'événement à « à quel moment cela a-t-il
*commencé* ? ». L'écart entre 622 et 1 n'est pas une erreur de l'un des deux : c'est la
distance entre une durée et un fait.

### Les 213 auto-répétitions, et la question qu'elles tranchent

L'exécution A donnait zéro répétition, ce qui ne prouvait rien : l'en-tête du moteur
prévient que le système n'en déclenche qu'après un délai initial d'environ 500 ms, et mon
appui n'avait duré que 241 ms. L'exécution B en compte **213**.

Cela répond à une question que je m'étais posée : où va l'auto-répétition du clavier ?
**Elle ne pollue pas `NkKeyPressEvent`**, qui reste à 1. Le dorsal Win32 la range bien
dans `NkKeyRepeatEvent`, comme l'en-tête l'annonce. Un programme qui compte les appuis ne
compte donc pas les répétitions du système — c'est le comportement souhaitable, et il est
vérifié.

Ordre de grandeur : 213 répétitions pendant un appui de 6939 ms font une répétition toutes
les 30 ms environ, soit une trentaine par seconde. C'est cohérent avec les réglages
habituels de Windows.

### Ce que cela donnerait pour l'appui d'une seconde demandé

Aucune de mes deux mesures ne dure exactement une seconde. À partir des deux, on peut
l'estimer, et je le donne comme une **extrapolation**, pas comme une mesure : environ
**90 pour l'état** (1000 ÷ 11,16), **1 pour l'événement**, et **une quinzaine de
répétitions** (le délai initial en mange la moitié).

## Quand se servir de l'un, quand de l'autre

- **L'état** pour tout ce qui dure tant que le doigt est là : avancer, viser, tenir une
  gâchette, charger une attaque. La question est « est-ce tenu en ce moment ? ».
- **L'événement** pour tout ce qui doit arriver une fois par appui : sauter, valider,
  tirer un coup, ouvrir un menu. Lire `IsKeyDown` pour sauter ferait sauter le personnage
  622 fois pour un appui de sept secondes.
- **Attention au piège inverse** : un déplacement fondé sur les événements avancerait par
  à-coups, une fois à l'appui puis au rythme de l'auto-répétition, avec un trou d'une
  demi-seconde au milieu — et ce rythme dépend des réglages de l'utilisateur, pas du jeu.
- **Un compteur d'état n'est comparable qu'à lui-même.** Deux machines dont les boucles ne
  tournent pas à la même vitesse ne donneront pas le même nombre pour le même geste. Pour
  qu'il veuille dire quelque chose, il faut le diviser par la cadence — ou compter du
  temps plutôt que des tours.

## Limites de ce rendu

- **Aucun appui d'exactement une seconde.** 241 ms et 6939 ms encadrent la valeur
  demandée ; le chiffre pour une seconde est déduit, pas mesuré.
- **Un seul appui par exécution**, et une seule machine.
- **Le délai initial et la fréquence de répétition ne peuvent pas être séparés par une
  mesure unique.** 213 répétitions en 6939 ms sont compatibles avec « aucun délai et
  30,7 par seconde » comme avec « 500 ms de délai et 33 par seconde ». Il faudrait
  plusieurs appuis de durées différentes : le nombre de répétitions en fonction de la
  durée donnerait la fréquence par sa pente et le délai par son origine. L'exécution A
  fournit déjà une borne — le délai dépasse 241 ms, puisqu'elle n'a rien compté.
- **La cadence de la boucle est celle du `Sleep(10)`**, pas celle d'un vrai rendu. Avec un
  affichage à 60 ou 90 images par seconde, le compteur d'état donnerait d'autres nombres
  pour le même geste.

## Les fichiers de ce dossier

- `c4-exo6_reponse.md` : ce compte rendu ;
- `MaFenetre.jenga` : le workspace, inchangé depuis l'exercice 1 ;
- `src/main.cpp` : les quatre compteurs et le comptage des tours de boucle ;
- `.gitignore` : `Build/`, `logs/`, les binaires et `NkentseuKit/` restent hors du dépôt.
