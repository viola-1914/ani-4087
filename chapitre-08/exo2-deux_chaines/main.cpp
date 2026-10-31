// Chapitre 8, exercice 2 : deux chaines.
// Cree des chaines d'echange a partir de leur description,
// dit lesquelles sont refusees et fait le bilan par oeil.
#include <iostream>
#include <string>

int main() {
    const long long kEmplacementsParChaine = 4;
    const long long kImagesMax = 4;

    long long n = 0;
    std::cin >> n;

    long long acceptees = 0;
    long long occupes = 0;
    long long gauche = 0;
    long long droit = 0;
    long long sansNom = 0;

    for (long long i = 0; i < n; ++i) {
        long long largeur = 0, hauteur = 0, images = 0;
        int oeil = 0;
        std::string nom;
        if (!(std::cin >> largeur >> hauteur >> images >> oeil >> nom)) {
            break;
        }
        const bool estSansNom = (nom == "-");
        const std::string affiche = estSansNom ? "SANS-NOM" : nom;

        if (largeur == 0 || hauteur == 0) {
            std::cout << "REFUSEE " << affiche << " TAILLE\n";
            continue;
        }
        if (images == 0 || images > kImagesMax) {
            std::cout << "REFUSEE " << affiche << " IMAGES\n";
            continue;
        }

        std::cout << "CHAINE " << affiche << ' ' << largeur << ' ' << hauteur << ' '
                  << images << ' ' << (oeil == 0 ? "GAUCHE" : "DROIT") << '\n';
        ++acceptees;
        occupes += images;
        if (oeil == 0) {
            ++gauche;
        } else {
            ++droit;
        }
        if (estSansNom) {
            ++sansNom;
        }
    }

    std::cout << "ACCEPTEES " << acceptees << '\n';
    std::cout << "EMPLACEMENTS " << acceptees * kEmplacementsParChaine << '\n';
    std::cout << "OCCUPES " << occupes << '\n';
    std::cout << "GAUCHE " << gauche << '\n';
    std::cout << "DROIT " << droit << '\n';
    std::cout << "SANS NOM " << sansNom << '\n';
    return 0;
}
