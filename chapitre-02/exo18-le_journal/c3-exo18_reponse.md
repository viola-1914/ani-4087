# Chapitre 02 — Exo 18 : le journal

## Les trois lignes

Au démarrage, `MaSalle` écrit trois lignes dans le journal d'Android, avec la fonction `__android_log_print` de la bibliothèque `liblog` (déjà liée par `links(["android", "log", ...])` dans le filtre Android de `MaSalle.jenga`). Toutes portent la même **étiquette**, `MaSalle` :

```cpp
#include <android/log.h>
#define MASALLE_LOG(...) __android_log_print(ANDROID_LOG_INFO, "MaSalle", __VA_ARGS__)
```

Les trois lignes suivent les étapes du démarrage, dans l'ordre où elles se produisent dans `src/main.cpp` :

| Ligne | Écrite dans | Moment |
|---|---|---|
| `1/3 Bienvenue dans MaSalle ! ...` | `android_main` | le code natif commence |
| `2/3 Fenetre prete : L x H pixels` | `SurCommande`, sur `APP_CMD_INIT_WINDOW` | Android donne une fenêtre au programme |
| `3/3 Ecran rempli en bleu-vert : L x H pixels` | `Peindre`, une seule fois | la couleur est affichée |

## Lancer le programme sur le téléphone

Sur mon téléphone (TECNO SPARK 10C, relié en USB), depuis `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo18-le_journal` :

```text
jenga clean --platform android-arm64 --config Release
jenga build --platform android-arm64 --config Release
"C:\Android\build-tools\34.0.0\apksigner.bat" sign --ks "%USERPROFILE%\cles\masalle.jks" --ks-key-alias masalle --out dist\MaSalle-arm64-signe.apk build\bin\Android\Release\android-build-arm64-v8a\MaSalle-Release.apk
C:\Android\platform-tools\adb.exe install -r dist\MaSalle-arm64-signe.apk
Success
```

Puis, pour un journal propre :

```text
C:\Android\platform-tools\adb.exe logcat -c
C:\Android\platform-tools\adb.exe shell am force-stop com.mafo.masalle
C:\Android\platform-tools\adb.exe shell monkey -p com.mafo.masalle -c android.intent.category.LAUNCHER 1
```

`logcat -c` vide le journal, pour ne pas y retrouver les lancements précédents. `am force-stop` arrête toute instance déjà ouverte de l'application : sans cela, Android peut simplement la ramener au premier plan sans la redémarrer, et les trois lignes de démarrage ne seraient pas réécrites. L'écran du téléphone est devenu bleu-vert.

## La commande de filtrage

```text
C:\Android\platform-tools\adb.exe logcat -d -s MaSalle
```

```text
--------- beginning of main
09-20 15:45:54.262  2235  2257 I MaSalle : 1/3 Bienvenue dans MaSalle ! Demarrage de l'application native.
09-20 15:45:54.323  2235  2257 I MaSalle : 2/3 Fenetre prete : 1540 x 720 pixels
09-20 15:45:54.340  2235  2257 I MaSalle : 3/3 Ecran rempli en bleu-vert : 1540 x 720 pixels
```

- **`-s MaSalle`** : « silence », sauf pour l'étiquette `MaSalle`. C'est l'abréviation de `MaSalle:V *:S` : tout ce qui porte l'étiquette `MaSalle` est affiché, quel que soit son niveau, et toutes les autres étiquettes (`*`) sont réduites au silence (`S`).
- **`-d`** : affiche le journal actuel puis s'arrête, au lieu de le suivre en continu. Sans `-d`, la même commande reste ouverte et affiche les lignes au fur et à mesure qu'elles arrivent.

Chaque ligne indique la date et l'heure, le numéro du processus (`2235`) et du fil (`2257`), le niveau (`I` pour information), l'étiquette et le message. Les trois lignes arrivent en 78 millisecondes, de 15:45:54.262 à 15:45:54.340. Elles donnent aussi une information que je n'avais pas avant : la fenêtre fait **1540 × 720 pixels**, plus large que haute, ce qui confirme que l'application s'ouvre en paysage (`androidscreenorientation("landscape")`).

## Au milieu de tout le reste

Le même journal, sans filtre, juste après :

```text
C:\Android\platform-tools\adb.exe logcat -d | find /c /v ""
10822
```

`find /c /v ""` compte les lignes. **Mes 3 lignes se trouvaient au milieu de 10 822 lignes**, écrites en un peu plus d'une minute, depuis le `logcat -c`, par tout le reste du téléphone : le système, les services, les autres applications. Sans le filtre sur l'étiquette, elles seraient introuvables à l'œil.

C'est pour cela que l'étiquette compte autant que le message : c'est elle qui permet de retrouver ses propres lignes. Il faut la choisir propre au programme, et la garder identique partout dans le code.
