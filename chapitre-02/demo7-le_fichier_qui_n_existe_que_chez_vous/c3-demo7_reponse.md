# Chapitre 02 — Démo 7 : le fichier qui n'existe que chez vous

## Le montage

Une ressource, `Resources/salle.txt`, 61 octets, dont la première ligne est `Salle 1 : le plan de MaSalle`. Le programme la lit **deux fois, avec le même chemin** :

1. directement avec `fopen("Resources/salle.txt", "rb")` ;
2. à travers un petit système de fichiers, bâti sur le modèle du module NKFileSystem du moteur.

Dans `MaSalle.jenga`, le filtre Android contient une ligne de plus :

```python
            androidassets(["Resources"])    # le dossier est copie dans l'APK
```

Même fichier source, même chemin, même ressource : seule la machine change.

Environnement : Windows x86_64, Jenga 2.8.0, et un TECNO SPARK 10C (Android 12, `arm64-v8a`).

## Premier temps : sur ma machine

```text
jenga clean --config Release
jenga build --config Release
jenga run MaSalle --config Release
```

La boîte de message affiche :

```text
Lecture de la ressource Resources/salle.txt

fopen("Resources/salle.txt") : OK - 61 octets, 1re ligne : "Salle 1 : le plan de MaSalle"

systeme de fichiers -> disque : fopen("Resources/salle.txt") : OK - 61 octets, 1re ligne : "Salle 1 : le plan de MaSalle"
```

Les deux lectures réussissent. Tout va bien, et c'est justement le piège : **sur ma machine, rien ne laisse deviner le problème.**

## Deuxième temps : sur l'appareil

Même programme, construit pour Android, signé, installé, puis lancé :

```text
jenga clean --platform android-arm64 --config Release
jenga build --platform android-arm64 --config Release
"C:\Android\build-tools\34.0.0\apksigner.bat" sign --ks "%USERPROFILE%\cles\masalle.jks" --ks-key-alias masalle --out dist\MaSalle.apk build\bin\Android\Release\android-build-arm64-v8a\MaSalle-Release.apk
C:\Android\platform-tools\adb.exe install -r dist\MaSalle.apk
Success
```

```text
C:\Android\platform-tools\adb.exe logcat -d -s MaSalle
--------- beginning of main
09-23 09:54:50.947 16925 16947 I MaSalle : Lecture de la ressource Resources/salle.txt
09-23 09:54:50.947 16925 16947 I MaSalle : fopen("Resources/salle.txt") : ECHEC — errno 2 (No such file or directory)
09-23 09:54:50.947 16925 16947 I MaSalle : systeme de fichiers ("Resources/salle.txt") : OK — 61 octets, 1re ligne : "Salle 1 : le plan de MaSalle"
```

| | Sur ma machine | Sur le téléphone |
|---|---|---|
| `fopen("Resources/salle.txt")` | **OK**, 61 octets | **ECHEC — errno 2 (No such file or directory)** |
| système de fichiers | OK, 61 octets | OK, 61 octets |

## La question posée à la classe

> Le fichier est bien dans le paquet, le programme est le même, le chemin est le même, et la seconde lecture réussit sur les deux machines. Pourquoi la **première** échoue-t-elle sur le téléphone ?

## Les hypothèses qu'on entend, et pourquoi elles sont fausses

| Hypothèse | Pourquoi ce n'est pas ça |
|---|---|
| « Le fichier n'a pas été mis dans le paquet. » | Il y est : la liste du contenu de l'APK, plus bas, le montre, avec ses 61 octets. Et la seconde lecture le trouve, donc il est bien là. |
| « C'est une histoire de permissions : Android interdit de lire des fichiers. » | Non, l'erreur serait `errno 13 (Permission denied)`. Ici c'est `errno 2`, « fichier inexistant ». Et le programme n'essaie de lire aucun fichier personnel de l'utilisateur. |
| « Le nom du fichier est mal écrit, ou la casse ne correspond pas. » | C'est **le même chemin** que la seconde lecture, qui réussit, dans le même programme, à la même seconde. |
| « Il faut demander une permission de stockage dans le manifeste. » | Cela concerne la mémoire partagée du téléphone, photos et documents. Une ressource livrée avec l'application n'a rien à voir. |
| « Le programme est lancé depuis un autre dossier, il faudrait un chemin absolu. » | On s'en approche : le dossier courant n'est en effet pas celui du projet. Mais même avec un chemin absolu, cela échouerait : **aucun chemin** ne mène à cette ressource. |

## L'explication : où vivent les ressources dans un paquet

```text
tar -tvf dist\MaSalle.apk
-rw-rw-r--  0 0      0        2196 janv. 01  1980 AndroidManifest.xml
-rw-rw-r--  0 0      0          40 janv. 01  1980 resources.arsc
-rw-rw-r--  0 0      0      490792 sept. 23 09:45 lib/arm64-v8a/libMaSalle.so
-rw-rw-r--  0 0      0          61 sept. 23 09:40 assets/salle.txt
-rw-rw-r--  0 0      0         516 sept. 23 09:45 META-INF/MASALLE.SF
-rw-rw-r--  0 0      0        1242 sept. 23 09:45 META-INF/MASALLE.RSA
-rw-rw-r--  0 0      0         389 sept. 23 09:45 META-INF/MANIFEST.MF
```

La ressource est là, ses 61 octets exacts, mais regardez **où** : `assets/salle.txt`, **à l'intérieur de l'archive**, et non plus dans un dossier `Resources`.

- **Sur mon ordinateur**, `Resources/salle.txt` est un vrai fichier, posé sur un vrai disque, dans le dossier du projet, et le programme est lancé depuis ce dossier. `fopen` suit le chemin et le trouve.
- **Sur le téléphone**, l'application n'est pas un dossier de fichiers : c'est **un seul fichier**, l'APK, une archive zip que le système garde compressée. `assets/salle.txt` n'existe pas sur le système de fichiers : c'est une entrée dans une archive. Aucun chemin ne mène à lui, ni relatif ni absolu, et `fopen` ne sait pas ouvrir ce qui est à l'intérieur d'une archive. D'où `errno 2` : pour le système, ce fichier n'existe pas.

**Comment la seconde lecture y arrive.** Elle ne passe pas par le système de fichiers, mais par l'`AAssetManager` d'Android, qui sait ouvrir les entrées `assets/` de l'APK. Elle retire aussi le préfixe `Resources/` du chemin, puisque `androidassets` a copié le **contenu** du dossier à la racine de `assets/`. Le code appelant, lui, écrit toujours le même chemin.

C'est exactement le rôle du module de fichiers d'un moteur, comme NKFileSystem dans Nkentseu : un seul chemin dans le code, résolu différemment selon la plateforme.

## Ce que la démo montre

1. **Un chemin n'est pas une ressource.** Sur un ordinateur, les deux se confondent, et c'est ce qui rend l'erreur invisible pendant tout le développement.
2. **L'erreur n'apparaît qu'à l'exécution, sur l'appareil.** La construction réussit, le paquet contient bien le fichier, l'installation réussit : rien ne prévient.
3. **`errno` dit lequel des deux problèmes on a** : `2` signifie « ce chemin ne mène à rien », `13` signifierait « il existe, mais on n'a pas le droit ». Ce sont deux enquêtes très différentes.
4. **Il faut passer par le système de fichiers du moteur dès le premier fichier lu**, sans attendre : le jour où l'on charge une texture ou un shader sur le casque, il sera trop tard pour s'en apercevoir.
