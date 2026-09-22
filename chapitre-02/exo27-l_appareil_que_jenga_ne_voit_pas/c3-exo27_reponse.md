# Chapitre 02 — Exo 27 : l'appareil que Jenga ne voit pas

## Le montage

Un seul téléphone, le Samsung `SM-A055F` (série `R92X51YQD1P`, Android 15), sur mon ordinateur Windows avec Jenga 2.8.0. Le TECNO est débranché, pour qu'aucun autre appareil ne brouille la lecture.

Pour provoquer l'état `unauthorized` : téléphone débranché, **Options de développement → Révoquer les autorisations de débogage USB**, puis rebranché. La fenêtre « Autoriser le débogage USB ? » est apparue sur l'écran, et je n'y ai pas touché.

Il a fallu relancer le serveur `adb` pour que le téléphone rebranché apparaisse : avant, `adb devices` ne listait rien du tout.

```text
C:\Android\platform-tools\adb.exe kill-server
C:\Android\platform-tools\adb.exe start-server
* daemon not running; starting now at tcp:5037
* daemon started successfully
```

## Téléphone branché, autorisation refusée : `unauthorized`

| Ce que dit Jenga (`jenga deploy --platform android --list-devices --detailed`) | Ce que dit `adb devices` |
|---|---|
| `No Android device connected.` | `List of devices attached`<br>`R92X51YQD1P     unauthorized` |

Les sorties complètes :

```text
jenga deploy --platform android --list-devices --detailed
No Android device connected.
```

```text
C:\Android\platform-tools\adb.exe devices
List of devices attached
R92X51YQD1P     unauthorized
```

Pour comparer, la version sans `--detailed`, qui recopie la sortie d'`adb` :

```text
jenga deploy --platform android --list-devices
List of devices attached
R92X51YQD1P     unauthorized
```

## Téléphone autorisé : `device`

Après avoir touché **Autoriser** sur le Samsung :

| Ce que dit Jenga (`--list-devices --detailed`) | Ce que dit `adb devices` |
|---|---|
| `SERIAL       MARQUE   MODELE    ANDROID  ABI`<br>`R92X51YQD1P  samsung  SM-A055F  15       arm64-v8a` | `List of devices attached`<br>`R92X51YQD1P     device` |

```text
jenga deploy --platform android --list-devices --detailed
SERIAL       MARQUE   MODELE    ANDROID  ABI
-----------  -------  --------  -------  ---------
R92X51YQD1P  samsung  SM-A055F  15       arm64-v8a
```

```text
C:\Android\platform-tools\adb.exe devices
List of devices attached
R92X51YQD1P     device
```

```text
jenga deploy --platform android --list-devices
List of devices attached
R92X51YQD1P     device
```

## Un troisième cas, rencontré en route : rien du tout

Avant de relancer le serveur `adb`, aucune commande ne voyait le téléphone :

| Ce que dit Jenga (`--list-devices --detailed`) | Ce que dit `adb devices` |
|---|---|
| `No Android device connected.` | `List of devices attached` (liste vide) |

**Jenga donne exactement le même message** quand rien n'est détecté et quand un téléphone est branché mais pas autorisé. `adb`, lui, distingue les deux.

## Pourquoi c'est ainsi

Le code de la commande (`Jenga/Commands/Deploy.py`) lit la sortie d'`adb devices` et ne garde que les lignes dont l'état est exactement `device` :

```python
if len(parts) >= 2 and parts[1] == "device":
    serials.append(parts[0])
if not serials:
    Colored.PrintWarning("No Android device connected.")
```

Un appareil `unauthorized` (ou `offline`) est donc écarté sans un mot, puis la liste vide est présentée comme l'absence de tout appareil.

## Pourquoi le message de Jenga est trompeur, et ce que j'aurais écrit

**Le message est trompeur parce qu'il affirme qu'aucun appareil n'est connecté alors que le téléphone est bien branché et vu par `adb` : il n'attend qu'une autorisation, et la seule chose à faire (toucher « Autoriser » sur le téléphone) n'est jamais dite, ce qui pousse à chercher la panne dans le câble, le port ou le pilote.**

À sa place, j'aurais écrit, pour chaque appareil écarté, son état et l'action attendue :

```text
1 appareil branché mais inutilisable :
  R92X51YQD1P  unauthorized  -> acceptez « Autoriser le débogage USB ? » sur le téléphone
Aucun appareil prêt.
```

et gardé `No Android device connected.` pour le seul cas où `adb devices` ne liste vraiment rien.

C'est d'ailleurs ce que fait mon script de déploiement de l'exercice 23 : quand aucun appareil n'est prêt, il ajoute `Vus mais non utilisables : R92X51YQD1P (unauthorized).`
