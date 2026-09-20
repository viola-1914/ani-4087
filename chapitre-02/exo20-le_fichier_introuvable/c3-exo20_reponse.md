# Chapitre 02 — Exo 20 : le fichier introuvable

## Le montage

Une ressource, `Resources/salle.txt`, 61 octets, dont la première ligne est `Salle 1 : le plan de MaSalle`. Dans `MaSalle.jenga`, le filtre Android la fait entrer dans le paquet :

```python
            androidassets(["Resources"])    # le dossier est copie dans assets/ de l'APK
```

Le programme lit **le même chemin**, `Resources/salle.txt`, de deux façons, et écrit le résultat des deux : dans une boîte de message sous Windows, dans le journal sous Android.

**Essai 1 — directement avec `fopen` :**

```cpp
std::FILE* f = std::fopen(chemin, "rb");
if (!f)
    return std::string("fopen(\"") + chemin + "\") : ECHEC - errno "
           + std::to_string(errno) + " (" + std::strerror(errno) + ")";
```

## Les deux comportements, avant correction

**Sur ma machine (Windows x86_64, `jenga run MaSalle --config Release`) : ça marche.**

```text
Lecture de la ressource Resources/salle.txt

fopen("Resources/salle.txt") : OK - 61 octets, 1re ligne : "Salle 1 : le plan de MaSalle"
```

**Sur l'appareil (TECNO SPARK 10C, `adb logcat -d -s MaSalle`) : ça échoue.**

```text
09-20 18:26:52.344 25382 25404 I MaSalle : Lecture de la ressource Resources/salle.txt
09-20 18:26:52.344 25382 25404 I MaSalle : fopen("Resources/salle.txt") : ECHEC — errno 2 (No such file or directory)
```

**Pourquoi.** Sur l'ordinateur, `Resources/salle.txt` est un vrai fichier, sur un vrai disque, et le programme est lancé depuis le dossier du projet : `fopen` le trouve. Sur Android, ce fichier n'existe nulle part sur le système de fichiers : il est **rangé à l'intérieur de l'APK**, une archive zip, dans `assets/salle.txt`. Aucun chemin ne mène jusqu'à lui, et le dossier courant de l'application n'a rien à voir avec celui du projet. `fopen` cherche donc un fichier qui n'existe pas, et `errno` vaut 2, `No such file or directory`.

C'est une erreur qu'aucune construction ne peut détecter : le code compile, se lie, s'installe et se lance. Elle n'apparaît qu'à l'exécution, sur l'appareil.

## La correction : passer par le système de fichiers

Le moteur Nkentseu règle ce problème dans son module **NKFileSystem**. J'ai lu son code (`Kernel/System/NKFileSystem/src/NKFileSystem/NkFile.cpp`) : sous Android, il garde un `AAssetManager` global, posé au démarrage, ouvre les ressources avec `AAssetManager_open`, et retire du chemin le préfixe `Resources/`, parce que le contenu du dossier a été copié à la racine de `assets/`.

Ce module n'arrive qu'au sprint 3. J'ai donc écrit dans `main.cpp` un petit système de fichiers **sur le même modèle**, en quelques lignes :

```cpp
static AAssetManager* gGestionnaireAssets = nullptr;   // pose au demarrage

static std::string EssaiSystemeDeFichiers(const char* chemin)
{
    const char* relatif = chemin;
    if (std::strncmp(relatif, "Resources/", 10) == 0)
        relatif += 10;                       // "Resources/salle.txt" -> "salle.txt"

    AAsset* asset = AAssetManager_open(gGestionnaireAssets, relatif, AASSET_MODE_BUFFER);
    ...
}
```

Le gestionnaire vient du glue de NativeActivity, au tout début de `android_main` :

```cpp
    PoserGestionnaireAssets(app->activity->assetManager);
```

Sous Windows et sous Linux, la même fonction lit simplement le disque. **Le code appelant, lui, ne change pas d'une plateforme à l'autre** : c'est tout l'intérêt d'un système de fichiers de moteur.

## Les deux comportements, après correction

**Sur ma machine : ça marche toujours.**

```text
systeme de fichiers -> disque : fopen("Resources/salle.txt") : OK - 61 octets, 1re ligne : "Salle 1 : le plan de MaSalle"
```

**Sur l'appareil : ça marche maintenant.**

```text
09-20 18:26:52.344 25382 25404 I MaSalle : systeme de fichiers ("Resources/salle.txt") : OK — 61 octets, 1re ligne : "Salle 1 : le plan de MaSalle"
```

Même chemin, même contenu, mêmes 61 octets, sur les deux plateformes.

## Le tableau

| | `fopen` direct | système de fichiers |
|---|---|---|
| Windows | OK, 61 octets | OK, 61 octets |
| Android | **ECHEC — errno 2 (No such file or directory)** | OK, 61 octets |

## Ce que j'en retiens

- **Un chemin n'est pas une ressource.** Sur un ordinateur les deux se confondent ; sur Android, la ressource vit dans l'APK et n'a pas de chemin.
- **L'erreur ne se voit qu'à l'exécution, sur l'appareil.** C'est la troisième fois dans ce chapitre : la construction réussit, et pourtant quelque chose manque. Après l'architecture absente (exercice 17) et la bibliothèque absente (exercice 17 encore), voici le fichier absent.
- **C'est pour cela qu'un moteur a un module de fichiers.** Il ne remplace pas `fopen` par caprice : il permet d'écrire un seul chemin dans le code, et de le résoudre différemment selon la plateforme.

Un détail d'affichage : la boîte de message de Windows montre `â€"` à la place du tiret long, parce qu'elle lit le texte en ANSI alors que mon fichier source est en UTF-8. Cela ne change rien au test, mais c'est une raison de plus de s'en tenir aux caractères simples dans les messages.
