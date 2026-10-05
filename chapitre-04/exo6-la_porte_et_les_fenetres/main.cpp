//
// main.cpp
// =============================================================================
// Description :
//   Verifie des panneaux plaques sur un mur (porte, fenetres, tableau) :
//   affiche la saillie de chacun et son verdict, puis le bilan
//   OK / A REPRENDRE.
//
// Caracteristiques :
//   - Les verdicts sont testes dans l'ordre impose ; le premier gagne.
//   - Les tests de debordement sont faits en doublant les deux membres :
//     on evite ainsi la division de W par 2, qui tronquerait un W impair.
// =============================================================================

#include <iostream>
#include <string>

// Vrai si le panneau sort du mur. Les bords peuvent coincider :
// on ne refuse que les depassements stricts.
static bool Deborde(long long largeurMur,
                    long long hauteurMur,
                    long long u,
                    long long y,
                    long long l,
                    long long h) {
    // u - l / 2 < -W / 2, multiplie par 2 des deux cotes.
    if (2 * u - l < -largeurMur) {
        return true;
    }
    if (2 * u + l > largeurMur) {
        return true;
    }
    if (2 * y - h < 0) {
        return true;
    }
    if (2 * y + h > 2 * hauteurMur) {
        return true;
    }
    return false;
}

int main() {
    long long largeurMur = 0;
    long long hauteurMur = 0;
    long long seuil = 0;
    if (!(std::cin >> largeurMur >> hauteurMur >> seuil)) {
        largeurMur = 0;
        hauteurMur = 0;
        seuil = 0;
    }

    long long nombrePanneaux = 0;
    if (!(std::cin >> nombrePanneaux)) {
        nombrePanneaux = 0;
    }

    long long ok = 0;
    long long aReprendre = 0;

    for (long long i = 0; i < nombrePanneaux; ++i) {
        std::string nom;
        long long u = 0;
        long long y = 0;
        long long l = 0;
        long long h = 0;
        long long e = 0;
        long long d = 0;

        // Une entree tronquee arrete proprement la lecture.
        if (!(std::cin >> nom >> u >> y >> l >> h >> e >> d)) {
            break;
        }

        // L'epaisseur est paire : e / 2 est exact en entiers.
        const long long saillie = d + e / 2;
        const long long faceArriere = d - e / 2;

        std::string verdict = "OK";
        if (Deborde(largeurMur, hauteurMur, u, y, l, h)) {
            verdict = "DEBORDE";
        } else if (saillie <= 0) {
            verdict = "INVISIBLE";
        } else if (saillie < seuil) {
            verdict = "CLIGNOTE";
        } else if (faceArriere > seuil) {
            verdict = "DECOLLE";
        }

        if (verdict == "OK") {
            ++ok;
        } else {
            ++aReprendre;
        }

        // La saillie s'affiche toujours, meme quand le panneau deborde.
        std::cout << nom << ' ' << saillie << ' ' << verdict << '\n' << std::flush;
    }

    std::cout << "OK " << ok << '\n';
    std::cout << "A REPRENDRE " << aReprendre << '\n';
    return 0;
}
