# Chapitre 02 — Exo 24 : le journal de bord du portage

Ce journal liste tout ce qui a cassé entre mon premier `jenga build` pour Android et le premier écran bleu-vert sur mon téléphone, puis pendant les exercices qui ont suivi. Pour chaque entrée : le **symptôme**, ce que j'ai **cru**, ce que c'**était**, **ce que j'ai fait** pour m'en sortir, et le **temps perdu**.

Les temps sont reconstitués après coup, à partir de mes sorties de terminal et des horodatages du journal Android, et non chronométrés sur le moment : ils sont donnés à la dizaine de minutes près (≈).

Machine : Windows x86_64, Jenga 2.8.0, chaîne `mingw`. Appareil : TECNO SPARK 10C, Android 12, `arm64-v8a`.

---

## 1. Aucune chaîne pour Android

- **Symptôme :** `jenga compile-flags --platform android-arm64` → `No suitable toolchain found for Android arm64`. `jenga info -v` ne listait que `host-gcc` et `mingw` (exercices 13 et 14).
- **Ce que j'ai cru :** que Jenga savait construire pour Android dès l'installation.
- **Ce que c'était :** il faut installer soi-même le SDK Android, les build-tools, la plateforme et le NDK, puis déclarer `ANDROID_SDK_ROOT`, `ANDROID_NDK_ROOT` et `JAVA_HOME`. Jenga ne fait que les trouver.
- **Ce que j'ai fait :** J'ai installé les outils Android en ligne de commande dans `C:\Android`, accepté les licences, installé `platform-tools`, `platforms;android-34`, `build-tools;34.0.0` et `ndk;26.3.11579264` un par un pour ne relancer que ce qui échouait sur ma connexion, déclaré les trois variables avec `setx`, puis vérifié avec `jenga info -v` que la ligne `android-ndk` apparaissait.
- **Temps perdu :** ≈ 1 h 30, surtout en téléchargements sur une connexion instable (le NDK seul fait environ 700 Mo).

## 2. Java absent, puis « toujours absent » après l'installation

- **Symptôme :** `where.exe keytool` → introuvable. Après `winget install EclipseAdoptium.Temurin.21.JDK` (téléchargement bloqué une première fois à 106 Mo sur 171), toujours introuvable.
- **Ce que j'ai cru :** que l'installation avait échoué.
- **Ce que c'était :** l'installation avait réussi, mais la fenêtre de terminal ouverte **avant** l'installation ne voyait pas le nouveau `PATH`. Il suffisait de la fermer et d'en rouvrir une.
- **Ce que j'ai fait :** J'ai relancé le téléchargement interrompu, rouvert un terminal neuf et revérifié avec `where.exe keytool` jusqu'à obtenir le chemin du JDK.
- **Temps perdu :** ≈ 30 min (dont le téléchargement à relancer).

## 3. Une commande coupée en deux

- **Symptôme :** `jenga keygen: error: argument --output/-o: expected one argument`.
- **Ce que j'ai cru :** que la commande était fausse.
- **Ce que c'était :** j'avais collé la commande sans sa fin : le chemin après `--output` manquait.
- **Ce que j'ai fait :** J'ai retapé la commande en entier sur une seule ligne, guillemets compris.
- **Temps perdu :** ≈ 5 min.

## 4. Le paquet refusé : la mauvaise architecture

- **Symptôme :** `adb install` → `INSTALL_FAILED_NO_MATCHING_ABIS: Failed to extract native libraries, res=-113` (exercice 17).
- **Ce que j'ai cru :** que Jenga prenait « la première architecture déclarée » dans `targetarchs`.
- **Ce que c'était :** `jenga package` ne permet pas de choisir l'architecture et prend celle **de ma machine**, `x86_64`, parce qu'elle figure dans `targetarchs`. Le paquet ne contenait que `lib/x86_64/`, alors que le téléphone attend `arm64-v8a`. La solution : `jenga build --platform android-arm64`, qui produit aussi l'APK.
- **Ce que j'ai fait :** J'ai vérifié les architectures acceptées par mon téléphone avec `adb shell getprop ro.product.cpu.abilist`, reconstruit avec `--platform android-arm64`, contrôlé le contenu du paquet avec `tar -tvf`, puis réinstallé : `Success`.
- **Temps perdu :** ≈ 20 min.

## 5. Le mot de passe de la clé perdu

- **Symptôme :** `keystore password was incorrect`, trois fois de suite, avec `apksigner` puis `keytool -list`, alors que la même clé avait signé l'APK de l'exercice 16.
- **Ce que j'ai cru :** une faute de frappe.
- **Ce que c'était :** le mot de passe tapé ne correspondait plus à celui de la clé, très probablement à cause de la rangée des chiffres du clavier AZERTY, invisible dans une saisie masquée. Irrécupérable : il a fallu supprimer la clé et en recréer une, avec un mot de passe fait de lettres minuscules seulement, noté aussitôt et vérifié avec `keytool -list`.
- **Au passage :** répondre `y` à `Keystore already exists. Overwrite?` n'efface pas l'ancienne clé ; `keytool` aurait échoué de nouveau. Il fallait `del` le fichier d'abord.
- **Ce que j'ai fait :** J'ai testé le mot de passe seul avec `keytool -list` pour isoler le problème, supprimé la clé, recréé une nouvelle clé au même endroit, noté le nouveau mot de passe dans mon carnet et vérifié immédiatement qu'il fonctionnait.
- **Temps perdu :** ≈ 40 min.

## 6. L'application se ferme dès qu'on la touche

- **Symptôme :** l'icône `MaSalle` apparaît, l'application s'ouvre et se ferme aussitôt. `adb logcat -d -b crash` : `UnsatisfiedLinkError ... dlopen failed: library "libc++_shared.so" not found` (exercice 17, premier plantage à 14:27:57).
- **Ce que j'ai cru :** que l'installation s'était mal passée.
- **Ce que c'était :** le NDK relie par défaut le C++ à une bibliothèque standard **séparée**, `libc++_shared.so`, que Jenga n'avait pas mise dans l'APK. Correction : `androidstl("c++_static")` dans le filtre Android, qui l'intègre dans `libMaSalle.so`. Vérification avant d'installer : `llvm-readelf -d libMaSalle.so | findstr NEEDED` ne doit plus citer `libc++_shared.so`.
- **Ce que j'ai fait :** J'ai récupéré la trace exacte avec `adb logcat -d -b crash`, ajouté `androidstl("c++_static")` à mon `MaSalle.jenga`, et vérifié avec `llvm-readelf` que `libc++_shared.so` avait disparu **avant** de réinstaller.
- **Temps perdu :** ≈ 25 min.

## 7. La correction ne change rien : le cache

- **Symptôme :** après avoir ajouté `androidstl("c++_static")`, même plantage. Le build disait `✓ All files up to date`.
- **Ce que j'ai cru :** que la correction était mauvaise, ou que je l'avais mal écrite.
- **Ce que c'était :** Jenga ne recompile que si un **fichier source** change ; il ne remarque pas qu'un **réglage d'édition de liens** a changé. Il avait gardé l'ancienne bibliothèque. Solution : `jenga clean --platform android-arm64` avant de reconstruire.
- **Ce que j'ai fait :** J'ai relu la sortie du build, repéré `All files up to date`, lancé `jenga clean`, reconstruit, et obtenu l'écran bleu-vert, que j'ai photographié.
- **Temps perdu :** ≈ 20 min. C'est la même famille de piège qu'à l'exercice 12 (entrée 12).

## 8. « Débogage USB » non autorisé

- **Symptôme :** `adb devices` → `105453739L108475   unauthorized`.
- **Ce que j'ai cru :** que le câble ou le téléphone ne marchait pas.
- **Ce que c'était :** il fallait accepter la fenêtre « Autoriser le débogage USB ? » sur le téléphone déverrouillé.
- **Ce que j'ai fait :** J'ai autorisé le débogage USB sur le téléphone et relancé `adb devices` jusqu'à lire `device`.
- **Temps perdu :** ≈ 5 min.

## 9. Le journal vide : des commandes qui n'ont jamais tourné

- **Symptôme :** `adb logcat -d -s MaSalle` n'affichait pas les nouvelles lignes, et `adb logcat -d -b crash` était vide alors que le programme devait planter (exercices 18 et 19).
- **Ce que j'ai cru :** que le programme ne s'exécutait pas, ou que le filtre était faux.
- **Ce que c'était :** j'avais collé plusieurs commandes à la fois. `adb install -r` avait été sauté, donc le téléphone faisait tourner **l'ancienne version** ; et `monkey`, collé avec `force-stop`, ne s'était jamais exécuté (il n'avait rien affiché). Leçon : une commande à la fois, et vérifier que chacune a produit sa sortie.
- **Ce que j'ai fait :** J'ai repris les commandes une par une, en attendant chaque fois la sortie attendue (`Success`, la longue sortie de `monkey`), et obtenu mes trois lignes de journal puis la trace de plantage.
- **Temps perdu :** ≈ 30 min sur les deux exercices.

## 10. Le bruit qui fait peur : les « tombstones »

- **Symptôme :** `monkey` affichait des dizaines de lignes `** New tombstone found: /data/tombstones/...` et `First NativeCrash Time`, même quand mon application allait bien.
- **Ce que j'ai cru :** que mon application plantait.
- **Ce que c'était :** ces fichiers sont les rapports de plantage que le téléphone conserve, pour toutes les applications et depuis longtemps. Seul `adb logcat -d -b crash`, filtré sur `com.mafo.masalle`, dit si **mon** programme a planté.
- **Ce que j'ai fait :** J'ai appris à ignorer ces lignes et à ne croire que `adb logcat -d -b crash`.
- **Temps perdu :** ≈ 10 min d'inquiétude.

## 11. Le fichier introuvable sur l'appareil

- **Symptôme :** `fopen("Resources/salle.txt")` → `errno 2 (No such file or directory)` sur le téléphone, alors que le même code lisait le fichier sur l'ordinateur (exercice 20).
- **Ce que j'aurais cru sans l'exercice :** qu'un chemin relatif marche partout.
- **Ce que c'était :** sur Android, la ressource est rangée **dans l'APK**, qui est une archive ; aucun chemin du système de fichiers ne mène à elle. Il faut passer par l'`AAssetManager`, comme le fait le module NKFileSystem du moteur.
- **Ce que j'ai fait :** J'ai fait lire la ressource par un petit système de fichiers sur le modèle de NKFileSystem, avec l'`AAssetManager`, et obtenu la lecture sur les deux plateformes : 61 octets de chaque côté.
- **Temps perdu :** 0, c'était prévu. Je le garde parce que ce sera un vrai piège au premier chargement de texture.

## 12. Le programme ne fait rien : le fichier n'était pas enregistré

- **Symptôme :** la boîte de salutation ne s'ouvrait pas ; le programme se terminait en 0,06 s (exercice 12, sur l'ordinateur).
- **Ce que j'ai cru :** d'abord que `jenga run` ne recompilait pas, puis que la date du fichier extrait du zip trompait le cache. Les deux hypothèses étaient fausses.
- **Ce que c'était :** le nouveau `main.cpp` était ouvert dans VS Code **sans être enregistré** ; le fichier sur le disque était toujours l'ancien. L'avertissement `WIN32_LEAN_AND_MEAN redefined` à la ligne 5 l'a trahi.
- **Ce que j'ai fait :** J'ai vérifié le contenu réel du fichier avec `Get-Content src\main.cpp`, enregistré avec Ctrl+S, puis corrigé l'avertissement avec un `#ifndef`, jusqu'à une compilation propre et une boîte de salutation affichée.
- **Temps perdu :** ≈ 45 min, en de nombreux allers-retours.

## 13. `jenga package --type aab` ne peut pas marcher

- **Symptôme :** `bundletool not found. Please install via SDK Manager.` (exercice 22).
- **Ce que j'ai cru :** qu'il suffisait d'installer `bundletool`.
- **Ce que c'était :** `bundletool` n'est pas dans le SDK ; et même installé, Jenga lui passe un dossier et un manifeste au format binaire, alors qu'il attend un module ZIP au format proto ; et il signerait l'AAB avec `apksigner` au lieu de `jarsigner`. J'ai fabriqué l'AAB avec un script (`faire_aab.py`).
- **Ce que j'ai fait :** J'ai téléchargé `bundletool`, fabriqué l'AAB avec mon script `faire_aab.py`, et je l'ai signé avec `jarsigner` : `jar signed.`
- **Temps perdu :** ≈ 30 min.

## 14. La bibliothèque x86_64 déguisée en arm64

- **Symptôme :** dans mon premier AAB, `libMaSalle.so` pesait 22 752 octets, contre 23 440 dans l'APK (exercice 22).
- **Ce que j'ai cru :** que c'était le même fichier.
- **Ce que c'était :** le `jenga package --type aab` lancé entre-temps avait recompilé la bibliothèque **pour x86_64, au même endroit** que la version arm64. Mon script l'avait rangée dans `lib/arm64-v8a/`. Sur le téléphone, ce paquet aurait planté. Le script vérifie désormais l'en-tête ELF (octets 18-19 : 183 pour AArch64).
- **Ce que j'ai fait :** J'ai comparé les tailles des deux paquets, reconstruit proprement en arm64, refait l'AAB : même bibliothèque de 23 440 octets dans les deux.
- **Temps perdu :** ≈ 15 min, et un paquet faux évité de justesse.

## 15. « Waiting for device »

- **Symptôme :** `adb logcat -c` bloqué sur `- waiting for device -`, `adb install` → `no devices/emulators found` (exercice 21).
- **Ce que j'ai cru :** que les commandes étaient bloquées.
- **Ce que c'était :** le téléphone n'était plus branché. `Ctrl+C`, rebrancher, `adb devices`.
- **Ce que j'ai fait :** J'ai arrêté la commande bloquée avec Ctrl+C, rebranché le téléphone et vérifié avec `adb devices` avant de reprendre.
- **Temps perdu :** ≈ 10 min.

## 16. Le mot de passe, encore

- **Symptôme :** `keystore password was incorrect` au premier essai, à presque chaque signature (exercices 18 à 23).
- **Ce que j'ai cru :** chaque fois, une simple faute de frappe.
- **Ce que c'était :** c'en était une, chaque fois. Mais répétée, elle coûte : le script de l'exercice 23 s'est même arrêté à l'étape 3 pour cette raison.
- **Ce que j'ai fait :** J'ai pris l'habitude de taper le mot de passe lentement, en vérifiant le verrouillage des majuscules.
- **Temps perdu :** ≈ 2 min par signature, soit ≈ 20 min sur le chapitre.

## 17. Le `.gitignore` sans son point

- **Symptôme :** le fichier de la racine du dépôt apparaissait sur GitHub sous le nom `gitignore`.
- **Ce que j'ai cru :** qu'il était actif.
- **Ce que c'était :** Windows avait retiré le point au téléchargement. Sans lui, Git l'ignore, et **rien n'empêchait une clé `.jks` d'être poussée**. Renommé avec `ren gitignore .gitignore`.
- **Ce que j'ai fait :** J'ai renommé le fichier avec `ren gitignore .gitignore`, commité et poussé, puis vérifié qu'aucune clé n'était dans le dépôt.
- **Temps perdu :** ≈ 10 min, pour un risque bien plus grand.

## 18. Le script ne vérifiait pas le NDK

- **Symptôme :** lors de l'essai sur une autre machine sans NDK, décrit à l'exercice 23, mon script de déploiement passait la vérification des outils, puis échouait au milieu du build avec `No suitable toolchain found for Android arm64`.
- **Ce que j'ai cru :** que le script vérifiait tout ce qu'il fallait.
- **Ce que c'était :** chez moi, le NDK est là depuis l'exercice 16, donc je n'avais pas pensé à le vérifier. Ajout de `trouver_ndk` à l'étape 0.
- **Ce que j'ai fait :** J'ai remplacé le script par la version qui vérifie le NDK, vérifié la version avec `findstr trouver_ndk deployer.py`, et relancé le déploiement complet jusqu'à `SUCCES`.
- **Temps perdu :** 0 chez moi ; c'est justement ce qu'un premier utilisateur aurait perdu.

---

## Le bilan

| # | Ce qui a cassé | Temps perdu |
|---:|---|---:|
| 1 | aucune chaîne Android | ≈ 1 h 30 |
| 2 | Java « absent » après installation | ≈ 30 min |
| 3 | commande coupée | ≈ 5 min |
| 4 | paquet x86_64 refusé | ≈ 20 min |
| 5 | mot de passe de clé perdu | ≈ 40 min |
| 6 | `libc++_shared.so` absente | ≈ 25 min |
| 7 | cache : correction ignorée | ≈ 20 min |
| 8 | débogage USB non autorisé | ≈ 5 min |
| 9 | commandes collées, jamais exécutées | ≈ 30 min |
| 10 | les « tombstones » | ≈ 10 min |
| 11 | fichier introuvable dans l'APK | 0 |
| 12 | fichier non enregistré | ≈ 45 min |
| 13 | `jenga package --type aab` | ≈ 30 min |
| 14 | bibliothèque x86_64 déguisée | ≈ 15 min |
| 15 | téléphone débranché | ≈ 10 min |
| 16 | mot de passe mal tapé, encore | ≈ 20 min |
| 17 | `.gitignore` sans point | ≈ 10 min |
| 18 | NDK non vérifié par le script | 0 |
| | **Total** | **≈ 6 h 45** |

## Ce que je relirai au chapitre 22

1. **« Build Successful » ne veut rien dire tout seul.** Trois fois (entrées 7, 12 et 14), l'outil annonçait un succès en ayant construit autre chose que ce que je croyais. Après toute modification de réglage : `jenga clean`. Et vérifier le résultat lui-même, pas seulement le message.
2. **Les vraies pannes n'apparaissent que sur l'appareil.** Architecture (4), bibliothèque manquante (6), fichier introuvable (11) : tout compilait. Seul le téléphone les révèle, donc on installe tôt et souvent.
3. **Lire le message exact, en entier.** Chaque panne était nommée dans sa trace : `NO_MATCHING_ABIS`, `libc++_shared.so" not found`, `errno 2`, `All files up to date`, le numéro de ligne d'un avertissement.
4. **Quand l'outil se trompe, lire son code.** Les entrées 4, 5, 13 et 14 ne se comprennent qu'en ouvrant le code de Jenga.
5. **Une commande à la fois, et vérifier sa sortie.** L'entrée 9 a coûté une demi-heure pour des commandes qui n'avaient simplement jamais tourné.
6. **Le mot de passe de la clé est une donnée critique.** Deux entrées (5 et 16), plus d'une heure en tout : noter le mot de passe proprement, sans ambiguïté de clavier, et le tester tout de suite.
7. **Ce qui marche chez moi n'est pas vérifié.** Le NDK (18) et le `.gitignore` (17) n'ont été vus qu'en regardant depuis une autre machine ou depuis GitHub.

## Ce que j'en retire

Au bout de ce chapitre, je sais faire, de bout en bout, ce qui me paraissait hors de portée au début : installer une chaîne Android complète sur une connexion difficile, construire pour la bonne architecture, créer et garder une clé de signature, empaqueter en APK et en AAB, installer sur un vrai appareil, lire le journal et une trace de plantage, et automatiser le tout dans un script qui s'arrête à la première panne.

Surtout, j'ai pris des réflexes : ne pas croire un « Build Successful » sur parole, vérifier le résultat lui-même (le contenu du paquet, l'architecture de la bibliothèque, ses dépendances), lire le message d'erreur exact en entier, et avancer une commande à la fois. Aucune de ces 18 pannes ne m'a arrêtée : chacune a fini par une correction vérifiée sur le téléphone.

