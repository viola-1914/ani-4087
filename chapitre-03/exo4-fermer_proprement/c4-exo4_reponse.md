# Chapitre 03 — Exercice 4 : fermer proprement

## Ce qui est demandé

Ajouter un rappel sur `NkWindowCloseEvent` qui met un booléen à faux, et faire porter la
boucle sur ce booléen plutôt que sur `IsOpen()`. Ajouter un rappel sur `NkKeyPressEvent`
qui fait la même chose sur Échap. Rendre le code, et expliquer pourquoi les deux chemins
de sortie doivent aboutir au même endroit.

## Le code

```cpp
    bool tourne = true;                 // LA decision du programme
    const char *raison = "aucune";      // par ou la sortie a ete demandee

    NkEventSystem &evenements = NkEvents();

    // Chemin 1 : la croix de la barre de titre.
    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) {
        raison = "la croix (NkWindowCloseEvent)";
        tourne = false;                 // on NOTE la demande, on ne ferme pas ici
    });

    // Chemin 2 : la touche Echap.
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) {
            raison = "la touche Echap (NkKeyPressEvent)";
            tourne = false;             // exactement la meme ligne que ci-dessus
        }
    });

    while (tourne) {                    // et NON : while (tourne && fenetre.IsOpen())
        evenements.PollEvents();
        NkClock::Sleep((int64)10);
    }

    // ---- LE SEUL CHEMIN DE SORTIE -----------------------------------------
    logger.Info("[exo4] Sortie demandee par : {0}", raison);
    fenetre.Close();
    logger.Info("[exo4] Fenetre fermee, terminaison propre. Code 0.");
    // ------------------------------------------------------------------------
    return 0;
```

La variable `raison` n'est pas demandée par l'énoncé : je l'ai ajoutée pour **prouver** que
les deux chemins aboutissent au même endroit, au lieu de l'affirmer. Elle enregistre par où
la demande est venue ; la suite est commune.

## Les deux exécutions

**Premier lancement — fermeture par la croix :**

```text
[2026-09-27 20:24:59.828] [INF] [main.cpp:52] -> [exo4] Boucle demarree. Fermez par la croix OU par Echap.
[2026-09-27 20:25:28.471] [INF] [main.cpp:61] -> [exo4] Sortie demandee par : la croix (NkWindowCloseEvent)
[2026-09-27 20:25:28.494] [INF] [main.cpp:63] -> [exo4] Fenetre fermee, terminaison propre. Code 0.

  ◀  FIN D'EXECUTION  —  termine normalement  (28.95s)
```

**Second lancement — fermeture par Échap :**

```text
[2026-09-27 20:25:47.109] [INF] [main.cpp:52] -> [exo4] Boucle demarree. Fermez par la croix OU par Echap.
[2026-09-27 20:25:50.632] [INF] [main.cpp:61] -> [exo4] Sortie demandee par : la touche Echap (NkKeyPressEvent)
[2026-09-27 20:25:50.670] [INF] [main.cpp:63] -> [exo4] Fenetre fermee, terminaison propre. Code 0.

  ◀  FIN D'EXECUTION  —  termine normalement  (3.65s)
```

Ce qu'il faut lire dans ces deux traces :

- **La ligne du milieu diffère** : la demande n'est pas venue du même endroit.
- **La dernière ligne est identique, au numéro de ligne près** : `main.cpp:63` dans les
  deux cas. Ce n'est pas la même formulation répétée à deux endroits du code, c'est **la
  même ligne de code**, exécutée deux fois.
- **`termine normalement` dans les deux cas.** À comparer à l'exercice 2, où le processus
  s'achevait sur le code 0xCFFFFFFF d'une terminaison forcée. Ici, le programme décide de
  s'arrêter et rend 0.
- **23 ms d'un côté, 38 ms de l'autre**, entre la demande et la fermeture effective : la
  fermeture n'est pas instantanée, elle passe par le système.

## Pourquoi les deux chemins doivent aboutir au même endroit

**1. Parce que ce qu'il y a à faire en partant ne dépend pas de la façon de partir.**
Fermer la fenêtre, libérer les ressources, enregistrer le travail en cours, prévenir un
serveur : rien de tout cela ne change selon qu'on a cliqué sur une croix ou appuyé sur une
touche. Écrire ces gestes une fois, après la boucle, c'est la seule manière de garantir
qu'ils sont les mêmes. Deux sorties, deux blocs de fermeture — et le jour où l'on en
modifie un, l'autre est déjà faux.

**2. Parce qu'un troisième chemin arrivera.** Un menu « Quitter », un raccourci Ctrl+Q,
une commande reçue par le réseau, un arrêt demandé par le système. Avec cette structure,
chacun coûte trois lignes : un rappel de plus qui pose le même booléen. Avec des sorties
qui ferment chacune de leur côté, chacun coûte une copie du bloc de fermeture — et un
oubli possible.

**3. Parce qu'un rappel n'est pas un bon endroit pour fermer quoi que ce soit.** Il est
appelé depuis `PollEvents()`, c'est-à-dire **au milieu** du dépilement des messages.
Détruire la fenêtre à cet instant, ce serait la retirer sous les pieds du code qui est en
train de la faire parler. Le rappel note une intention ; la boucle la constate au tour
suivant ; la fermeture a lieu quand plus rien n'est en cours.

**4. Parce que `IsOpen()` ne répond pas à la même question que `tourne`.**
`fenetre.IsOpen()` décrit l'état du monde : la fenêtre native existe-t-elle encore ?
`tourne` porte la décision du programme : ai-je encore quelque chose à faire ? Les deux ne
coïncident pas toujours.

Faire porter la boucle sur `IsOpen()` casse les deux chemins, chacun à sa manière :

- **Par Échap**, la fenêtre est toujours ouverte — c'est le clavier qui a parlé, pas le
  gestionnaire de fenêtres. La condition reste vraie, la boucle continue, et le programme
  ignore la demande. Il faudrait fermer la fenêtre *depuis le rappel* pour sortir, ce qui
  ramène au problème du point 3.
- **Par la croix**, la boucle s'arrêterait parce que la fenêtre a disparu, pas parce que
  le programme l'a décidé. La sortie devient un effet de bord. Et le code d'après n'a plus
  de fenêtre sur laquelle travailler : impossible d'y afficher une demande de confirmation
  ou d'y lire une dernière information.

**5. Parce que la sortie devient lisible.** Une seule condition, `while (tourne)`, dit tout
ce qu'il faut savoir : ce programme s'arrête quand il décide de s'arrêter. Le journal, lui,
dit pourquoi. Un lecteur n'a pas à parcourir les rappels pour reconstituer les chemins
possibles.

## Ce que j'ai remarqué en plus

**Le cache de Jenga n'a rien reconstruit entre les deux essais :**

```text
ℹ Found 1 source file(s)
✓ All files up to date
Time: 0.07s
```

1,53 s à la première construction, 0,07 s à la seconde. Aucune source n'avait changé — je
n'avais changé que ma façon de fermer la fenêtre, ce qui ne se compile pas. C'est le même
comportement qu'au chapitre 2, où il avait fallu un `jenga clean` parce que le cache ne
surveille que les fichiers sources.

## Limites de ce rendu

- **Deux chemins testés, un seul par exécution.** Je n'ai pas éprouvé ce qui se passe si
  les deux arrivent presque en même temps — Échap pressé pendant que la croix est cliquée.
  La structure devrait y résister, les deux rappels écrivant la même valeur, mais je ne
  l'ai pas vérifié.
- **Rien à libérer dans ce programme.** L'argument du point 1 porte sur ce qu'on écrira
  *plus tard* après la boucle : ici, il n'y a qu'un `Close()`. Le bénéfice se mesurera
  quand il y aura une scène, des fichiers ouverts, une sauvegarde.
- **Une seule plateforme.** Sur mobile, l'arrêt vient souvent du système et non de
  l'utilisateur ; le chemin serait un troisième rappel, pas une croix.
- **Aucune capture.** Cet exercice ne se voit pas à l'écran : la fenêtre est celle de
  l'exercice 1. Ce qu'il y a à montrer est dans le journal.

## Les fichiers de ce dossier

- `c4-exo4_reponse.md` : ce compte rendu ;
- `MaFenetre.jenga` : le workspace, inchangé depuis l'exercice 1 ;
- `src/main.cpp` : les deux rappels, la boucle sur le booléen, la fermeture unique ;
- `.gitignore` : `Build/`, `logs/`, les binaires et `NkentseuKit/` restent hors du dépôt.
