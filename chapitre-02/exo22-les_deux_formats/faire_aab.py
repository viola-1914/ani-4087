"""
faire_aab.py - fabrique un Android App Bundle (.aab) de MaSalle.

Pourquoi un script : `jenga package --type aab` passe a bundletool un DOSSIER
et un manifeste au format binaire, alors que bundletool attend un module ZIP
dont le manifeste et les ressources sont au format « proto » (protobuf).
Ce script fait les trois etapes a la main :

  1. aapt2 link --proto-format : manifeste + ressources au format proto ;
  2. assemblage du module base.zip :
        manifest/AndroidManifest.xml   resources.pb   lib/arm64-v8a/libMaSalle.so
  3. bundletool build-bundle : base.zip -> dist/MaSalle.aab

A lancer APRES `jenga build --platform android-arm64 --config Release`,
depuis le dossier de l'exercice :  python faire_aab.py
"""

import os
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

ICI = Path(__file__).resolve().parent
SDK = Path(os.environ.get("ANDROID_SDK_ROOT", r"C:\Android"))
BUILD_TOOLS = SDK / "build-tools" / "34.0.0"
ANDROID_JAR = SDK / "platforms" / "android-34" / "android.jar"
AAPT2 = BUILD_TOOLS / ("aapt2.exe" if os.name == "nt" else "aapt2")
BUNDLETOOL = ICI / "tools" / "bundletool-all-1.18.3.jar"

BUILD = ICI / "build" / "bin" / "Android" / "Release"
MANIFESTE = BUILD / "android-build-arm64-v8a" / "AndroidManifest.xml"
BIBLIOTHEQUE = BUILD / "libMaSalle.so"

TRAVAIL = ICI / "build" / "aab"
SORTIE = ICI / "dist" / "MaSalle.aab"


def verifier(chemin: Path, quoi: str) -> None:
    if not chemin.exists():
        print(f"ERREUR : {quoi} introuvable : {chemin}")
        sys.exit(1)


def verifier_arm64(bibliotheque: Path) -> None:
    """Refuse une bibliotheque qui n'est pas compilee pour ARM 64 bits.

    Octets 18-19 de l'en-tete ELF = architecture : 183 = AArch64, 62 = x86-64.
    """
    with open(bibliotheque, "rb") as f:
        entete = f.read(20)
    machine = int.from_bytes(entete[18:20], "little")
    noms = {183: "AArch64 (arm64-v8a)", 62: "x86-64", 40: "ARM 32 bits"}
    print(f"Architecture de {bibliotheque.name} : {noms.get(machine, machine)}")
    if machine != 183:
        print("ERREUR : ce n'est pas une bibliotheque arm64. Relancez :")
        print("  jenga clean --platform android-arm64 --config Release")
        print("  jenga build --platform android-arm64 --config Release")
        sys.exit(1)


def lancer(commande: list) -> None:
    print(">", " ".join(str(c) for c in commande))
    resultat = subprocess.run([str(c) for c in commande])
    if resultat.returncode != 0:
        print(f"ERREUR : la commande a echoue (code {resultat.returncode})")
        sys.exit(resultat.returncode)


def main() -> None:
    verifier(AAPT2, "aapt2")
    verifier(ANDROID_JAR, "android.jar")
    verifier(BUNDLETOOL, "bundletool (a telecharger dans tools/)")
    verifier(MANIFESTE, "le manifeste genere par jenga build")
    verifier(BIBLIOTHEQUE, "libMaSalle.so genere par jenga build")
    verifier_arm64(BIBLIOTHEQUE)

    if TRAVAIL.exists():
        shutil.rmtree(TRAVAIL)
    TRAVAIL.mkdir(parents=True)
    SORTIE.parent.mkdir(parents=True, exist_ok=True)

    # 1. Manifeste et ressources au format proto
    proto = TRAVAIL / "base-proto.zip"
    lancer([AAPT2, "link", "--proto-format",
            "-I", ANDROID_JAR,
            "--manifest", MANIFESTE,
            "-o", proto])

    # 2. Module base.zip, a la structure qu'attend bundletool
    module = TRAVAIL / "base.zip"
    with zipfile.ZipFile(proto) as source, \
         zipfile.ZipFile(module, "w", zipfile.ZIP_DEFLATED) as cible:
        for entree in source.namelist():
            donnees = source.read(entree)
            if entree == "AndroidManifest.xml":
                cible.writestr("manifest/AndroidManifest.xml", donnees)
            else:                                   # resources.pb, res/...
                cible.writestr(entree, donnees)
        cible.write(BIBLIOTHEQUE, "lib/arm64-v8a/libMaSalle.so")
    print(f"Module assemble : {module}")

    # 3. Le bundle
    if SORTIE.exists():
        SORTIE.unlink()
    lancer(["java", "-jar", BUNDLETOOL, "build-bundle",
            f"--modules={module}", f"--output={SORTIE}"])

    print(f"\nAAB produit : {SORTIE} ({SORTIE.stat().st_size} octets)")
    print("Il n'est pas encore signe : voir jarsigner.")


if __name__ == "__main__":
    main()
