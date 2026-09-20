# Chapitre 02 — Exo 11 : le fichier de projet lu en entier

Fichier lu : `Applications/NKXRDemo/NKXRDemo.jenga`, dans le dépôt Nkentseu (77 lignes, dernière modification le 2026-08-11, commit `831a5d60`). Je l'ai lu en entier, commentaires et docstring compris. Je n'ai pas construit la démo : les conséquences décrites plus bas sont déduites des commentaires du fichier et du code source qu'ils citent.

## Ce qu'il construit

Un seul projet, `NKXRDemo`, de type application fenêtrée (`windowedapp()`), en C++17, à partir de tous les fichiers `src/**.cpp`. La docstring le présente comme l'« étage 0 » de la mission NKXR : une scène NKRenderer rendue en **stéréo simulée** sur un écran de bureau, sans casque. La souris joue le rôle de la tête, ZQSD/WASD déplacent, et les deux yeux sont affichés côte à côte. Un renderer « compositeur » possède l'image, et chaque œil a son propre renderer qui dessine hors écran. Trois variables d'environnement servent de crochets pour un agent de test : `NK_XR_SIM_POSE` (pose figée), `NK_XR_SHOT` (capture par œil) et `NK_XR_EXIT`. L'exécutable sort dans `Build/Bin/<config>-<système>/NKXRDemo`, avec des métadonnées de paquet : éditeur « Rihen Universe », version 0.1.0, licence du dépôt.

## Ce dont il dépend

Un seul appel, `nkentseudependson([...])`, déclare **23 modules** (les mêmes que dans l'exercice 7) et génère pour chacun les dossiers d'inclusion, les liens et l'ordre de construction. On y trouve la réalité étendue (NKXR), le rendu (NKRenderer, NKRHI, NKSL et ses compilateurs NKGLSlang et NKSPIRVCross, NKGlad), les ressources (NKFont, NKImage, NKSerialization, NKReflection, NKFileSystem, NKStream), la fenêtre et les événements (NKWindow, NKEvent), et le socle (NKMath, NKTime, NKLogger, NKContainers, NKMemory, NKCore, NKPlatform, NKThreading). S'y ajoutent `src` et les en-têtes Vulkan 1.4.350 fournis dans `Externals`.

## Ce qui change d'un système à l'autre

| | Windows (hors UWP et Xbox) | Linux (fenêtrage XLib, le défaut) | macOS |
|---|---|---|---|
| Chaîne | `TC_WINDOWS` (`nk-windows-clang-mingw`) | `clang-native` | `clang-native` |
| Définitions | `WIN32_LEAN_AND_MEAN`, `_UNICODE`, `UNICODE` | `NKENTSEU_FORCE_WINDOWING_XLIB_ONLY` | aucune |
| Bibliothèques | `user32`, `gdi32`, `opengl32`, `dwmapi`, `shell32`, `advapi32`, `d3d11`, `d3d12`, `dxgi`, `dxguid`, `d3dcompiler`, `uuid`, `ole32` | `pthread`, `X11`, `Xext`, `GL` | frameworks `Cocoa`, `QuartzCore`, `OpenGL` |

Le filtre Linux ne s'active que si l'option `linux-backend=xlib` est posée, ou si aucune option `linux-backend` ni `headless` n'est donnée. Le fichier ne contient aucun filtre pour Android, le Web, Wayland ni UWP. Les réglages Debug (`optimize("Off")`, symboles) et Release (`optimize("Speed")`, `NDEBUG`) sont communs à tous les systèmes.

Un commentaire justifie aussi `advapi32` sous Windows : il fournit `RegGetValueA`, qui sert à trouver dans le registre le runtime OpenXR actif. Sans cette bibliothèque, l'édition de liens échouerait sous Windows sur une référence indéfinie à `RegGetValueA`.

## Les trois pièges documentés

**1. NKGLSlang et NKSPIRVCross écrits explicitement** (lignes 22 à 24). NKSL appelle des fonctions de glslang et de SPIRV-Cross, mais l'édition de liens de l'exécutable final ne les récupère pas d'elle-même : dépendre de NKSL ou de NKRHI ne suffit pas. *Sans ces deux noms*, tout compilerait, puis l'**édition de liens** échouerait sur des références indéfinies vers les fonctions de `glslang::` et `spirv_cross::`. C'est l'erreur de l'exercice 4, mais à travers une dépendance indirecte. Le piège a d'abord été rencontré, et documenté, dans `NkLocomotionDemo.jenga` le 2026-07-23.

**2. Le dossier des en-têtes Vulkan** (lignes 31 et 32). `NkVulkanDevice.h`, qu'utilise la liaison OpenXR de la démo, fait `#include <vulkan/vulkan.h>`. *Sans ce dossier d'inclusion*, et sans SDK Vulkan installé sur la machine, le **préprocesseur** s'arrêterait sur `vulkan/vulkan.h: No such file or directory`, à la première ligne de `main.cpp` qui inclut cet en-tête.

**3. La définition `NK_RHI_VK_ENABLED`** (lignes 35 à 38). C'est une définition locale à NKRHI, qui ne se propage pas aux projets qui en dépendent. Dans `NkVulkanDevice.h`, elle choisit entre la vraie classe `NkVulkanDevice` et une **classe vide** qui ne fait que renvoyer des valeurs nulles. *Sans cette ligne*, la démo recevrait la classe vide, et la **compilation** échouerait sur `vkDevice->GetVkInstance()` et `GetVkDevice()`, des méthodes qui n'existent que dans la vraie classe. C'est exactement la situation de l'exercice 5 : le message parlerait de membres absents, sans jamais nommer la définition manquante.

## Ce que je retiens

Les trois pièges tombent chacun à une étape différente de la chaîne : le préprocesseur pour les en-têtes Vulkan, la compilation pour la définition, l'édition de liens pour les bibliothèques. Dans les trois cas, le message d'erreur désignerait le code C++, pas la ligne du `.jenga` qui manque, et c'est pour cela que ces lignes sont commentées.
