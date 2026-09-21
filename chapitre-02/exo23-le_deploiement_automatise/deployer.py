#!/usr/bin/env python3
"""
deployer.py - construit, empaquette, signe et installe MaSalle sur un appareil
Android, et s'arrete a la PREMIERE etape qui echoue en disant laquelle.

    python deployer.py                 # les quatre etapes
    python deployer.py --propre        # commence par "jenga clean"
    python deployer.py --sans-installer  # s'arrete apres la signature

Rien n'est code en dur pour une machine precise : tout se regle par des
variables d'environnement ou des options (voir --help).

Code de retour : 0 si tout va bien, sinon le numero de l'etape qui a echoue
(1 construire, 2 empaqueter, 3 signer, 4 installer), et 9 si la
verification des outils a echoue avant meme de commencer.
"""

import argparse
import os
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

ICI = Path(__file__).resolve().parent
WINDOWS = os.name == "nt"
PLATEFORME = "android-arm64"
ABI = "arm64-v8a"
NOM = "MaSalle"

ETAPES = {1: "Construire", 2: "Empaqueter", 3: "Signer", 4: "Installer"}


class Echec(Exception):
    """Une etape a echoue : `etape` est son numero, 0 pour la verification."""

    def __init__(self, etape: int, raison: str):
        super().__init__(raison)
        self.etape = etape
        self.raison = raison


# -----------------------------------------------------------------------------
#  Outils : trouves sur CETTE machine, jamais supposes
# -----------------------------------------------------------------------------
def trouver_sdk() -> Path:
    for variable in ("ANDROID_SDK_ROOT", "ANDROID_HOME"):
        valeur = os.environ.get(variable)
        if valeur and Path(valeur).is_dir():
            return Path(valeur)
    raise Echec(0, "SDK Android introuvable : definissez ANDROID_SDK_ROOT "
                   "(ou ANDROID_HOME) vers le dossier du SDK.")


def trouver_ndk(sdk: Path) -> Path:
    """Ajout apres l'essai sur une autre machine : sans NDK, Jenga ne peut pas
    construire pour Android, et l'erreur n'apparaissait qu'a l'etape 1."""
    for variable in ("ANDROID_NDK_ROOT", "ANDROID_NDK_HOME"):
        valeur = os.environ.get(variable)
        if valeur and Path(valeur).is_dir():
            return Path(valeur)
    installes = sorted((d for d in (sdk / "ndk").glob("*") if d.is_dir()),
                       key=lambda d: version_cle(d.name))
    if installes:
        return installes[-1]
    raise Echec(0, "NDK Android introuvable : definissez ANDROID_NDK_ROOT, ou "
                   "installez-le avec sdkmanager \"ndk;26.3.11579264\".")


def version_cle(nom: str):
    try:
        return tuple(int(p) for p in nom.split("."))
    except ValueError:
        return (-1,)


def trouver_build_tools(sdk: Path) -> Path:
    dossier = sdk / "build-tools"
    versions = sorted((d for d in dossier.glob("*") if d.is_dir()),
                      key=lambda d: version_cle(d.name))
    if not versions:
        raise Echec(0, f"Aucun build-tools dans {dossier} : "
                       "installez-en un avec sdkmanager \"build-tools;34.0.0\".")
    return versions[-1]                     # la plus recente installee


def outil(chemin: Path, quoi: str) -> Path:
    if not chemin.exists():
        raise Echec(0, f"{quoi} introuvable : {chemin}")
    return chemin


def commande(nom: str, quoi: str) -> str:
    trouve = shutil.which(nom)
    if not trouve:
        raise Echec(0, f"{quoi} introuvable dans le PATH (commande '{nom}').")
    return trouve


# -----------------------------------------------------------------------------
#  Aides
# -----------------------------------------------------------------------------
def lancer(etape: int, cmd: list, interactif: bool = False) -> None:
    print("  >", " ".join(f'"{c}"' if " " in str(c) else str(c) for c in cmd))
    try:
        r = subprocess.run([str(c) for c in cmd], cwd=ICI)
    except OSError as e:
        raise Echec(etape, f"impossible de lancer {cmd[0]} : {e}")
    if r.returncode != 0:
        raise Echec(etape, f"{Path(str(cmd[0])).name} a renvoye le code {r.returncode}.")


def architecture_elf(fichier: Path) -> int:
    with open(fichier, "rb") as f:
        entete = f.read(20)
    if entete[:4] != b"\x7fELF":
        return -1
    return int.from_bytes(entete[18:20], "little")   # 183 = AArch64


# -----------------------------------------------------------------------------
#  Les quatre etapes
# -----------------------------------------------------------------------------
def construire(jenga: str, config: str, propre: bool) -> Path:
    if propre:
        lancer(1, [jenga, "clean", "--platform", PLATEFORME, "--config", config])
    lancer(1, [jenga, "build", "--platform", PLATEFORME, "--config", config])

    sortie = ICI / "build" / "bin" / "Android" / config
    bibliotheque = sortie / f"lib{NOM}.so"
    apk = sortie / f"android-build-{ABI}" / f"{NOM}-{config}.apk"
    if not bibliotheque.exists():
        raise Echec(1, f"jenga dit avoir reussi, mais {bibliotheque} n'existe pas.")
    if architecture_elf(bibliotheque) != 183:
        raise Echec(1, f"{bibliotheque.name} n'est pas une bibliotheque arm64 "
                       "(un autre build l'a ecrasee ?) : relancez avec --propre.")
    if not apk.exists():
        raise Echec(1, f"APK non signe introuvable : {apk}")
    print(f"  bibliotheque arm64 : {bibliotheque.stat().st_size} octets")
    return apk


def empaqueter(apk_brut: Path) -> Path:
    dist = ICI / "dist"
    dist.mkdir(exist_ok=True)
    non_signe = dist / f"{NOM}-non-signe.apk"
    shutil.copy2(apk_brut, non_signe)

    try:
        with zipfile.ZipFile(non_signe) as z:
            noms = z.namelist()
            if z.testzip() is not None:
                raise Echec(2, "l'APK est endommage (testzip).")
    except zipfile.BadZipFile:
        raise Echec(2, f"{non_signe.name} n'est pas une archive valide.")

    attendus = ["AndroidManifest.xml", f"lib/{ABI}/lib{NOM}.so"]
    manquants = [n for n in attendus if n not in noms]
    if manquants:
        raise Echec(2, f"il manque dans l'APK : {', '.join(manquants)}")
    autres_abi = sorted({n.split('/')[1] for n in noms
                         if n.startswith("lib/") and n.count("/") >= 2} - {ABI})
    if autres_abi:
        print(f"  attention : l'APK contient aussi {', '.join(autres_abi)}")
    print(f"  {non_signe.name} : {non_signe.stat().st_size} octets, {len(noms)} fichiers")
    return non_signe


def signer(build_tools: Path, non_signe: Path, cle: Path, alias: str) -> Path:
    apksigner = outil(build_tools / ("apksigner.bat" if WINDOWS else "apksigner"),
                      "apksigner")
    if not cle.exists():
        raise Echec(3, f"cle de signature introuvable : {cle} "
                       "(option --cle ou variable MASALLE_CLE).")
    signe = ICI / "dist" / f"{NOM}.apk"
    print("  (apksigner va demander le mot de passe de la cle)")
    lancer(3, [apksigner, "sign", "--ks", cle, "--ks-key-alias", alias,
               "--out", signe, non_signe], interactif=True)
    lancer(3, [apksigner, "verify", signe])
    print(f"  {signe.name} : {signe.stat().st_size} octets, signature verifiee")
    return signe


def installer(adb: Path, signe: Path) -> None:
    r = subprocess.run([str(adb), "devices"], capture_output=True, text=True)
    lignes = [l.split() for l in r.stdout.splitlines()[1:] if l.strip()]
    prets = [l[0] for l in lignes if len(l) >= 2 and l[1] == "device"]
    autres = [f"{l[0]} ({l[1]})" for l in lignes if len(l) >= 2 and l[1] != "device"]
    if not prets:
        detail = f" Vus mais non utilisables : {', '.join(autres)}." if autres else ""
        raise Echec(4, "aucun appareil pret : branchez-le, deverrouillez-le et "
                       "autorisez le debogage USB." + detail)
    if len(prets) > 1:
        raise Echec(4, f"plusieurs appareils branches ({', '.join(prets)}) : "
                       "gardez-en un seul.")
    print(f"  appareil : {prets[0]}")
    lancer(4, [adb, "install", "-r", signe])


# -----------------------------------------------------------------------------
def main() -> int:
    p = argparse.ArgumentParser(description="Construire, empaqueter, signer, installer.")
    p.add_argument("--config", default="Release")
    p.add_argument("--propre", action="store_true",
                   help="lancer 'jenga clean' avant de construire")
    p.add_argument("--cle", default=os.environ.get(
        "MASALLE_CLE", str(Path.home() / "cles" / "masalle.jks")),
        help="fichier de cle (defaut : MASALLE_CLE, sinon ~/cles/masalle.jks)")
    p.add_argument("--alias", default=os.environ.get("MASALLE_ALIAS", "masalle"))
    p.add_argument("--sans-installer", action="store_true",
                   help="s'arreter apres la signature (pas d'appareil)")
    args = p.parse_args()

    etape = 0
    try:
        print("[0/4] Verifier les outils")
        jenga = commande("jenga", "jenga")
        sdk = trouver_sdk()
        build_tools = trouver_build_tools(sdk)
        ndk = trouver_ndk(sdk)
        adb = outil(sdk / "platform-tools" / ("adb.exe" if WINDOWS else "adb"), "adb")
        print(f"  SDK : {sdk}\n  build-tools : {build_tools.name}\n  NDK : {ndk}")

        etape = 1; print(f"\n[1/4] {ETAPES[1]}")
        apk_brut = construire(jenga, args.config, args.propre)

        etape = 2; print(f"\n[2/4] {ETAPES[2]}")
        non_signe = empaqueter(apk_brut)

        etape = 3; print(f"\n[3/4] {ETAPES[3]}")
        signe = signer(build_tools, non_signe, Path(args.cle).expanduser(), args.alias)

        if args.sans_installer:
            print(f"\nTermine sans installation : {signe}")
            return 0

        etape = 4; print(f"\n[4/4] {ETAPES[4]}")
        installer(adb, signe)

    except Echec as e:
        nom = ETAPES.get(e.etape, "Verification des outils")
        print(f"\nECHEC a l'etape {e.etape}/4 : {nom}")
        print(f"  {e.raison}")
        return e.etape if e.etape else 9
    except KeyboardInterrupt:
        print(f"\nINTERROMPU pendant l'etape {etape}/4 : {ETAPES.get(etape, 'Verification')}")
        return etape or 9

    print("\nSUCCES : construit, empaquete, signe et installe.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
