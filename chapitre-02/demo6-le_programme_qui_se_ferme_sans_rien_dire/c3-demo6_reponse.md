# Chapitre 02 — Démo 6 : le programme qui se ferme sans rien dire

## Le montage

Le programme est celui des démos précédentes, `MaSalle`, avec une seule différence : l'appel au plantage volontaire a été déplacé **au tout début** de `android_main`, avant que l'écran soit peint.

```cpp
__attribute__((noinline))
static void PlanterVolontairement()
{
    volatile int* pointeurNul = nullptr;
    MASALLE_LOG("Demarrage : ecriture a l'adresse %p (plantage volontaire)", (void*)pointeurNul);
    *pointeurNul = 42;                       // ecriture a l'adresse 0
}

extern "C" void android_main(android_app* app)
{
    app->onAppCmd = SurCommande;
    // Demo 6 : le plantage a lieu AU DEMARRAGE, avant que l'ecran soit peint.
    PlanterVolontairement();
    ...
}
```

Construction, signature et installation, comme d'habitude :

```text
jenga clean --platform android-arm64 --config Release
jenga build --platform android-arm64 --config Release
✓ Built: build\bin\Android\Release\libMaSalle.so
✓ Build Successful                                   Time: 1.12s
ℹ Building APK for MaSalle (arm64-v8a)

"C:\Android\build-tools\34.0.0\apksigner.bat" sign --ks "%USERPROFILE%\cles\masalle.jks" --ks-key-alias masalle --out dist\MaSalle.apk build\bin\Android\Release\android-build-arm64-v8a\MaSalle-Release.apk

C:\Android\platform-tools\adb.exe install -r dist\MaSalle.apk
Performing Incremental Install
Performing Streamed Install
Success
```

**Rien n'annonce le problème** : la construction réussit, l'APK est signé, l'installation dit `Success`.

## Ce que voit la classe

Je touche l'icône **MaSalle** sur le téléphone. L'écran **revient brusquement au bureau**. Pas d'écran bleu-vert, pas de message « MaSalle s'est arrêtée », pas de boîte de dialogue : rien. On dirait que l'application n'a pas voulu s'ouvrir.

Pour quelqu'un qui découvre, c'est le pire des cas : il n'y a **rien à lire**. C'est là que commence la vraie question : où regarder ?

## Ouvrir le journal, et le regarder en direct

Je vide le journal, puis j'ouvre une **écoute en direct**. La commande ne rend pas la main : elle reste ouverte et affiche les plantages au fur et à mesure.

```text
C:\Android\platform-tools\adb.exe logcat -c
C:\Android\platform-tools\adb.exe logcat -b crash
```

Le terminal reste vide. Je retouche alors l'icône sur le téléphone, et **la trace s'écrit instantanément sous les yeux de la classe**. À chaque fois que je touche l'icône, une nouvelle trace apparaît, dans la seconde.

```text
--------- beginning of crash
09-23 09:23:34.570  4640  4659 F libc    : Fatal signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x0 in tid 4659 (om.mafo.masalle), pid 4640 (om.mafo.masalle)
09-23 09:23:34.828  4664  4664 F DEBUG   : *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
09-23 09:23:34.828  4664  4664 F DEBUG   : Native Crash TIME: 48829858
09-23 09:23:34.828  4664  4664 F DEBUG   : Build fingerprint: 'TECNO/KI5k-GL/TECNO-KI5k:12/SP1A.210812.016/GL-20241211V618:user/release-keys'
09-23 09:23:34.828  4664  4664 F DEBUG   : Revision: '0'
09-23 09:23:34.828  4664  4664 F DEBUG   : ABI: 'arm64'
09-23 09:23:34.828  4664  4664 F DEBUG   : Timestamp: 2026-09-23 09:23:34.640018921+0100
09-23 09:23:34.828  4664  4664 F DEBUG   : Process uptime: 0s
09-23 09:23:34.828  4664  4664 F DEBUG   : Cmdline: com.mafo.masalle
09-23 09:23:34.828  4664  4664 F DEBUG   : pid: 4640, tid: 4659, name: om.mafo.masalle  >>> com.mafo.masalle <<<
09-23 09:23:34.828  4664  4664 F DEBUG   : uid: 10276
09-23 09:23:34.828  4664  4664 F DEBUG   : signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x0
09-23 09:23:34.828  4664  4664 F DEBUG   : Cause: null pointer dereference
09-23 09:23:34.828  4664  4664 F DEBUG   :     x0  0000000000000001  x1  0000000000000000  x2  0000000000000050  x3  0000000000000003
...
09-23 09:23:34.829  4664  4664 F DEBUG   :     lr  00000079b20aa37c  sp  00000079b20a2b50  pc  00000079b20aa380  pst 0000000060001000
09-23 09:23:34.829  4664  4664 F DEBUG   : backtrace:
09-23 09:23:34.829  4664  4664 F DEBUG   :       #00 pc 0000000000003380  /data/app/~~zg-IUuCGuOZiQ1V_FkZu6w==/com.mafo.masalle-JCxg9v6M2Vl6JEfgHTyAdQ==/lib/arm64/libMaSalle.so (PlanterVolontairement()+48)
09-23 09:23:34.829  4664  4664 F DEBUG   :       #01 pc 000000000000327c  /data/app/~~zg-IUuCGuOZiQ1V_FkZu6w==/com.mafo.masalle-JCxg9v6M2Vl6JEfgHTyAdQ==/lib/arm64/libMaSalle.so (android_main+56)
09-23 09:23:34.829  4664  4664 F DEBUG   :       #02 pc 00000000000045b4  /data/app/~~zg-IUuCGuOZiQ1V_FkZu6w==/com.mafo.masalle-JCxg9v6M2Vl6JEfgHTyAdQ==/lib/arm64/libMaSalle.so (android_app_entry+172)
09-23 09:23:34.829  4664  4664 F DEBUG   :       #03 pc 00000000000b142c  /apex/com.android.runtime/lib64/bionic/libc.so (__pthread_start(void*)+204) (BuildId: 5d21548447ff2f9aab8359665aaabf4f)
09-23 09:23:34.829  4664  4664 F DEBUG   :       #04 pc 0000000000050ff4  /apex/com.android.runtime/lib64/bionic/libc.so (__start_thread+64) (BuildId: 5d21548447ff2f9aab8359665aaabf4f)
```

(Les lignes des registres `x0` à `x29` sont abrégées ici ; elles figurent en entier dans la sortie du terminal. La même trace s'est répétée à chaque lancement : 09:23:34, 09:23:35, 09:23:40, 09:23:42, 09:24:03, 09:24:20, etc.)

## Lire la trace devant la classe

Il faut la lire de bas en haut pour l'histoire, et de haut en bas pour la cause.

| Ligne | Ce qu'elle dit |
|---|---|
| `Fatal signal 11 (SIGSEGV)` | le système a tué le programme : accès mémoire interdit |
| `fault addr 0x0` | l'adresse touchée est zéro |
| `Cause: null pointer dereference` | Android écrit la cause en toutes lettres |
| `Cmdline: com.mafo.masalle` | c'est bien **mon** application, pas une autre |
| `Process uptime: 0s` | le plantage a lieu **moins d'une seconde après le démarrage** : c'est pour cela qu'on n'a rien vu à l'écran |
| `#00 ... PlanterVolontairement()+48` | la fonction fautive, 48 octets après son début |
| `#01 ... android_main+56` | appelée par `android_main`, tout au début |
| `#02 android_app_entry`, `#03 __pthread_start` | le glue de NativeActivity et le fil créé par la bibliothèque C |

La pile d'appels raconte donc exactement le chemin : Android démarre le fil de l'application, appelle `android_main`, qui appelle `PlanterVolontairement()`, qui écrit à l'adresse 0.

## Pourquoi l'écran ne dit rien

La différence avec un plantage de code Java est là : quand une application Java échoue, Android affiche « L'application s'est arrêtée ». Ici, l'erreur est **native** : c'est un signal du système (SIGSEGV) qui tue le processus d'un coup. Il n'y a personne pour afficher une boîte de dialogue, donc le lanceur reprend la main et on revient au bureau, sans un mot.

Rien n'est perdu pour autant : le système écrit la trace complète dans son journal, et un fichier détaillé, un « tombstone », dans `/data/tombstones/`.

## Ce que la démo montre

1. **Un écran qui revient au bureau n'est pas une absence d'information.** L'information existe, elle est seulement ailleurs, dans le journal.
2. **`logcat -b crash` en direct est l'outil de la démonstration** : on laisse l'écoute ouverte, on relance l'application, et la trace s'écrit sous les yeux de tous, immédiatement.
3. **Trois lignes suffisent à conclure** : `Cause: null pointer dereference`, `Process uptime: 0s`, et la première ligne de la pile d'appels. Tout le reste est du contexte.
4. **Le réflexe à prendre :** quand une application se ferme sans rien dire, ouvrir le journal avant de toucher au code.

## Pour réparer

Retirer l'appel dans `android_main` :

```cpp
    // PlanterVolontairement();
```

Le fichier déposé garde volontairement l'appel, pour que la démonstration puisse être rejouée.
