//
// lumiere.cpp
// =============================================================================
// Description :
//   Calcule, face par face, la lumiere que recoit la salle sous plusieurs
//   soleils, puis le contraste entre la face la plus eclairee et la plus
//   sombre.
//
// Caracteristiques :
//   - Directions, intensites et lumieres en milliemes.
//   - Une face recoit ambiante + I * max(0, c), arrondi a l'entier le plus
//     proche, avec c = -(normale . direction) / |direction| : la direction est
//     NORMALISEE, sinon un soleil oblique eclairerait trop fort.
//   - Une face tournee dans le sens de la lumiere garde l'ambiante.
// =============================================================================

#include <cmath>
#include <iostream>
#include <string>

struct Face {
    const char *nom;
    double nx;
    double ny;
    double nz;
};

// Les cinq faces et leurs normales interieures, dans l'ordre impose.
static const Face FACES[5] = {
    {"SOL", 0.0, 1.0, 0.0},
    {"FOND", 0.0, 0.0, 1.0},
    {"ENTREE", 0.0, 0.0, -1.0},
    {"GAUCHE", 1.0, 0.0, 0.0},
    {"DROIT", -1.0, 0.0, 0.0},
};

int main() {
    long long ambiante = 0;
    long long nombreSoleils = 0;
    if (!(std::cin >> ambiante >> nombreSoleils)) {
        return 0;
    }

    for (long long i = 0; i < nombreSoleils; ++i) {
        std::string nom;
        double dx = 0.0;
        double dy = 0.0;
        double dz = 0.0;
        double intensite = 0.0;

        // Une entree tronquee arrete proprement la lecture.
        if (!(std::cin >> nom >> dx >> dy >> dz >> intensite)) {
            break;
        }

        const double longueur = std::sqrt(dx * dx + dy * dy + dz * dz);

        long long plusClaire = 0;
        long long plusSombre = 0;
        for (int f = 0; f < 5; ++f) {
            const Face &face = FACES[f];

            // Le soleil eclaire une face quand sa direction s'oppose a la
            // normale : d'ou le changement de signe.
            double c = 0.0;
            if (longueur > 0.0) {
                c = -(face.nx * dx + face.ny * dy + face.nz * dz) / longueur;
            }
            if (c < 0.0) {
                c = 0.0;
            }

            const long long lumiere = std::llround((double)ambiante + intensite * c);
            if (f == 0 || lumiere > plusClaire) {
                plusClaire = lumiere;
            }
            if (f == 0 || lumiere < plusSombre) {
                plusSombre = lumiere;
            }

            std::cout << nom << ' ' << face.nom << ' ' << lumiere << '\n';
        }

        std::cout << nom << " CONTRASTE " << plusClaire - plusSombre << '\n';
    }

    return 0;
}
