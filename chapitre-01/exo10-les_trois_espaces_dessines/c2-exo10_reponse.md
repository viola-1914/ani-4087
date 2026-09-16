# Exercice 10 — Les trois espaces dessinés

Pour cet exercice, j'ai représenté une même pièce vue de côté avec un
utilisateur debout et les origines des trois espaces : STAGE, LOCAL et VIEW.

## STAGE

Dans l'espace STAGE, l'origine est liée à l'espace physique et le plan
`y = 0` correspond au plancher.

Une table dont le plateau est à 0,80 m est donc naturellement représentée
à 0,80 m au-dessus du sol.

## VIEW

L'espace VIEW est lié à la position de la tête de l'utilisateur.

Son origine se déplace donc avec l'utilisateur. Une position située à
`y = 0,80 m` dans cet espace est mesurée relativement à cette origine et
non directement à partir du plancher.

## LOCAL

L'espace LOCAL possède une origine locale établie par le système.

Contrairement à STAGE, son origine ne garantit pas que `y = 0` corresponde
au plancher. Une hauteur de 0,80 m exprimée dans cet espace est donc mesurée
par rapport à cette origine locale.

## Schéma de principe

                    VIEW
                      O  ← origine liée à la tête
                     /|\
                    / | \
                      |
                   utilisateur

             ┌───────────────┐
             │     TABLE     │
             └───────────────┘
                   0,80 m
                      ↑
                      |
LOCAL : O             |
                      |
──────────────────────O────────────────── sol
                    STAGE
                    y = 0

Dans mon dessin sur papier, je place également une table à `y = 0,80 m`
par rapport à chacune des trois origines afin de montrer que les mêmes
coordonnées numériques ne désignent pas nécessairement le même endroit
physique lorsque l'espace de référence change.

## Conclusion

Les coordonnées d'un objet n'ont de sens que si l'on connaît l'espace de
référence dans lequel elles sont exprimées.

Dans STAGE, 0,80 m peut être directement interprété comme une hauteur de
0,80 m au-dessus du plancher. Dans LOCAL et VIEW, cette même valeur est
relative à leurs propres origines.
