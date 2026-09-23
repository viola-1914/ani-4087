# Chapitre 02 — Démo 9 : trois appareils, une commande

## Ce qui a pu être branché, et ce qui n'a pas pu

L'énoncé demande trois appareils, ou deux et un émulateur. J'ai deux téléphones Android, et pas de casque. J'ai donc tenté d'ajouter un émulateur, sans y parvenir : le téléchargement de l'image système a été coupé à chaque essai, sans jamais reprendre où il s'était arrêté.

```text
C:\Android\cmdline-tools\latest\bin\sdkmanager.bat "emulator" "system-images;android-34;google_apis;x86_64"
Warning: An error occurred while preparing SDK package Google APIs Intel x86_64 Atom System Image: Connection reset.
[======================                 ] 56% Downloading x86_64-34_r14.zip...

(deuxieme essai, reparti de zero)
[=====                                  ] 15% Downloading x86_64-34_r14.zip...

(troisieme essai, avec l'image legere sans les services Google)
C:\Android\cmdline-tools\latest\bin\sdkmanager.bat "emulator" "system-images;android-34;default;x86_64"
Warning: An error occurred while preparing SDK package Intel x86_64 Atom System Image: Connection reset.
[=======                                ] 19% Downloading x86_64-34_r04.zip...
```

**La démonstration se fait donc avec deux appareils.** Tout ce que l'énoncé demande de montrer reste démontrable : lire la liste détaillée, désigner un appareil, installer dessus, puis sur l'autre en ne changeant qu'un mot, et enfin faire disparaître un appareil de la liste de Jenga sans qu'il disparaisse de celle d'`adb`.

## La liste, brute puis détaillée

```text
jenga deploy --platform android --list-devices
List of devices attached
105453739L108475        device
R92X51YQD1P     device
```

```text
jenga deploy --platform android --list-devices --detailed
SERIAL            MARQUE   MODELE      ANDROID  ABI
----------------  -------  ----------  -------  ---------
105453739L108475  TECNO    TECNO KI5k  12       arm64-v8a
R92X51YQD1P       samsung  SM-A055F    15       arm64-v8a
```

**Ce que j'explique :** la première liste ne donne qu'un numéro de série et un état. Impossible de savoir lequel est lequel. La seconde interroge chaque appareil et ajoute la marque, le modèle, la version d'Android et l'architecture. C'est elle qui permet de **choisir**.

## La classe désigne l'appareil

Je n'ai pas de casque : la classe a donc désigné, parmi les deux appareils listés, celui qui **joue le rôle du casque** pour la démonstration. Pour cette démo, la classe était jouée par un assistant IA.

> **Sa réponse : `105453739L108475`, le TECNO.** Deux raisons données : c'est le seul appareil sur lequel le programme a déjà été déployé et mesuré tout au long du chapitre, et sa version d'Android, la 12, est la plus proche de celle d'un Quest, qui repose sur un Android plus ancien que le Samsung en Android 15. L'ABI, en revanche, ne départage pas : les deux sont en `arm64-v8a`, comme un casque.

Retenir un numéro de série pareil est impossible, mais ce n'est pas le but : on le lit dans la liste détaillée, on le copie, et c'est **le seul mot qui changera** entre les deux commandes qui suivent.

## Installer sur l'appareil désigné, puis sur l'autre

L'APK est celui signé à la démo 5. Les deux commandes sont **identiques à un mot près**, le numéro de série.

**Sur le TECNO, l'appareil désigné :**

```text
jenga deploy --platform android --apk ..\demo5-du_bureau_a_la_main\dist\MaSalle.apk --target 105453739L108475 --run
Performing Incremental Install
Performing Streamed Install
Success
APK installed: ...\demo5-du_bureau_a_la_main\dist\MaSalle.apk
Launching com.mafo.masalle...
App launched: com.mafo.masalle
```

**Sur le Samsung, en ne changeant que le numéro de série :**

```text
jenga deploy --platform android --apk ..\demo5-du_bureau_a_la_main\dist\MaSalle.apk --target R92X51YQD1P --run
Performing Incremental Install
Performing Streamed Install
Success
APK installed: ...\demo5-du_bureau_a_la_main\dist\MaSalle.apk
Launching com.mafo.masalle...
App launched: com.mafo.masalle
```

Les deux écrans deviennent bleu-vert, l'un après l'autre. Le même paquet, non reconstruit, fonctionne sur les deux téléphones : ils partagent la même ABI, `arm64-v8a`.

## Révoquer l'autorisation d'un appareil

Sur le Samsung : débranché, **Options de développement → Révoquer les autorisations de débogage USB**, rebranché, et sans toucher à la fenêtre « Autoriser le débogage USB ? ».

**Ce que dit `adb` :**

```text
C:\Android\platform-tools\adb.exe devices
List of devices attached
105453739L108475        device
R92X51YQD1P     unauthorized
```

**Ce que dit Jenga :**

```text
jenga deploy --platform android --list-devices --detailed
SERIAL            MARQUE  MODELE      ANDROID  ABI
----------------  ------  ----------  -------  ---------
105453739L108475  TECNO   TECNO KI5k  12       arm64-v8a
```

Le Samsung est **toujours branché**, `adb` le voit et le dit : `unauthorized`. Mais il a **disparu de la liste détaillée de Jenga**, sans le moindre message.

**Pourquoi :** la commande détaillée ne garde que les appareils à l'état `device`, c'est-à-dire branchés **et** autorisés. Un appareil `unauthorized` ou `offline` est écarté en silence. Si le Samsung avait été le seul branché, Jenga aurait affiché `No Android device connected.`, ce qui aurait laissé croire qu'aucun appareil n'est relié, alors qu'il attendait simplement une autorisation (c'est ce que montrait l'exercice 27).

## Ce que la démo montre

1. **Avec plusieurs appareils, il faut désigner.** Sans `--target`, `adb` refuse : `more than one device/emulator`.
2. **La liste détaillée est ce qui rend le choix possible** : elle relie un numéro de série illisible à une marque, un modèle, une version d'Android et une ABI.
3. **Changer d'appareil, c'est changer un mot.** Rien d'autre dans la commande, et surtout pas une reconstruction.
4. **Deux listes ne disent pas la même chose.** Celle d'`adb` montre tout ce qui est branché, avec son état ; celle de Jenga ne montre que ce qui est utilisable. Quand un appareil « disparaît », il faut aller voir la première avant de soupçonner le câble.
