# Exercice 10 — Les trois espaces dessinés

Pour cet exercice, j'ai représenté une même pièce vue de côté avec un
utilisateur debout et les trois espaces de référence : **STAGE, LOCAL et
VIEW**.

L'objectif est surtout de voir ce que signifie une même coordonnée
`y = 0,80 m` lorsqu'elle est exprimée dans trois espaces différents.

## Les trois origines

### STAGE

Dans l'espace **STAGE**, l'origine est liée à l'espace physique.

Le plan :

`y = 0`

correspond au plancher.

Une table placée à :

`y = 0,80 m`

dans STAGE a donc son plateau à **80 cm au-dessus du sol**.

### VIEW

L'espace **VIEW** est lié à la tête de l'utilisateur.

Son origine se trouve donc au niveau de la tête et se déplace avec elle.

Une table placée à :

`y = 0,80 m`

dans VIEW ne se retrouve donc pas à 80 cm du sol.

Elle se retrouve **80 cm au-dessus de l'origine VIEW**, donc au-dessus de
la tête de l'utilisateur.

### LOCAL

L'espace **LOCAL** possède une origine locale établie par le système.

Contrairement à STAGE, son origine ne garantit pas que :

`y = 0`

corresponde au plancher.

Une table placée à :

`y = 0,80 m`

dans LOCAL se retrouve donc **80 cm au-dessus de l'origine LOCAL**.

Sa hauteur physique dans la pièce dépend de l'endroit où cette origine locale
a été établie.

## Les trois tables sur le même dessin

Voici le dessin de la pièce vue de côté.

Les trois tables portent volontairement la même coordonnée :

`y = 0,80 m`

mais cette coordonnée est exprimée dans trois espaces différents.

```text
                    TABLE VIEW
              ┌──────────────────┐
              │                  │
              └──────────────────┘
                     ↑
                     │ 0,80 m
                     │
                    VIEW
                     O  ← origine liée à la tête
                    /|\
                   / | \
                     |
                     |
                  utilisateur


       TABLE LOCAL
   ┌──────────────────┐
   │                  │
   └──────────────────┘
          ↑
          │ 0,80 m
          │
LOCAL  O  ← origine locale


                     TABLE STAGE
                 ┌──────────────────┐
                 │                  │
                 └──────────────────┘
                         ↑
                         │ 0,80 m
                         │
─────────────────────────O──────────────────────── sol
                       STAGE
                        y = 0
```

Ce dessin montre donc **trois tables différentes** et non une seule.

## Table dans STAGE

Pour STAGE :

```text
origine STAGE
      O
      |
      | 0,80 m
      |
┌───────────────┐
│  TABLE STAGE  │
└───────────────┘
```

Comme l'origine STAGE correspond au plancher, la table se retrouve à une
hauteur normale de **80 cm au-dessus du sol**.

C'est le cas où la valeur `0,80 m` peut être directement interprétée comme
une hauteur physique au-dessus du plancher.

## Table dans VIEW

Pour VIEW :

```text
┌───────────────┐
│  TABLE VIEW   │
└───────────────┘
       ↑
       | 0,80 m
       |
       O  VIEW
      /|\
       |
   utilisateur
```

L'origine VIEW étant liée à la tête, ajouter `0,80 m` sur l'axe Y place la
table **80 cm au-dessus des yeux**, et non 80 cm au-dessus du plancher.

C'est le résultat qui m'a paru le plus frappant dans le dessin : la même
valeur numérique qui donnait une hauteur normale dans STAGE place maintenant
la table au-dessus de l'utilisateur.

## Table dans LOCAL

Pour LOCAL :

```text
┌───────────────┐
│  TABLE LOCAL  │
└───────────────┘
       ↑
       | 0,80 m
       |
       O  LOCAL
```

Ici encore, les `0,80 m` sont mesurés depuis l'origine LOCAL.

On ne peut donc pas déterminer la hauteur de cette table par rapport au sol
en connaissant seulement sa coordonnée locale.

Il faut également connaître la position de l'origine LOCAL dans la pièce.

## Comparaison

| Espace | Origine | Signification de `y = 0,80 m` |
|---|---|---|
| STAGE | Plancher | Table à 80 cm au-dessus du sol |
| LOCAL | Origine locale du système | Table à 80 cm au-dessus de l'origine LOCAL |
| VIEW | Tête de l'utilisateur | Table à 80 cm au-dessus de l'origine VIEW, donc au-dessus des yeux |

La valeur numérique est donc exactement la même :

`0,80 m`

mais elle ne désigne pas nécessairement la même position physique.

## Ce que montre le dessin

Avant de placer les trois tables, il était facile de considérer `0,80 m`
simplement comme une hauteur de table.

Le dessin montre que cette interprétation n'est correcte que si l'on connaît
l'espace dans lequel cette coordonnée est exprimée.

Dans **STAGE**, la relation avec le plancher est immédiate.

Dans **VIEW**, la même coordonnée place la table au-dessus de la tête parce
que l'origine suit la tête.

Dans **LOCAL**, sa position par rapport au sol dépend de la position de
l'origine locale.

## Conclusion

Les coordonnées d'un objet n'ont de sens que si l'on connaît l'espace de
référence dans lequel elles sont exprimées.

Une table placée à `y = 0,80 m` ne se retrouve pas nécessairement au même
endroit dans STAGE, LOCAL et VIEW :

- dans **STAGE**, elle est à 80 cm au-dessus du plancher ;
- dans **VIEW**, elle est à 80 cm au-dessus de l'origine située au niveau de
  la tête ;
- dans **LOCAL**, elle est à 80 cm au-dessus de l'origine locale.

Le fait de dessiner **les trois tables sur la même pièce** rend cette
différence beaucoup plus visible : une même coordonnée numérique peut
désigner des positions physiques très différentes selon l'espace de
référence utilisé.
