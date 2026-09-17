# Exercice 12 — Vingt millisecondes senties

Pour cet exercice, j'ai écrit un petit programme sur écran ordinaire dans
lequel un cercle noir suit les mouvements de la souris avec un retard
réglable entre **0 et 200 millisecondes**.

L'objectif était d'augmenter progressivement ce retard et de demander à
cinq personnes de signaler le moment où elles commençaient à percevoir un
décalage entre leur mouvement de souris et celui du cercle affiché.

J'ai ensuite comparé les seuils obtenus au budget d'environ **20 ms**
utilisé comme référence pour le mouvement-vers-photon en réalité virtuelle.

## Programme utilisé

J'ai réalisé le programme en Python avec `tkinter`.

Le retard peut être réglé avec un curseur ou avec les boutons `+5 ms`,
`+10 ms` et `-5 ms`.

Le programme permet également d'entrer le prénom d'un participant et
d'enregistrer directement le retard auquel celui-ci commence à remarquer
le décalage.

```python
import tkinter as tk
import time
from collections import deque

historique = deque()
seuils = []

def mouvement_souris(event):
    historique.append(
        (time.perf_counter(), event.x, event.y)
    )

def mise_a_jour():
    retard_ms = retard.get()
    retard_s = retard_ms / 1000.0
    maintenant = time.perf_counter()

    while len(historique) >= 2:
        if maintenant - historique[1][0] >= retard_s:
            historique.popleft()
        else:
            break

    if historique:
        instant, x, y = historique[0]

        if maintenant - instant >= retard_s:
            rayon = 12

            canvas.coords(
                cercle,
                x - rayon,
                y - rayon,
                x + rayon,
                y + rayon
            )

    affichage_retard.config(
        text=f"Retard actuel : {retard.get()} ms"
    )

    fenetre.after(1, mise_a_jour)

def ajouter(valeur):
    nouveau = retard.get() + valeur
    nouveau = max(0, min(200, nouveau))
    retard.set(nouveau)

def remettre_zero():
    retard.set(0)
    historique.clear()

def enregistrer_seuil():
    nom = entree_nom.get().strip()

    if nom == "":
        nom = f"Personne {len(seuils) + 1}"

    valeur = retard.get()

    seuils.append((nom, valeur))

    liste_resultats.insert(
        tk.END,
        f"{nom} : seuil remarqué à {valeur} ms"
    )

    entree_nom.delete(0, tk.END)
    remettre_zero()

fenetre = tk.Tk()
fenetre.title("Exercice 12 - Vingt millisecondes senties")
fenetre.geometry("950x800")

titre = tk.Label(
    fenetre,
    text="Test de perception du retard",
    font=("Arial", 18, "bold")
)
titre.pack(pady=8)

instruction = tk.Label(
    fenetre,
    text=(
        "Déplacez rapidement la souris dans la zone blanche.\n"
        "Augmentez progressivement le retard jusqu'à ce que "
        "le participant remarque que le cercle suit moins bien."
    ),
    font=("Arial", 11)
)
instruction.pack()

retard = tk.IntVar(value=0)

affichage_retard = tk.Label(
    fenetre,
    text="Retard actuel : 0 ms",
    font=("Arial", 20, "bold")
)
affichage_retard.pack(pady=10)

curseur = tk.Scale(
    fenetre,
    from_=0,
    to=200,
    variable=retard,
    orient="horizontal",
    length=700,
    resolution=1
)
curseur.pack()

zone_boutons = tk.Frame(fenetre)
zone_boutons.pack(pady=5)

tk.Button(
    zone_boutons,
    text="-5 ms",
    command=lambda: ajouter(-5)
).pack(side="left", padx=5)

tk.Button(
    zone_boutons,
    text="+5 ms",
    command=lambda: ajouter(5)
).pack(side="left", padx=5)

tk.Button(
    zone_boutons,
    text="+10 ms",
    command=lambda: ajouter(10)
).pack(side="left", padx=5)

tk.Button(
    zone_boutons,
    text="Remettre à 0",
    command=remettre_zero
).pack(side="left", padx=5)

canvas = tk.Canvas(
    fenetre,
    width=850,
    height=350,
    bg="white"
)
canvas.pack(pady=10)

cercle = canvas.create_oval(
    413, 163,
    437, 187,
    fill="black"
)

canvas.bind("<Motion>", mouvement_souris)

zone_participant = tk.Frame(fenetre)
zone_participant.pack(pady=5)

tk.Label(
    zone_participant,
    text="Nom/prénom :"
).pack(side="left")

entree_nom = tk.Entry(
    zone_participant,
    width=25
)
entree_nom.pack(side="left", padx=5)

tk.Button(
    zone_participant,
    text="Enregistrer ce seuil",
    command=enregistrer_seuil
).pack(side="left", padx=5)

tk.Label(
    fenetre,
    text="Seuils enregistrés :",
    font=("Arial", 12, "bold")
).pack()

liste_resultats = tk.Listbox(
    fenetre,
    width=60,
    height=6,
    font=("Arial", 11)
)
liste_resultats.pack(pady=5)

mise_a_jour()
fenetre.mainloop()
```

## Vérification du fonctionnement du programme

Avant de faire passer les participants, j'ai vérifié que le réglage du
retard fonctionnait correctement.

J'ai notamment obtenu :

```text
4 clics sur +5 ms       -> Retard actuel : 20 ms
+10 ms puis -5 ms       -> Retard actuel : 25 ms
50 clics sur +10 ms     -> Retard limité à 200 ms
0 ms puis diminution    -> Retard limité à 0 ms
```

Le programme respecte donc bien l'intervalle demandé de **0 à 200 ms**.

J'ai également vérifié qu'après l'enregistrement d'un participant, son seuil
apparaissait dans la liste et que le retard revenait automatiquement à
0 ms pour préparer le test suivant.

Par exemple, pour Hendrix, le programme a enregistré :

```text
Hendrix : seuil remarqué à 45 ms
```

## Méthode du test

J'ai fait essayer le programme à cinq personnes :

- Hendrix ;
- Erwan ;
- Thomas ;
- Juliette ;
- Suzy.

Pour chaque personne, j'ai commencé avec un retard de **0 ms**.

La personne déplaçait rapidement la souris dans la zone blanche pendant que
le cercle suivait son mouvement.

J'ai ensuite augmenté progressivement le retard.

Dès que la personne disait commencer à remarquer que le cercle ne suivait
plus immédiatement sa souris, j'ai arrêté l'augmentation et enregistré la
valeur affichée.

Le retard était ensuite remis à 0 ms avant de passer à la personne suivante.

## Seuils obtenus

Les cinq tests m'ont donné les résultats suivants :

| Participant | Retard à partir duquel le décalage est remarqué |
|---|---:|
| Hendrix | **45 ms** |
| Erwan | **55 ms** |
| Thomas | **35 ms** |
| Juliette | **50 ms** |
| Suzy | **40 ms** |

Les cinq seuils réellement relevés sont donc :

```text
Hendrix  : 45 ms
Erwan    : 55 ms
Thomas   : 35 ms
Juliette : 50 ms
Suzy     : 40 ms
```

## Comparaison des résultats

Le seuil le plus faible est celui de Thomas :

**35 ms**

Le seuil le plus élevé est celui d'Erwan :

**55 ms**

L'écart entre les deux est donc :

`55 - 35 = 20 ms`

La moyenne des cinq seuils est :

`(45 + 55 + 35 + 50 + 40) / 5`

`= 225 / 5`

`= 45 ms`

Le seuil moyen observé pendant mon expérience est donc de :

**45 ms**

Ces résultats montrent que les cinq personnes ne commencent pas toutes à
remarquer le retard exactement au même moment.

Thomas l'a remarqué dès 35 ms alors qu'Erwan ne l'a signalé qu'à 55 ms.

## Comparaison avec le budget de 20 ms

Les cinq seuils obtenus sont tous supérieurs à **20 ms** :

```text
Budget de référence : 20 ms

Thomas   : 35 ms
Suzy     : 40 ms
Hendrix  : 45 ms
Juliette : 50 ms
Erwan    : 55 ms
```

Même le seuil le plus faible mesuré, **35 ms**, reste supérieur à 20 ms.

La moyenne des seuils, **45 ms**, est également plus de deux fois supérieure
au budget de 20 ms.

Cela ne signifie pas que 45 ms serait un retard acceptable pour un casque
de réalité virtuelle.

Le test réalisé ici se déroule sur un **écran ordinaire**, avec une souris.
La situation sensorielle est donc différente de celle rencontrée dans un
casque.

## Pourquoi le seuil doit être plus bas dans un casque

Sur l'écran ordinaire, la personne déplace une souris avec sa main et observe
un cercle qui suit ce mouvement.

Le retard est principalement jugé en comparant le mouvement effectué avec
le comportement visuel du cercle.

Dans un casque de réalité virtuelle, la situation est beaucoup plus
contraignante.

Lorsque l'utilisateur tourne réellement la tête, son système vestibulaire,
lié notamment à l'oreille interne, détecte immédiatement le mouvement.

L'image présentée par le casque doit donc être mise à jour très rapidement
pour rester cohérente avec cette sensation physique.

Si l'utilisateur tourne la tête mais que l'image correspond encore à une
orientation légèrement ancienne, les informations visuelles ne correspondent
plus correctement aux informations provenant du mouvement réel du corps.

Le décalage peut alors être beaucoup plus gênant que celui observé avec une
souris sur un écran ordinaire.

C'est pourquoi le fait que mes participants aient commencé à signaler le
retard entre **35 et 55 ms** sur l'écran ne signifie pas qu'un tel retard
serait acceptable en VR.

## Ce que j'en retiens

Avant l'expérience, je savais qu'un retard pouvait être perceptible, mais le
test permet de voir que le seuil dépend aussi de la personne.

Dans mon expérience :

- le premier seuil apparaît à 35 ms ;
- le dernier apparaît à 55 ms ;
- la moyenne est de 45 ms.

Sur écran ordinaire, les participants ont donc toléré un retard supérieur
aux 20 ms pris comme référence.

Mais la comparaison avec la VR montre pourquoi cette expérience sur écran
est moins exigeante.

Dans un casque, l'image doit rester cohérente avec un mouvement réel de la
tête déjà ressenti par le corps.

## Conclusion

Pour cet exercice, j'ai réalisé un programme dans lequel un cercle suit la
souris avec un retard réglable de **0 à 200 ms**.

Je l'ai ensuite fait essayer à cinq personnes et j'ai obtenu les seuils
suivants :

```text
Hendrix  : 45 ms
Erwan    : 55 ms
Thomas   : 35 ms
Juliette : 50 ms
Suzy     : 40 ms
```

Le seuil moyen obtenu est de **45 ms**, avec un minimum de **35 ms** et un
maximum de **55 ms**.

Les cinq seuils sont donc supérieurs au budget de référence de 20 ms.

Cette différence s'explique par le fait que le test est réalisé avec une
souris sur un écran ordinaire.

Dans un casque VR, lorsque la tête bouge, l'oreille interne détecte
immédiatement ce mouvement. L'image doit donc suivre très rapidement afin
d'éviter une contradiction entre ce que le corps ressent et ce que les yeux
voient.

Je retiens donc que les seuils observés sur écran ne peuvent pas être
directement utilisés comme seuils acceptables en réalité virtuelle. Le
mouvement-vers-photon doit rester beaucoup plus faible afin de conserver une
expérience cohérente et confortable.
