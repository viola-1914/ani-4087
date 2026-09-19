# Exercice 2 — Info avant build

## 1. Commande exécutée

J'ai exécuté `jenga info` dans PowerShell, sur mon ordinateur Windows, depuis
le dossier du projet minimal créé à l'exercice 1 :

```powershell
PS C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo1-le_projet_minimal> jenga info
```

Le fichier examiné est `MaSalle.jenga`, qui déclare l'espace de travail
`MaSalleWks` et le projet `MaSalle`. La version affichée de Jenga est 2.8.0.

## 2. Sortie complète obtenue

```text
╔══════════════════════════════════════════════════════════════════╗
║                                                                  ║
║                ██╗███████╗███╗   ██╗ ██████╗  █████╗             ║
║                ██║██╔════╝████╗  ██║██╔════╝ ██╔══██╗            ║
║                ██║█████╗  ██╔██╗ ██║██║  ███╗███████║            ║
║           ██   ██║██╔══╝  ██║╚██╗██║██║   ██║██╔══██║            ║
║           ╚█████╔╝███████╗██║ ╚████║╚██████╔╝██║  ██║            ║
║            ╚════╝ ╚══════╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝            ║
║                                                                  ║
║             Multi-platform C/C++ Build System v2.8.0             ║
║                                                                  ║
╚══════════════════════════════════════════════════════════════════╝

========================= Jenga Workspace: MaSalleWks ==========================

Location: C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo1-le_projet_minimal
Entry file: C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo1-le_projet_minimal\MaSalle.jenga
Configurations: Debug, Release
Platforms: Windows
Target OSes: 
Target Architectures: 


Projects
------------------------------------------------------------
Name      Kind          Language   Test   External
==================================================
MaSalle   WindowedApp   C++        No     No


Available Toolchains
------------------------------------------------------------
Name       Family   Target OS   Arch     Env  
==============================================
host-gcc   gcc      Windows     x86_64   mingw
mingw      gcc      Windows     x86_64   mingw


Daemon
------------------------------------------------------------
Status: Not running
```

La commande rend ensuite la main à PowerShell, sans message d'erreur affiché.

## 3. Lecture de la sortie

| Élément affiché | Ce qu'il signifie |
|---|---|
| Bannière `v2.8.0` | Version de Jenga exécutée sur l'ordinateur. |
| `Jenga Workspace: MaSalleWks` | L'espace de travail déclaré dans le fichier a été reconnu. |
| `Location` | Emplacement absolu de l'espace de travail sur mon ordinateur. |
| `Entry file` | Chemin du fichier `MaSalle.jenga` effectivement chargé. |
| `Configurations: Debug, Release` | Les deux configurations déclarées ont été reconnues. Cela ne signifie pas qu'elles ont été compilées. |
| `Platforms: Windows` | Le champ des plateformes contient Windows. Dans la version du code Jenga consultée, c'est aussi la valeur par défaut du champ historique `platforms`, non précisé dans notre fichier. |
| `Target OSes` vide | Aucune liste explicite de systèmes cibles n'a été renseignée dans le projet. |
| `Target Architectures` vide | Aucune liste explicite d'architectures cibles n'a été renseignée dans le projet. |
| `MaSalle`, `WindowedApp`, `C++` | Le nom du projet, son type d'application et son langage correspondent à nos déclarations. |
| `Test: No` | Le projet n'est pas marqué comme une suite de tests. |
| `External: No` | Il n'est pas marqué comme un projet externe. |
| `host-gcc` et `mingw` | Deux entrées de chaînes de compilation sont détectées par Jenga. |
| `Family: gcc` | Ces deux entrées appartiennent à la famille GCC. |
| `Target OS: Windows` | Ces chaînes ciblent Windows. |
| `Arch: x86_64` | Elles ciblent l'architecture x86-64, donc 64 bits. |
| `Env: mingw` | Elles utilisent l'environnement MinGW. |
| `Daemon — Status: Not running` | Le service d'arrière-plan de Jenga n'est pas lancé. Ce statut n'est pas une erreur de lecture du projet. |

## 4. Ce que cette sortie m'apprend que le fichier ne disait pas explicitement

### Les outils réellement détectés

Mon fichier de projet indique C++ et C++17, mais ne nomme pas de compilateur.
La rubrique `Available Toolchains` montre que Jenga détecte `host-gcc` et
`mingw`, tous deux de famille GCC, pour Windows x86_64 et l'environnement MinGW.

Ces deux noms ne prouvent pas qu'il existe deux installations différentes de
GCC : la sortie ne donne ni leurs chemins ni leurs versions. Ils peuvent
correspondre à la même installation enregistrée de deux façons. La liste ne
prouve pas non plus que les deux chaînes seront utilisées ensemble pour construire.

### Les chemins résolus

Le fichier contient `location(".")`. La sortie transforme ce chemin relatif
en un emplacement absolu et indique le fichier d'entrée réellement sélectionné.
Je peux ainsi vérifier que la commande examine bien le projet de l'exercice 1.

### Les valeurs implicites et l'état de l'installation

Le fichier ne précise ni la version de Jenga installée, ni l'état de son service
d'arrière-plan. La sortie indique respectivement `2.8.0` et `Not running`.
Elle rend également visibles les indicateurs `Test: No` et `External: No`.

`Platforms: Windows` est affiché même si je n'ai pas écrit `platforms(...)`.
Les champs `Target OSes` et `Target Architectures` sont vides parce que je n'ai
pas déclaré ces listes. Il faut distinguer ces champs du workspace des
caractéristiques des chaînes détectées : le tableau des outils indique bien
Windows et x86_64. Des listes de cibles non renseignées ne signifient donc pas
qu'aucune chaîne de compilation n'est disponible.

### Aucune chaîne Android n'est affichée

Les deux entrées ciblent Windows ; aucune ne cible Android dans cette sortie.
Avant un déploiement Android, il faudra vérifier la détection et la configuration
de la chaîne correspondante. Cette absence ne prouve pas à elle seule que tous
les outils Android sont absents du disque.

## 5. Ce que la commande ne permet pas de conclure

`jenga info` inspecte le projet et les outils disponibles : elle ne compile pas
les sources et ne lance pas l'application. La détection de GCC ne garantit donc
pas que `jenga build` réussira.

Cette sortie simple ne détaille pas la liste des sources, les options complètes
de compilation, le dialecte C++17 ni le résultat de chaque filtre.
`jenga info -v` peut fournir des informations supplémentaires, notamment sur le
système hôte, mais le journal reproduit ici provient bien de `jenga info`.

## Conclusion

Le fichier `.jenga` décrit ce que je souhaite construire. `jenga info` montre
comment Jenga a chargé ce projet et les moyens de compilation qu'il détecte sur
mon ordinateur. Les principales informations nouvelles sont les chemins absolus,
les chaînes GCC/MinGW pour Windows x86_64, les valeurs implicites et l'état du daemon.

## Références techniques consultées

Ces références servent à interpréter la sortie ; le journal ci-dessus provient
de mon exécution Windows, et non de l'environnement utilisé pour lire ces sources.

- [Commande Info : rubriques affichées et détection des chaînes](https://github.com/RihenUniverse/Jenga/blob/7eb93ec423199ef75a043f9249e5f0ece3dfa209/Jenga/Commands/Info.py)
- [API : valeurs par défaut du workspace](https://github.com/RihenUniverse/Jenga/blob/7eb93ec423199ef75a043f9249e5f0ece3dfa209/Jenga/Core/Api.py)
