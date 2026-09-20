# Chapitre 02 — Exo 19 : le plantage provoqué

## Le plantage volontaire

Dans `src/main.cpp`, juste après avoir peint l'écran, le programme appelle une fonction qui écrit à l'adresse 0 :

```cpp
__attribute__((noinline))
static void PlanterVolontairement()
{
    volatile int* pointeurNul = nullptr;
    MASALLE_LOG("4/4 Plantage volontaire : ecriture a l'adresse %p", (void*)pointeurNul);
    *pointeurNul = 42;
}
```

`volatile` oblige le compilateur à faire réellement l'écriture, même en Release avec `optimize("Speed")` : sans lui, l'optimiseur aurait pu la supprimer. `noinline` empêche la fonction d'être fondue dans `Peindre`, pour qu'elle apparaisse sous son nom dans la trace.

Sur le téléphone, l'écran devient bleu-vert un court instant, puis l'application se ferme d'elle-même.

## Les lignes du programme

```text
C:\Android\platform-tools\adb.exe logcat -d -s MaSalle
09-20 16:19:53.116  9953  9970 I MaSalle : 1/3 Bienvenue dans MaSalle ! Demarrage de l'application native.
09-20 16:19:53.159  9953  9970 I MaSalle : 2/3 Fenetre prete : 1540 x 720 pixels
09-20 16:19:53.170  9953  9970 I MaSalle : 3/3 Ecran rempli en bleu-vert : 1540 x 720 pixels
09-20 16:19:53.170  9953  9970 I MaSalle : 4/4 Plantage volontaire : ecriture a l'adresse 0x0
```

Le programme a bien écrit ses quatre lignes, puis il meurt : rien ne suit `4/4`.

## La trace du plantage

```text
C:\Android\platform-tools\adb.exe logcat -d -b crash
```

```text
09-20 16:19:53.170  9953  9970 F libc    : Fatal signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x0 in tid 9970 (om.mafo.masalle), pid 9953 (om.mafo.masalle)
09-20 16:19:53.424  9975  9975 F DEBUG   : *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
09-20 16:19:53.424  9975  9975 F DEBUG   : Native Crash TIME: 27955301
09-20 16:19:53.424  9975  9975 F DEBUG   : Build fingerprint: 'TECNO/KI5k-GL/TECNO-KI5k:12/SP1A.210812.016/GL-20241211V618:user/release-keys'
09-20 16:19:53.424  9975  9975 F DEBUG   : Revision: '0'
09-20 16:19:53.424  9975  9975 F DEBUG   : ABI: 'arm64'
09-20 16:19:53.424  9975  9975 F DEBUG   : Timestamp: 2026-09-20 16:19:53.225308402+0100
09-20 16:19:53.424  9975  9975 F DEBUG   : Process uptime: 0s
09-20 16:19:53.424  9975  9975 F DEBUG   : Cmdline: com.mafo.masalle
09-20 16:19:53.424  9975  9975 F DEBUG   : pid: 9953, tid: 9970, name: om.mafo.masalle  >>> com.mafo.masalle <<<
09-20 16:19:53.424  9975  9975 F DEBUG   : uid: 10265
09-20 16:19:53.424  9975  9975 F DEBUG   : signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x0
09-20 16:19:53.424  9975  9975 F DEBUG   : Cause: null pointer dereference
09-20 16:19:53.424  9975  9975 F DEBUG   :     x0  0000000000000001  x1  0000000000000000  x2  0000000000000050  x3  0000000000000003
09-20 16:19:53.424  9975  9975 F DEBUG   :     x4  000000750c8212c0  x5  0000000000000000  x6  7473687162641f39  x7  7f7f7f7f7f7f7f7f
09-20 16:19:53.424  9975  9975 F DEBUG   :     x8  000000000000002a  x9  ce30fc29a6eea1f3  x10 000000750c821258  x11 fffffffffffffffd
09-20 16:19:53.424  9975  9975 F DEBUG   :     x12 000000750c8213e0  x13 0000000000000033  x14 000000750c822678  x15 0000000026762762
09-20 16:19:53.424  9975  9975 F DEBUG   :     x16 00000077a7b5c528  x17 00000077a7b56a30  x18 00000074841a0000  x19 0000000000000000
09-20 16:19:53.424  9975  9975 F DEBUG   :     x20 0000000000000001  x21 b4000075d56cb7a0  x22 0000000000000001  x23 00000000000026e1
09-20 16:19:53.424  9975  9975 F DEBUG   :     x24 000000750c822c30  x25 000000750c822c30  x26 000000750c822f78  x27 00000000000fc000
09-20 16:19:53.424  9975  9975 F DEBUG   :     x28 000000750c72a000  x29 000000750c822ad0
09-20 16:19:53.424  9975  9975 F DEBUG   :     lr  000000750c82a460  sp  000000750c822ad0  pc  000000750c82a464  pst 0000000060000000
09-20 16:19:53.424  9975  9975 F DEBUG   : backtrace:
09-20 16:19:53.424  9975  9975 F DEBUG   :       #00 pc 0000000000003464  /data/app/~~ADm7CeYCMVnE1bGJnIbIgA==/com.mafo.masalle-c41LL2IV5oj5cH55wPp3_w==/lib/arm64/libMaSalle.so (PlanterVolontairement()+48)
09-20 16:19:53.424  9975  9975 F DEBUG   :       #01 pc 0000000000003420  /data/app/~~ADm7CeYCMVnE1bGJnIbIgA==/com.mafo.masalle-c41LL2IV5oj5cH55wPp3_w==/lib/arm64/libMaSalle.so (Peindre(ANativeWindow*)+212)
09-20 16:19:53.424  9975  9975 F DEBUG   :       #02 pc 00000000000046dc  /data/app/~~ADm7CeYCMVnE1bGJnIbIgA==/com.mafo.masalle-c41LL2IV5oj5cH55wPp3_w==/lib/arm64/libMaSalle.so (process_cmd+160)
09-20 16:19:53.424  9975  9975 F DEBUG   :       #03 pc 00000000000032c4  /data/app/~~ADm7CeYCMVnE1bGJnIbIgA==/com.mafo.masalle-c41LL2IV5oj5cH55wPp3_w==/lib/arm64/libMaSalle.so (android_main+136)
09-20 16:19:53.424  9975  9975 F DEBUG   :       #04 pc 00000000000045b4  /data/app/~~ADm7CeYCMVnE1bGJnIbIgA==/com.mafo.masalle-c41LL2IV5oj5cH55wPp3_w==/lib/arm64/libMaSalle.so (android_app_entry+172)
09-20 16:19:53.424  9975  9975 F DEBUG   :       #05 pc 00000000000b142c  /apex/com.android.runtime/lib64/bionic/libc.so (__pthread_start(void*)+204) (BuildId: 5d21548447ff2f9aab8359665aaabf4f)
09-20 16:19:53.424  9975  9975 F DEBUG   :       #06 pc 0000000000050ff4  /apex/com.android.runtime/lib64/bionic/libc.so (__start_thread+64) (BuildId: 5d21548447ff2f9aab8359665aaabf4f)
```

La même trace apparaît six fois dans le journal : Android et le lanceur ont réessayé de démarrer l'application, et elle a planté à chaque fois, toujours au même endroit.

## Ce que la trace m'apprend

- **La nature de l'erreur.** `signal 11 (SIGSEGV)`, `code 1 (SEGV_MAPERR)` : le programme a touché une adresse qui n'est associée à aucune mémoire. Android va même jusqu'à écrire la cause en toutes lettres : `Cause: null pointer dereference`.
- **L'adresse fautive.** `fault addr 0x0` : l'adresse zéro, celle du pointeur nul.
- **L'endroit exact dans mon code.** La pile d'appels se lit de haut en bas, du plus récent au plus ancien : `PlanterVolontairement()` a été appelée par `Peindre(ANativeWindow*)`, elle-même par `process_cmd` puis `android_main` (le glue de NativeActivity), lancés depuis un fil créé par `libc`. C'est précisément le chemin que suit mon programme.
- **Le décalage dans la fonction.** `PlanterVolontairement()+48` : la faute est 48 octets après le début de la fonction, et `pc 0x3464` donne sa position dans `libMaSalle.so`.
- **Le fil fautif.** `pid: 9953, tid: 9970` : le plantage n'a pas eu lieu sur le fil principal, mais sur le fil séparé que `android_native_app_glue` crée pour `android_main`.
- **Le binaire concerné.** Le chemin complet du `.so` chargé, dans le dossier d'installation de l'application, ainsi que l'ABI `arm64`.
- **Une trace de la valeur écrite.** Le registre `x8` vaut `0x2a`, soit 42 en décimal : c'est bien la valeur que la ligne `*pointeurNul = 42;` voulait écrire. `x19` vaut 0, l'adresse visée.
- **La machine et le moment.** Le modèle et la version d'Android (`Build fingerprint`), l'horodatage, et `Process uptime: 0s` : le plantage a lieu moins d'une seconde après le démarrage.

## Ce qu'elle ne m'apprend pas

- **Ni le fichier ni la ligne.** La trace ne dit jamais « `main.cpp`, ligne 27 ». Elle ne connaît que des adresses et des noms de fonctions, parce que la configuration Release de mon `.jenga` compile avec `symbols(False)` : il n'y a pas d'informations de débogage dans la bibliothèque. Pour obtenir le numéro de ligne, il faudrait retraduire l'adresse `0x3464` avec `llvm-addr2line` ou `ndk-stack`, à partir de la bibliothèque non allégée gardée sur mon ordinateur, ou construire en Debug avec `symbols(True)`.
- **Pourquoi le pointeur était nul.** Elle décrit l'accident, jamais la logique qui y mène. Ici je le sais, puisque je l'ai voulu ; dans un vrai bogue, il faudrait remonter soi-même de `Peindre` vers ce qui a mis le pointeur à zéro.
- **Le contenu des variables.** Les registres sont donnés bruts, sans nom : rien ne dit que `x8` est « la valeur 42 » ni que `pointeurNul` existe. Il faut relire le code pour les interpréter.
- **Mes propres lignes de journal.** La trace et les lignes `1/3` à `4/4` sont dans deux tampons différents : `-b crash` ne montre pas les miennes, et `-s MaSalle` ne montre pas la trace. Il faut les rapprocher à la main, ou tout lire d'un coup avec une commande comme `adb logcat -d -b main -b crash`.
- **Ce que faisait l'utilisateur.** Rien n'indique quel geste a déclenché le plantage, ni ce qui s'affichait à l'écran.
- **Les autres fils d'exécution.** Le journal ne montre que le fil fautif. Le fichier complet, le « tombstone », en garde bien plus (la carte mémoire, l'état des autres fils) ; il est conservé dans `/data/tombstones/` sur l'appareil.
- **Quelle version du binaire.** Les bibliothèques du système affichent un `BuildId`, qui identifie leur version exacte. `libMaSalle.so` n'en affiche pas : si j'avais plusieurs constructions différentes, rien dans la trace ne dirait laquelle a planté.

## Ce que j'en retiens

Une trace native répond très bien à « où » et « quoi » : quel signal, quelle adresse, quelle fonction, quel fil. Elle ne répond jamais à « pourquoi ». Et la précision de la réponse dépend de la façon dont on a construit le programme : en Release, j'obtiens des noms de fonctions, mais pas de numéros de ligne.
