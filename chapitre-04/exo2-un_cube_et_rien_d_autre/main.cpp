//
// main.cpp
// =============================================================================
// Description :
//   Dit pourquoi un cube pose devant la camera ne s'affiche pas, en testant
//   les causes dans un ordre fixe, puis affiche le bilan VISIBLES / EN PANNE.
//
// Caracteristiques :
//   - Seule la premiere cause qui s'applique est retenue.
//   - Les drapeaux sont lus dans un long long : NK_SS_ALL (4294967295)
//     ne tient pas dans un int signe de 32 bits.
// =============================================================================

#include <iostream>
#include <string>

static std::string Verdict(long long drapeaux,
                           long long sx,
                           long long sy,
                           long long sz,
                           long long distance,
                           long long lumieres,
                           long long ambiante,
                           long long proche) {
    // Sans le bit RENDER3D (valeur 2), rien n'est dessine en 3D.
    if ((drapeaux & 2) == 0) {
        return "RENDER3D ETEINT";
    }
    if (sx == 0 || sy == 0 || sz == 0) {
        return "ECHELLE NULLE";
    }

    // Les tailles sont paires : sz / 2 est exact en entiers.
    const long long faceAvant = distance - sz / 2;

    // Une face exactement sur la camera compte deja comme "dedans".
    if (faceAvant <= 0) {
        return "CAMERA DANS LE CUBE";
    }
    if (faceAvant < proche) {
        return "COUPE PAR LE PLAN PROCHE";
    }

    // Une ambiante seule suffit a voir le cube.
    if (lumieres == 0 && ambiante == 0) {
        return "PAS DE LUMIERE";
    }
    return "VISIBLE";
}

int main() {
    long long nombreCubes = 0;
    if (!(std::cin >> nombreCubes)) {
        nombreCubes = 0;
    }

    long long visibles = 0;
    long long enPanne = 0;

    for (long long i = 0; i < nombreCubes; ++i) {
        std::string nom;
        long long drapeaux = 0;
        long long sx = 0;
        long long sy = 0;
        long long sz = 0;
        long long distance = 0;
        long long lumieres = 0;
        long long ambiante = 0;
        long long proche = 0;

        // Une entree tronquee arrete proprement la lecture.
        if (!(std::cin >> nom >> drapeaux >> sx >> sy >> sz >> distance >> lumieres >> ambiante >> proche)) {
            break;
        }

        const std::string verdict = Verdict(drapeaux, sx, sy, sz, distance, lumieres, ambiante, proche);
        if (verdict == "VISIBLE") {
            ++visibles;
        } else {
            ++enPanne;
        }

        // Envoi immediat : si le programme est interrompu, la sortie montre
        // jusqu'ou il est alle.
        std::cout << nom << ' ' << verdict << '\n' << std::flush;
    }

    std::cout << "VISIBLES " << visibles << '\n';
    std::cout << "EN PANNE " << enPanne << '\n';
    return 0;
}
