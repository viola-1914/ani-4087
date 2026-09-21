# Chapitre 02 — Exo 22 : les deux formats

## Le programme

Le même `MaSalle` que depuis l'exercice 18 : il remplit l'écran d'un bleu-vert et écrit trois lignes dans le journal. Une seule construction pour les deux paquets :

```text
jenga clean --platform android-arm64 --config Release
jenga build --platform android-arm64 --config Release
✓ Built: build\bin\Android\Release\libMaSalle.so
✓ APK generated: ...\build\bin\Android\Release\android-build-arm64-v8a\MaSalle-Release.apk
```

## Produire l'APK

Jenga fabrique l'APK pendant le build ; je le signe avec ma clé :

```text
"C:\Android\build-tools\34.0.0\apksigner.bat" sign --ks "%USERPROFILE%\cles\masalle.jks" --ks-key-alias masalle --out dist\MaSalle.apk build\bin\Android\Release\android-build-arm64-v8a\MaSalle-Release.apk
```

## Produire l'AAB

### Ce que fait Jenga

```text
jenga package --platform android --type aab --config Release
ℹ Building AAB for MaSalle (x86_64)
bundletool not found. Please install via SDK Manager.
```

Trois problèmes, dont deux que j'ai trouvés en lisant le code de la commande (`BuildAAB` dans `Jenga/Core/Builders/Android.py`) :

1. **`bundletool` manque.** C'est l'outil de Google qui fabrique les AAB, et il n'est **pas** fourni avec le SDK, contrairement à ce que dit le message.
2. **Même avec `bundletool`, cela ne marcherait pas.** Jenga lui passe un **dossier**, avec un manifeste au format binaire des APK ; `bundletool` attend un **module ZIP** dont le manifeste et les ressources sont au format « proto » (protobuf).
3. **La signature prévue est la mauvaise.** Jenga signerait l'AAB avec `apksigner`, qui ne sait signer que des APK ; un AAB se signe avec `jarsigner`.

Et comme `jenga package` ne permet pas de choisir l'architecture, il construit pour `x86_64`.

### Ce que j'ai fait à la place

J'ai téléchargé `bundletool` (version 1.18.3, dans `tools/`, non versionné) et écrit un petit script, `faire_aab.py`, qui fait les trois étapes correctement :

1. `aapt2 link --proto-format` : manifeste et ressources au format proto ;
2. assemblage du module `base.zip` : `manifest/AndroidManifest.xml`, `resources.pb`, `lib/arm64-v8a/libMaSalle.so` ;
3. `java -jar bundletool-all-1.18.3.jar build-bundle --modules=base.zip --output=dist\MaSalle.aab`.

Puis la signature :

```text
python faire_aab.py
Architecture de libMaSalle.so : AArch64 (arm64-v8a)
...
AAB produit : ...\dist\MaSalle.aab (9254 octets)

jarsigner -keystore "%USERPROFILE%\cles\masalle.jks" dist\MaSalle.aab masalle
jar signed.
Warning:
The signer's certificate is self-signed.
```

L'avertissement est attendu : ma clé est personnelle, elle n'a pas été délivrée par une autorité.

### Un piège rencontré en route

Mon premier AAB contenait un `libMaSalle.so` de **22 752 octets**, contre **23 440** dans l'APK. En cause : le `jenga package --type aab` lancé entre-temps avait **recompilé la bibliothèque pour x86_64, au même endroit** (`build\bin\Android\Release\libMaSalle.so`) que la version arm64. Mon script avait alors rangé une bibliothèque x86_64 dans `lib/arm64-v8a/` : un paquet qui aurait planté sur le téléphone. J'ai ajouté au script une vérification de l'en-tête ELF de la bibliothèque (octets 18-19 : 183 pour AArch64, 62 pour x86-64), reconstruit en arm64, et refait l'AAB. Les deux paquets contiennent maintenant la même bibliothèque, de 23 440 octets.

## Les tailles

```text
dir dist
21/09/2026  10:14            11 852 MaSalle.aab
21/09/2026  09:55            16 801 MaSalle.apk
21/09/2026  09:55             5 568 MaSalle.apk.idsig
```

| | APK | AAB |
|---|---:|---:|
| Taille signée | **16 801 octets** | **11 852 octets** |
| Avant signature | — | 9 254 octets |
| Même `libMaSalle.so` | 23 440 octets | 23 440 octets |

Ici, l'AAB est plus petit que l'APK, pour une raison simple : l'APK porte en plus le bloc de signature des schémas v2 et v3 d'`apksigner`, qui n'est pas un fichier de l'archive mais pèse plusieurs kilo-octets. Avec un seul programme et une seule architecture, la différence de taille ne dit rien de l'intérêt de l'AAB : elle vient des signatures, pas du contenu.

## Les contenus

**L'APK :**

```text
tar -tvf dist\MaSalle.apk
-rw-rw-r--  0 0      0        2196 janv. 01  1980 AndroidManifest.xml
-rw-rw-r--  0 0      0          40 janv. 01  1980 resources.arsc
-rw-rw-r--  0 0      0       23440 sept. 21 09:54 lib/arm64-v8a/libMaSalle.so
-rw-rw-r--  0 0      0         428 sept. 21 09:54 META-INF/MASALLE.SF
-rw-rw-r--  0 0      0        1242 sept. 21 09:54 META-INF/MASALLE.RSA
-rw-rw-r--  0 0      0         301 sept. 21 09:54 META-INF/MANIFEST.MF
```

**L'AAB :**

```text
tar -tvf dist\MaSalle.aab
-rw-rw-r--  0 0      0         653 sept. 21 10:14 META-INF/MANIFEST.MF
-rw-rw-r--  0 0      0         858 sept. 21 10:14 META-INF/MASALLE.SF
-rw-rw-r--  0 0      0        1413 sept. 21 10:14 META-INF/MASALLE.RSA
-rw-rw-r--  0 0      0          10 janv. 01  2010 BundleConfig.pb
-rw-rw-r--  0 0      0       23440 janv. 01  2010 base/lib/arm64-v8a/libMaSalle.so
-rw-rw-r--  0 0      0        1723 janv. 01  2010 base/manifest/AndroidManifest.xml
-rw-rw-r--  0 0      0          23 janv. 01  2010 base/native.pb
-rw-rw-r--  0 0      0          86 janv. 01  2010 base/resources.pb
```

| | APK | AAB |
|---|---|---|
| Organisation | à plat, prêt à installer | un dossier par **module** (`base/`) |
| Manifeste | `AndroidManifest.xml`, XML binaire (2 196 o) | `base/manifest/AndroidManifest.xml`, format proto (1 723 o) |
| Table des ressources | `resources.arsc` | `base/resources.pb` (proto) |
| Mon programme | `lib/arm64-v8a/libMaSalle.so` | `base/lib/arm64-v8a/libMaSalle.so`, identique |
| Description des bibliothèques natives | — | `base/native.pb` |
| Réglages du paquet | — | `BundleConfig.pb` |
| Signature | v1 (`META-INF`) + v2/v3 (bloc de signature) + v4 (`.idsig`) | signature JAR (`META-INF`) seulement |

## Ce que l'AAB contient que l'APK n'a pas

- **`BundleConfig.pb`** : la configuration du paquet, pour `bundletool` et la boutique : comment découper l'application, avec quelle version de l'outil elle a été construite.
- **`base/native.pb`** : la description des bibliothèques natives, rangées par architecture. C'est ce qui permet d'envoyer à chaque appareil **uniquement** la bibliothèque de son architecture.
- **Des modules** : tout est rangé sous `base/`. Un AAB peut contenir d'autres modules à côté, des fonctionnalités téléchargées plus tard, à la demande.
- **Le format proto** pour le manifeste et les ressources : un format de travail, que la boutique relit pour **fabriquer** des APK.

Et une chose qu'il n'a pas : on **ne peut pas l'installer**. Un téléphone ne sait pas lire un AAB. C'est une matière première, pas un paquet final.

## Pourquoi une boutique le préfère

Un APK est le paquet final, le même pour tous les appareils : il doit contenir les bibliothèques de **toutes** les architectures, les images de **toutes** les densités d'écran, les textes de **toutes** les langues. Chaque utilisateur télécharge tout, et n'en utilise qu'une partie.

Un AAB laisse la boutique fabriquer elle-même les APK, **pour chaque appareil** : elle découpe le paquet et n'envoie à mon TECNO SPARK 10C que la bibliothèque `arm64-v8a`, les images de sa densité d'écran et les textes de sa langue. Le téléchargement est plus petit et l'installation plus légère. C'est aussi ce qui permet de livrer certains modules seulement quand l'utilisateur en a besoin.

Il y a une contrepartie : puisque c'est la boutique qui fabrique les APK, c'est elle qui les **signe**, avec la clé de l'application qu'on lui confie ; ma propre clé ne sert plus qu'à prouver que c'est bien moi qui envoie l'AAB. Google Play exige d'ailleurs le format AAB pour les nouvelles applications.

Dans mon cas, avec une seule architecture et aucune ressource, ce gain est invisible : les deux formats transportent la même bibliothèque. Il deviendrait réel le jour où l'application viserait plusieurs architectures et contiendrait des images, des sons et des traductions.

## Les fichiers de ce dossier

- `faire_aab.py` : le script qui fabrique l'AAB ;
- `MaSalle.jenga`, `src/main.cpp` : le projet ;
- `.gitignore` : exclut `dist/`, les paquets et `tools/*.jar`, donc ni les paquets ni `bundletool` ne sont dans le dépôt.
