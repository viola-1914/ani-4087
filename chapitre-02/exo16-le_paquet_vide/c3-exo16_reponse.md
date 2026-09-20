# Chapitre 02 — Exo 16 : le paquet vide

## Le programme empaqueté

Le projet est `MaSalle.jenga`, celui des exercices 12 et 14, avec son filtre Android. Sous Android, `src/main.cpp` ne fait qu'une chose : il remplit l'écran d'une seule couleur, un bleu-vert. C'est une application native (`NativeActivity` et `android_native_app_glue`), sans une ligne de Java. Sous Windows, le même fichier ouvre toujours la boîte de salutation de l'exercice 12.

## Ce qu'il a fallu installer

L'exercice 13 l'avait montré : ma machine n'avait aucune chaîne pour Android. J'ai installé les outils Android en ligne de commande dans `C:\Android` :

```text
sdkmanager --sdk_root=C:\Android --licenses
sdkmanager --sdk_root=C:\Android "platform-tools"
sdkmanager --sdk_root=C:\Android "platforms;android-34"
sdkmanager --sdk_root=C:\Android "build-tools;34.0.0"
sdkmanager --sdk_root=C:\Android "ndk;26.3.11579264"
setx ANDROID_SDK_ROOT "C:\Android"
setx ANDROID_NDK_ROOT "C:\Android\ndk\26.3.11579264"
setx JAVA_HOME "C:\Program Files\Eclipse Adoptium\jdk-21.0.12.101-hotspot"
```

Après cela, `jenga info -v` affiche une troisième ligne dans `Available Toolchains` :

```text
android-ndk   android-ndk   Android     arm64    android
```

## Empaqueter

Depuis `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo16-le_paquet_vide`, dans l'Invite de commandes Windows :

```text
jenga package --platform android --config Release
```

```text
✓   [1/1] Compiled: main.cpp
✓ Built: build\bin\Android\Release\libMaSalle.so
✓ Build Successful
ℹ Building APK for MaSalle (x86_64)
⚠ Debug keystore not found at C:\Users\HP\.android\debug.keystore - APK ne sera PAS signe ; Android refusera l'install.
✓ APK generated: ...\build\bin\Android\Release\android-build-x86_64\MaSalle-Release.apk
APK packaged: ...\dist\MaSalle.apk
```

L'avertissement est attendu : Jenga signe d'office avec une clé de debug, et il n'y en a pas sur ma machine. Je signe donc moi-même, avec ma clé de l'exercice 15.

## Signer

```text
"C:\Android\build-tools\34.0.0\apksigner.bat" sign --ks "%USERPROFILE%\cles\masalle.jks" --ks-key-alias masalle --out dist\MaSalle-signe.apk dist\MaSalle.apk
Keystore password for signer #1:
```

Le mot de passe n'est pas dans la commande : `apksigner` le demande au clavier, sans l'afficher. Il est noté dans mon carnet (exercice 15). J'ai préféré `apksigner` à `jenga sign`, qui attend le mot de passe en argument de commande ou dans le `.jenga`, donc à un endroit où il finirait dans l'historique ou dans le dépôt.

Vérification de la signature :

```text
"C:\Android\build-tools\34.0.0\apksigner.bat" verify --print-certs dist\MaSalle-signe.apk
Signer #1 certificate DN: CN=Mafo
Signer #1 certificate SHA-256 digest: 2ff7e5fa576ee89835fbb9e014d1748dc91e90ad3b25ce426ada23b2a6cf1e0d
Signer #1 certificate SHA-1 digest: a928559899264e6dfd2d625f80aa518f2d0c8ce3
Signer #1 certificate MD5 digest: 5cfa3a2069136d445c9913a78a1f3e84
```

Le certificat porte bien mon nom, `CN=Mafo`.

## La taille du paquet

```text
dir dist
20/09/2026  13:20            16 798 MaSalle-signe.apk
20/09/2026  13:20             5 474 MaSalle-signe.apk.idsig
20/09/2026  13:16             9 266 MaSalle.apk
```

**Le paquet signé pèse 16 798 octets, soit environ 16,4 Ko.** Avant signature, il pesait 9 266 octets : la signature ajoute 7 532 octets, presque autant que le programme lui-même. Le fichier `MaSalle-signe.apk.idsig`, produit à côté par `apksigner`, est une signature séparée (schéma v4) qui sert à l'installation incrémentale ; il ne fait pas partie de l'APK.

## Ce qu'il contient

Un APK est une archive zip ordinaire : `tar` sait la lire.

```text
tar -tvf dist\MaSalle-signe.apk
-rw-rw-r--  0 0      0        2196 janv. 01  1980 AndroidManifest.xml
-rw-rw-r--  0 0      0          40 janv. 01  1980 resources.arsc
-rw-rw-r--  0 0      0       21992 sept. 20 13:16 lib/x86_64/libMaSalle.so
-rw-rw-r--  0 0      0         425 sept. 20 13:16 META-INF/MASALLE.SF
-rw-rw-r--  0 0      0        1101 sept. 20 13:16 META-INF/MASALLE.RSA
-rw-rw-r--  0 0      0         298 sept. 20 13:16 META-INF/MANIFEST.MF
```

| Fichier | Taille (non compressée) | Rôle |
|---|---:|---|
| `AndroidManifest.xml` | 2 196 o | la carte d'identité de l'application : identifiant `com.mafo.masalle`, versions d'Android, activité native |
| `resources.arsc` | 40 o | la table des ressources, presque vide : l'application n'a ni image ni texte |
| `lib/x86_64/libMaSalle.so` | 21 992 o | **mon programme**, compilé par le NDK en bibliothèque partagée ; c'est le seul fichier qui vient de mon code |
| `META-INF/MASALLE.SF` | 425 o | la liste signée des empreintes des fichiers (signature v1, au nom de l'alias `masalle`) |
| `META-INF/MASALLE.RSA` | 1 101 o | le certificat `CN=Mafo` et la signature elle-même |
| `META-INF/MANIFEST.MF` | 298 o | l'empreinte de chaque fichier de l'archive |

Il n'y a aucun `classes.dex` : sans Java, il n'y a pas de code pour la machine virtuelle d'Android. Les signatures plus récentes (v2 et v3) ne sont pas des fichiers de l'archive : `apksigner` les place dans un bloc à part, à l'intérieur du fichier. C'est ce qui explique qu'il grossisse plus que les trois fichiers de `META-INF`.

## Ce que ce paquet ne fait pas encore

Le dossier `lib/` ne contient que **`x86_64`**, l'architecture des émulateurs Android. Jenga a pris la première architecture déclarée dans mon workspace (`targetarchs([TargetArch.X86_64, TargetArch.ARM64])`), comme l'annonçait déjà la ligne `Building APK for MaSalle (x86_64)`. Ce paquet ne s'installerait donc pas sur le Quest 2, qui attend `lib/arm64-v8a/`. Il faudra viser `arm64` pour les chapitres où l'application ira sur le casque.

Je n'ai pas non plus installé l'APK sur un appareil ou un émulateur : je n'ai pas vu l'écran se remplir de la couleur. Ce qui est vérifié ici, c'est que le programme compile pour Android, que le paquet est fabriqué, qu'il est signé avec ma clé, et ce qu'il contient.
