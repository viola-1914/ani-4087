# Chapitre 02 — Exo 15 : votre première clé

## La commande employée

Lancée dans l'Invite de commandes Windows, depuis `C:\Users\HP` :

```text
mkdir "%USERPROFILE%\cles"
jenga keygen -i --alias masalle --output "%USERPROFILE%\cles\masalle.jks"
```

La commande ne contient aucun mot de passe : avec l'option `-i` (mode interactif), Jenga le demande pendant l'exécution, et la saisie ne s'affiche pas à l'écran.

Sortie obtenue :

```text
============================================================
Generate Android Keystore
============================================================

Key alias [masalle]:
Validity (days) [10000]:
Distinguished Name (CN=Name, OU=Org, O=Company, L=City, ST=State, C=Country) [CN=Jenga User]: CN=Mafo
Keystore password:
Key password (default: same as keystore):
Generating keystore...
Keystore generated: C:\Users\HP\cles\masalle.jks
Alias: masalle
```

## Ce que j'ai dû installer

`jenga keygen` s'appuie sur `keytool`, l'outil de Java. Il n'était pas présent sur ma machine (`where.exe keytool` ne trouvait rien). J'ai donc d'abord installé un JDK :

```text
winget install EclipseAdoptium.Temurin.21.JDK
```

Après réouverture du terminal, `where.exe keytool` répondait `C:\Program Files\Eclipse Adoptium\jdk-21.0.12.101-hotspot\bin\keytool.exe`.

## Où sont rangés la clé et son mot de passe

- **La clé** est dans `C:\Users\HP\cles\masalle.jks`, un dossier de mon profil Windows **en dehors du dépôt** `ani-4087`. Elle n'a jamais été dans un dossier suivi par Git.
- **Le mot de passe**, avec l'emplacement du fichier et l'alias `masalle`, est **noté dans un carnet papier gardé chez moi**, séparé de l'ordinateur. Il n'apparaît ni dans le dépôt, ni dans ce fichier, ni dans aucun fichier de l'ordinateur.

Pour qu'une clé ne puisse jamais être poussée par erreur, le `.gitignore` à la racine du dépôt exclut `*.jks`, `*.keystore` et `*.p12`.

## Les deux pièges de `jenga keygen`

En lisant le code de la commande (`Jenga/Commands/Keygen.py`), j'ai repéré deux comportements par défaut à éviter :

1. **Sans `-i`**, Jenga ne demande aucun mot de passe : il utilise `android` pour le magasin et pour la clé, un mot de passe connu de tous. La clé ne protège alors rien.
2. **Sans `--output`**, la clé est créée sous le nom `keystore.jks` dans le dossier courant. Lancée depuis un dossier d'exercice, elle finirait dans le dépôt, puis sur GitHub.

## Pourquoi le mot de passe doit être noté

Une application Android doit être signée avec la même clé à chaque mise à jour. Si le mot de passe est perdu, la clé devient inutilisable et l'application ne peut plus être mise à jour. Le garder seulement en mémoire ne suffit pas.
