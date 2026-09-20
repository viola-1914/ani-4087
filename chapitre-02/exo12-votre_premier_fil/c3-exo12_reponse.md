# Chapitre 02 — Exo 12 : votre premier fil

## Le fichier rendu

Voici `MaSalle.jenga`, le fichier de projet que je garderai pour tout le livre. Il reprend le projet `MaSalle` de l'exercice 1 et ce que le chapitre 2 y a ajouté : les filtres Windows et Linux avec leurs bibliothèques (exercice 8) et les configurations Debug et Release (exercice 10). Ensuite, **une ligne commentée par chapitre à venir**, du chapitre 3 au chapitre 17, chacune précédée d'un commentaire qui dit ce que le chapitre apporte.

```python
from Jenga import *

# =============================================================================
#  MaSalle — le fichier de projet de tout le livre
#
#  Chapitres 1 et 2 : actifs (projet minimal, deux systemes, Debug/Release).
#  Chapitres 3 a 17 : UNE ligne commentee par chapitre, a decocher dans l'ordre.
#  Chaque ligne n'AJOUTE que ce qui est nouveau : nkentseudependson, files,
#  links et defines s'accumulent d'un appel a l'autre, on ne touche jamais
#  aux lignes deja decochees.
#  Quand un chapitre concerne un systeme precis, une ligne de plus, marquee
#  du meme numero, attend dans le filtre de ce systeme (plus bas).
# =============================================================================

with workspace("MaSalleWks", location="."):
    configurations(["Debug", "Release"])
    targetoses([TargetOS.WINDOWS, TargetOS.LINUX])
    targetarchs([TargetArch.X86_64])

    with project("MaSalle"):
        windowedapp()
        language("C++")
        cppdialect("C++17")
        location(".")
        files(["src/**.cpp"])

        targetdir("build/bin/%{cfg.system}/%{cfg.buildcfg}")
        objdir("build/obj/%{cfg.system}/%{cfg.buildcfg}/%{prj.name}")

        # ----- Un chapitre, une ligne ---------------------------------------------
        # Ch. 3  NKWindow et NKEvent : l'aide du workspace Nkentseu, la fenetre et les evenements
        # from jengaconfig import *; nkentseudependson(["NKWindow", "NKEvent"], extra_includes=["src"])
        # Ch. 4  NKRHI et NKRenderer : le rendu, les shaders (GLSlang/SPIRVCross explicites), les en-tetes Vulkan et le define local de NKRHI
        # nkentseudependson(["NKRHI", "NKRenderer", "NKSL", "NKGLSlang", "NKSPIRVCross", "NKGlad"], extra_includes=["%{wks.location}/Externals/Libs/Vulkan-Headers-1.4.350/include"]); defines(["NK_RHI_VK_ENABLED"])
        # Ch. 5  Images, modeles, textes, sons : images, polices et modeles (glTF, FBX) viennent deja avec NKRenderer ; il manque le son
        # nkentseudependson(["NKAudio"])
        # Ch. 6  La tete, l'orientation et les deux yeux : le module XR (poses, espaces, simulateur)
        # nkentseudependson(["NKXR"])
        # Ch. 7  La cadence et la prediction : rien a lier (NKTime est deja la), seulement le code du chapitre
        # files(["src/cadence/**.cpp"])
        # Ch. 8  Les chaines d'echange : une chaine d'echange par oeil, dans NKXR et NKRHI deja lies
        # files(["src/swapchain/**.cpp"])
        # Ch. 9  Les actions : les entrees abstraites de NKXR, sur NKEvent deja lie
        # files(["src/actions/**.cpp"])
        # Ch. 10 La composition et les couches : couches de projection et quads
        # files(["src/couches/**.cpp"])
        # Ch. 11 Le vrai backend, deux backends : les en-tetes OpenXR (le chargeur est ouvert a l'execution, rien a lier ici)
        # includedirs(["%{wks.location}/Externals/Libs/NKOpenXR/include"])
        # Ch. 12 Rendre deux fois : deux vues, deux projections, dans le rendu deja lie
        # files(["src/stereo/**.cpp"])
        # Ch. 13 La porte s'ouvre : la camera (passthrough), d'apres le titre
        # nkentseudependson(["NKCamera"])
        # Ch. 14 Des panneaux qu'on lit, et le son qui place les choses : l'interface (le son spatial passe par NKAudio, deja lie)
        # nkentseudependson(["NKUI"])
        # Ch. 15 Quelqu'un d'autre entre : le reseau
        # nkentseudependson(["NKNetwork"])
        # Ch. 16 Batir et livrer : les informations du paquet
        # apppublisher("Mafo"); appversion("1.0.0"); licensefile("LICENSE")
        # Ch. 17 Approfondissement : pas de nouvelle dependance prevue ; relire le fichier en entier et retirer ce qui ne sert plus
        # pass

        # ----- Systemes (chapitre 2, exercice 8) -----------------------------------
        with filter("system:Windows"):
            defines(["WIN32_LEAN_AND_MEAN", "_UNICODE", "UNICODE"])
            links(["user32", "gdi32"])
            # Ch. 3  links(["dwmapi", "shell32"])
            # Ch. 4  links(["opengl32", "d3d11", "d3d12", "dxgi", "dxguid", "d3dcompiler", "uuid", "ole32"])
            # Ch. 11 links(["advapi32"])      # trouver le runtime OpenXR actif dans le registre
            # Ch. 15 links(["ws2_32"])        # sockets Windows

        with filter("system:Linux"):
            links(["pthread"])
            # Ch. 3  links(["X11", "Xext"]); defines(["NKENTSEU_FORCE_WINDOWING_XLIB_ONLY"])
            # Ch. 4  links(["GL"])

        # ----- Configurations (chapitre 2, exercice 10) ----------------------------
        with filter("config:Debug"):
            defines(["_DEBUG"])
            symbols(True)
            optimize("Off")

        with filter("config:Release"):
            defines(["NDEBUG"])
            symbols(False)
            optimize("Speed")
```

## Le programme

`src/main.cpp` n'est plus vide : il salue avant de s'arrêter. Comme `MaSalle` est une application fenêtrée, elle n'a pas de console sous Windows et un `std::cout` ne s'afficherait nulle part. Sous Windows, le programme ouvre donc une boîte de message avec `MessageBoxW`, qui vient de `user32`, déjà lié par le filtre Windows. Sous Linux, il écrit le même message sur la sortie standard.

## D'où viennent les chapitres

Les titres sont ceux des sprints 3 à 17 annoncés sur la plateforme du cours. Pour chacun, j'ai cherché dans le dépôt Nkentseu ce qu'il faudra ajouter au projet : le registre des modules (`config/modules.jenga`), le projet du module XR (`Kernel/Runtime/NKXR/NKXR.jenga`), la démonstration XR lue à l'exercice 11 et la feuille de route `XR_MISSION_IA.md`.

| Ch. | Sprint | Ce qu'il faudra décocher |
|---|---|---|
| 3 | NKWindow et NKEvent | l'aide `jengaconfig`, `NKWindow`, `NKEvent` ; `dwmapi`/`shell32` sous Windows, `X11`/`Xext` sous Linux |
| 4 | NKRHI et NKRenderer | le rendu et la chaîne des shaders, les en-têtes Vulkan, `NK_RHI_VK_ENABLED` ; les API graphiques sous Windows, `GL` sous Linux |
| 5 | Images, modèles, textes, sons | `NKAudio` (images, polices et modèles viennent déjà avec NKRenderer) |
| 6 | La tête, l'orientation et les deux yeux | `NKXR` |
| 7 | La cadence et la prédiction | le code du chapitre seulement |
| 8 | Les chaînes d'échange | le code du chapitre seulement |
| 9 | Les actions | le code du chapitre seulement |
| 10 | La composition et les couches | le code du chapitre seulement |
| 11 | Le vrai backend, deux backends | les en-têtes OpenXR ; `advapi32` sous Windows |
| 12 | Rendre deux fois | le code du chapitre seulement |
| 13 | La porte s'ouvre | `NKCamera` |
| 14 | Panneaux, son qui place les choses | `NKUI` |
| 15 | Quelqu'un d'autre entre | `NKNetwork` ; `ws2_32` sous Windows |
| 16 | Bâtir et livrer | éditeur, version et licence du paquet |
| 17 | Approfondissement | rien de prévu : relire le fichier et retirer ce qui ne sert plus |

Deux lignes restent des hypothèses. Pour le chapitre 13, « La porte s'ouvre » m'a fait penser au passthrough, donc à la caméra, mais le titre ne le dit pas. Pour les chapitres 7 à 10 et 12, je n'ai trouvé aucun module à ajouter : leur travail se fait dans NKXR et NKRenderer, déjà liés. Leur ligne ajoute donc seulement un dossier de sources, et je la corrigerai si le chapitre demande autre chose.

## Pourquoi les lignes se décochent une à une

- **Chaque ligne n'ajoute que ce qui est nouveau.** `nkentseudependson`, `files`, `includedirs`, `links` et `defines` s'accumulent d'un appel à l'autre, ce que j'ai vérifié dans le code de Jenga et de Nkentseu. Décocher le chapitre 6 ne demande donc jamais de toucher aux lignes des chapitres 3 à 5.
- **Une ligne principale par chapitre.** Quand un chapitre a besoin de plusieurs appels, ils sont sur la même ligne, séparés par `;`. Quand il concerne un système précis, une ligne de plus, marquée du même numéro, attend dans le filtre de ce système.
- **Les trois pièges de l'exercice 11 sont prévus dès le chapitre 4** : NKGLSlang et NKSPIRVCross écrits explicitement, les en-têtes Vulkan, et `NK_RHI_VK_ENABLED`.
- **Le chapitre 3 décoche aussi l'import de `jengaconfig`**, l'aide du workspace Nkentseu qui fournit `nkentseudependson`. À partir de là, le projet devra vivre dans ce workspace.

## Ce que j'ai vérifié

Tel qu'il est rendu, avec toutes les lignes des chapitres à venir en commentaire, le fichier se construit dans les deux configurations. Sous Windows x86_64, Jenga 2.8.0, chaîne `mingw`, PowerShell, depuis `C:\Users\HP\Documents\GitHub\ani-4087\chapitre-02\exo12-votre_premier_fil` :

```text
jenga clean
jenga build --config Debug
jenga build --config Release
```

```text
Configuration: Debug
Target:        Windows x86_64
Toolchain:     mingw
  1. MaSalle [WINDOWED_APP]
✓   [1/1] Compiled: main.cpp
✓ Built: build\bin\Windows\Debug\MaSalle.exe
✓ Build Successful

Configuration: Release
Target:        Windows x86_64
Toolchain:     mingw
  1. MaSalle [WINDOWED_APP]
✓   [1/1] Compiled: main.cpp
✓ Built: build\bin\Windows\Release\MaSalle.exe
✓ Build Successful
```

Je n'ai pas pu vérifier que le projet se construit une fois les lignes décochées : il faut pour cela le moteur Nkentseu complet, qui arrivera au chapitre 3.