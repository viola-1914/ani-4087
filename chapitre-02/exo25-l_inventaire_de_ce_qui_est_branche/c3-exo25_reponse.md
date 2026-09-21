# Chapitre 02 — Exo 25 : l'inventaire de ce qui est branché

## Le montage

Deux téléphones Android branchés en même temps sur deux ports USB de mon ordinateur (Windows x86_64, Jenga 2.8.0), débogage USB activé et autorisé sur les deux :

- mon téléphone, un TECNO SPARK 10C ;
- un second téléphone, un Samsung.

Les deux commandes viennent de `jenga deploy` :

```text
jenga deploy --platform android --list-devices
jenga deploy --platform android --list-devices --detailed
```

## Première sortie : `--list-devices`

```text
List of devices attached
105453739L108475        device
R92X51YQD1P     device
```

## Seconde sortie : `--list-devices --detailed`

```text
SERIAL            MARQUE   MODELE      ANDROID  ABI
----------------  -------  ----------  -------  ---------
105453739L108475  TECNO    TECNO KI5k  12       arm64-v8a
R92X51YQD1P       samsung  SM-A055F    15       arm64-v8a
```

J'avais d'abord lancé les mêmes commandes avec chaque téléphone branché seul ; les lignes sont identiques à celles ci-dessus, ce qui montre que chaque appareil garde le même numéro de série quel que soit ce qui est branché à côté.

## Ce que la seconde sortie apprend que la première ne disait pas

La première sortie est la liste brute de `adb devices` : **un numéro de série et un état**, rien d'autre. `105453739L108475` et `R92X51YQD1P` ne disent pas quel téléphone est lequel. Le mot `device` dit seulement que l'appareil est connecté et autorisé.

La seconde interroge chaque appareil (le code de la commande appelle `adb -s <série> shell getprop` sur quatre propriétés) et ajoute, pour chacun :

| Colonne | Propriété Android lue | TECNO | Samsung |
|---|---|---|---|
| MARQUE | `ro.product.manufacturer` | TECNO | samsung |
| MODELE | `ro.product.model` | TECNO KI5k | SM-A055F |
| ANDROID | `ro.build.version.release` | 12 | 15 |
| ABI | `ro.product.cpu.abi` | arm64-v8a | arm64-v8a |

Ce que j'en tire :

- **Qui est qui.** Sans la seconde sortie, je ne pourrais pas savoir que `R92X51YQD1P` est le Samsung. Or c'est le numéro de série qu'il faut donner à `adb -s` pour viser un appareil précis.
- **Le modèle est une référence technique, pas un nom commercial.** Mon SPARK 10C s'affiche `TECNO KI5k`, le Samsung `SM-A055F`. Il faut savoir les faire correspondre.
- **Deux versions d'Android très différentes.** Android 12 d'un côté, Android 15 de l'autre. Mon paquet vise `androidminsdk(24)`, donc Android 7 et plus : il est compatible avec les deux.
- **L'ABI**, c'est-à-dire l'architecture du processeur, qui décide quelle bibliothèque native le téléphone peut charger.

La seconde sortie a aussi une limite, que j'ai vue dans le code de la commande : elle ne garde que les appareils à l'état `device`. Un téléphone branché mais pas encore autorisé (`unauthorized`) ou `offline` apparaît dans la première liste et disparaît de la seconde, sans message. La première sortie reste donc utile pour voir **tout** ce qui est branché.

## L'ABI de chaque appareil

| Appareil | Série | ABI |
|---|---|---|
| TECNO SPARK 10C (`TECNO KI5k`), Android 12 | `105453739L108475` | **arm64-v8a** |
| Samsung `SM-A055F`, Android 15 | `R92X51YQD1P` | **arm64-v8a** |

Les deux sont en **arm64-v8a**. Mon APK arm64 des exercices 17 à 23 peut donc s'installer sur l'un comme sur l'autre, alors que l'APK x86_64 de l'exercice 16 aurait été refusé par les deux (`INSTALL_FAILED_NO_MATCHING_ABIS`).

La colonne ABI ne montre que l'architecture **principale** (`ro.product.cpu.abi`). Un appareil en accepte souvent plusieurs : pour mon TECNO, `adb shell getprop ro.product.cpu.abilist` donne `arm64-v8a,armeabi-v7a,armeabi` (exercice 17). La liste complète se lit avec cette propriété, pas avec `--detailed`.

## Ce que change un deuxième appareil

Avec deux téléphones branchés, une commande `adb` sans précision ne sait plus lequel viser. Il faut ajouter `-s <série>`, par exemple :

```text
adb -s R92X51YQD1P install -r dist\MaSalle.apk
```

Mon script de déploiement de l'exercice 23 s'arrête d'ailleurs à l'étape 4 dans ce cas, avec le message `plusieurs appareils branches (...) : gardez-en un seul.` : c'est la liste de la première sortie qui lui permet de le voir.
