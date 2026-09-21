# Chapitre 02 — Exo 23 : le déploiement automatisé

## Le script

`deployer.py`, dans ce dossier, enchaîne les quatre étapes et s'arrête à la **première** qui échoue, en disant laquelle et pourquoi. Il est écrit en Python pour tourner de la même façon sous Windows, Linux et macOS.

```text

python deployer.py                   # les quatre étapes

python deployer.py --propre          # commence par jenga clean

python deployer.py --sans-installer  # s'arrête après la signature

python deployer.py --cle <fichier> --alias <alias>

```

| Étape | Ce qu'elle fait | Ce qu'elle vérifie avant de passer à la suite |

|---|---|---|

| 0. Vérifier les outils | trouve `jenga`, le SDK Android, le build-tools le plus récent, `adb` | chaque outil existe **sur cette machine** |

| 1. Construire | `jenga build --platform android-arm64` (précédé de `jenga clean` avec `--propre`) | le code de retour de Jenga, **et** que `libMaSalle.so` existe et est bien une bibliothèque **arm64** (en-tête ELF) |

| 2. Empaqueter | copie l'APK produit par Jenga dans `dist/` | que l'archive est intacte et contient `AndroidManifest.xml` et `lib/arm64-v8a/libMaSalle.so` |

| 3. Signer | `apksigner sign` avec ma clé, puis `apksigner verify` | que la signature est valide |

| 4. Installer | `adb install -r` | qu'**un seul** appareil est branché et autorisé |

En cas d'échec, il affiche par exemple `ECHEC a l'etape 3/4 : Signer`, suivi de la raison, et rend le numéro de l'étape comme code de retour (9 si la vérification des outils échoue). Le mot de passe de la clé n'est jamais dans le script : `apksigner` le demande au clavier.

Chaque vérification répond à un piège rencontré dans ce chapitre : « Jenga dit *Build Successful* » ne suffit pas (exercices 9, 12 et 17), une bibliothèque x86_64 peut se retrouver à la place de l'arm64 (exercice 22), une bibliothèque peut manquer dans le paquet (exercice 17).

## Sur ma machine

Windows x86_64, Jenga 2.8.0, SDK dans `C:\Android`, téléphone TECNO SPARK 10C. Ces deux exécutions ont eu lieu avec la première version du script, avant la correction du NDK décrite plus bas ; la version corrigée a été relancée ensuite (voir « Retour sur ma machine »).

**Téléphone débranché : le script s'arrête à l'étape 4.**

```text

python deployer.py --propre

[0/4] Verifier les outils

  SDK : C:\Android

  build-tools : 34.0.0

[1/4] Construire

  > ...\jenga.EXE clean --platform android-arm64 --config Release

  > ...\jenga.EXE build --platform android-arm64 --config Release

  ...

  bibliotheque arm64 : 23440 octets

[2/4] Empaqueter

  MaSalle-non-signe.apk : 9258 octets, 3 fichiers

[3/4] Signer

  (apksigner va demander le mot de passe de la cle)

  > C:\Android\build-tools\34.0.0\apksigner.bat sign --ks C:\Users\HP\cles\masalle.jks --ks-key-alias masalle --out ...\dist\MaSalle.apk ...\dist\MaSalle-non-signe.apk

Keystore password for signer #1:

  > C:\Android\build-tools\34.0.0\apksigner.bat verify ...\dist\MaSalle.apk

  MaSalle.apk : 16801 octets, signature verifiee

[4/4] Installer

ECHEC a l'etape 4/4 : Installer

  aucun appareil pret : branchez-le, deverrouillez-le et autorisez le debogage USB.

```

**Téléphone branché : les quatre étapes réussissent.**

```text

python deployer.py

...

[3/4] Signer

  MaSalle.apk : 16801 octets, signature verifiee

[4/4] Installer

  appareil : 105453739L108475

  > C:\Android\platform-tools\adb.exe install -r ...\dist\MaSalle.apk

Performing Incremental Install

Performing Streamed Install

Success

SUCCES : construit, empaquete, signe et installe.

```

## Sur une autre machine : mon camarade

Mon camarade a fait tourner le script sur sa machine, sans que je modifie quoi que ce soit, puis m'a transmis ses résultats. Voici tout ce qu'il a fait.

### Sa machine

| | Ma machine | La machine de mon camarade |

|---|---|---|

| Système | Windows x86_64 | **Ubuntu 24.04.4 LTS** (Linux), x86_64 |

| Python | 3.14.6 | **3.12.3** |

| Jenga | 2.8.0 | 2.8.0 |

| SDK Android | `C:\Android`, complet | **aucun** |

| NDK Android | 26.3.11579264 | **aucun** |

| Appareil | TECNO SPARK 10C | **aucun** |

| Dossier du projet | `C:\Users\HP\Documents\GitHub\ani-4087\...` | `/tmp/camarade/dépôt ani-4087/exo23`, avec **des espaces et des accents** |

Il a copié `deployer.py`, `MaSalle.jenga`, `src/` et `.gitignore` tels quels, et lancé le script avec `python3 deployer.py`.

### Essai 1 : sa machine, telle quelle

```text

python3 deployer.py

[0/4] Verifier les outils

ECHEC a l'etape 0/4 : Verification des outils

  SDK Android introuvable : definissez ANDROID_SDK_ROOT (ou ANDROID_HOME) vers le dossier du SDK.

code=9

```

Le script s'arrête avant de commencer, en disant ce qui manque. C'est le comportement voulu.

### Essai 2 : un dossier SDK, mais pas de NDK

Mon camarade a fourni un dossier SDK minimal (build-tools et `adb` remplacés par de petits programmes qui imitent les vrais), et lancé le **vrai** Jenga :

```text

python3 deployer.py --sans-installer

[0/4] Verifier les outils

  SDK : /tmp/sim/sdk

  build-tools : 34.0.0

[1/4] Construire

  > /usr/local/bin/jenga build --platform android-arm64 --config Release

Loading workspace...

Cannot create builder: No suitable toolchain found for Android arm64

ECHEC a l'etape 1/4 : Construire

  jenga a renvoye le code 1.

```

**C'est ici qu'il a trouvé ce qui ne marchait que chez moi.** Sur ma machine, le NDK est installé depuis l'exercice 16, donc je n'avais jamais pensé à le vérifier. Sur la sienne, il manque : le script passait l'étape 0 sans rien dire, et l'échec n'arrivait qu'au milieu du build, avec un message de Jenga (« No suitable toolchain ») qui ne dit pas quoi installer.

### La correction

J'ai ajouté à l'étape 0 une fonction `trouver_ndk` : elle cherche le NDK dans `ANDROID_NDK_ROOT`, puis `ANDROID_NDK_HOME`, puis dans le dossier `ndk/` du SDK, et s'arrête avec un message qui dit quoi installer. L'étape 0 affiche maintenant aussi la ligne `NDK : ...`.

### Essai 3 : la même machine, après correction

```text

python3 deployer.py --sans-installer

[0/4] Verifier les outils

ECHEC a l'etape 0/4 : Verification des outils

  NDK Android introuvable : definissez ANDROID_NDK_ROOT, ou installez-le avec sdkmanager "ndk;26.3.11579264".

code=9

```

Le manque est détecté tout de suite, avant de lancer Jenga, avec la commande pour le réparer.

### Essai 4 : une panne à chaque étape

Sa machine ne peut pas construire pour Android. Pour vérifier les étapes suivantes, mon camarade a remplacé `jenga`, `apksigner` et `adb` par de faux outils qui imitent les vrais, et provoqué une panne à chaque étape, toujours dans le dossier avec espaces et accents :

| Panne provoquée | Ce que le script a affiché | Code |

|---|---|---:|

| la compilation échoue | `ECHEC a l'etape 1/4 : Construire` — `jenga a renvoye le code 1.` | 1 |

| bibliothèque x86_64 au lieu d'arm64 | `ECHEC a l'etape 1/4 : Construire` — `libMaSalle.so n'est pas une bibliotheque arm64 (un autre build l'a ecrasee ?) : relancez avec --propre.` | 1 |

| bibliothèque absente de l'APK | `ECHEC a l'etape 2/4 : Empaqueter` — `il manque dans l'APK : lib/arm64-v8a/libMaSalle.so` | 2 |

| mauvais mot de passe | `ECHEC a l'etape 3/4 : Signer` — `apksigner a renvoye le code 1.` | 3 |

| aucun appareil | `ECHEC a l'etape 4/4 : Installer` — `aucun appareil pret : branchez-le, deverrouillez-le et autorisez le debogage USB.` | 4 |

| tout va bien | `SUCCES : construit, empaquete, signe et installe.` | 0 |

Dans chaque cas, le script s'est arrêté à la bonne étape, sans exécuter les suivantes, avec le bon message et le bon code de retour.

### Ce que cet essai vaut, et ce qu'il ne vaut pas

- **Réel :** un autre système (Linux), une autre version de Python, un chemin avec espaces et accents, le vrai Jenga, et une machine sans SDK ni NDK. C'est là qu'est apparu le défaut du NDK.

- **Simulé :** les essais 2 à 4 remplacent `apksigner`, `adb` et parfois `jenga` par des imitations. Ils vérifient la logique du script, pas une vraie construction Android ni une vraie installation.

- **Pas fait :** aucune installation sur un deuxième appareil.

## Retour sur ma machine, avec la version corrigée

Après la correction du NDK, j'ai relancé la nouvelle version de `deployer.py` sur ma machine, téléphone branché. `findstr trouver_ndk deployer.py` confirme que c'est bien elle.

**Premier lancement : un vrai échec à l'étape 3.** Je me suis trompée en tapant le mot de passe de la clé :

```text

python deployer.py

[0/4] Verifier les outils

  SDK : C:\Android

  build-tools : 34.0.0

  NDK : C:\Android\ndk\26.3.11579264

[1/4] Construire

  ...

  bibliotheque arm64 : 23440 octets

[2/4] Empaqueter

  MaSalle-non-signe.apk : 9258 octets, 3 fichiers

[3/4] Signer

  (apksigner va demander le mot de passe de la cle)

Keystore password for signer #1:

Failed to load signer "signer #1"

java.io.IOException: keystore password was incorrect

ECHEC a l'etape 3/4 : Signer

  apksigner.bat a renvoye le code 2.

```

La nouvelle ligne `NDK : ...` apparaît à l'étape 0. Et ce qui n'avait été testé qu'en simulation s'est produit pour de vrai : le script s'arrête à la signature, n'essaie pas d'installer un APK non signé, et dit quelle étape a échoué. Détail : le vrai `apksigner.bat` renvoie le code 2, là où le faux outil de la simulation renvoyait 1 ; le script ne dépend pas de la valeur, seulement du fait qu'elle n'est pas nulle.

**Deuxième lancement : les quatre étapes réussissent.**

```text

python deployer.py

[0/4] Verifier les outils

  SDK : C:\Android

  build-tools : 34.0.0

  NDK : C:\Android\ndk\26.3.11579264

[1/4] Construire

  ...

  bibliotheque arm64 : 23440 octets

[2/4] Empaqueter

  MaSalle-non-signe.apk : 9258 octets, 3 fichiers

[3/4] Signer

  (apksigner va demander le mot de passe de la cle)

Keystore password for signer #1:

  > C:\Android\build-tools\34.0.0\apksigner.bat verify ...\dist\MaSalle.apk

  MaSalle.apk : 16801 octets, signature verifiee

[4/4] Installer

  appareil : 105453739L108475

  > C:\Android\platform-tools\adb.exe install -r ...\dist\MaSalle.apk

Performing Incremental Install

Performing Streamed Install

Success

SUCCES : construit, empaquete, signe et installe.

```

La version corrigée fonctionne donc sur les deux machines : elle détecte l'absence de NDK chez mon camarade, et elle va jusqu'au bout chez moi.

## Ce qui ne marchait que chez moi, et ce que le script fait à la place

En plus du NDK trouvé par mon camarade, j'ai relu mes commandes manuelles des exercices 16 à 22 en me demandant ce qui ne marchait **que chez moi** :

| Ce qui ne marchait que chez moi | Ce que fait le script |

|---|---|

| le NDK supposé présent (trouvé par l'essai de mon camarade) | il le cherche dès l'étape 0 et dit quoi installer |

| le SDK supposé dans `C:\Android` | il le lit dans `ANDROID_SDK_ROOT` (ou `ANDROID_HOME`), et s'arrête avec un message clair s'il n'y est pas |

| `build-tools\34.0.0` écrit en dur | il prend la version de build-tools la plus récente installée |

| `apksigner.bat`, `adb.exe` : noms Windows | il choisit `apksigner.bat`/`adb.exe` sous Windows, `apksigner`/`adb` ailleurs ; vérifié sous Linux par mon camarade |

| la clé dans `%USERPROFILE%\cles\masalle.jks`, alias `masalle` | option `--cle` ou variable `MASALLE_CLE` ; option `--alias` ou `MASALLE_ALIAS` ; par défaut `~/cles/masalle.jks` |

| `jenga` supposé dans le `PATH` | il le cherche et s'arrête avec un message s'il manque |

| mon téléphone toujours branché | l'option `--sans-installer` permet de s'arrêter après la signature |

| les pièges connus de ma machine (cache de Jenga, bibliothèque x86_64 écrasée) | l'option `--propre` et la vérification de l'en-tête ELF |

Pour mon camarade, il suffirait de lancer `python deployer.py --cle <sa clé>` (ou `python3` sous Linux et macOS) avec sa propre clé, par exemple une clé de test.