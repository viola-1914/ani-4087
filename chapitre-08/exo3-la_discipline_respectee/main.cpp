// Chapitre 8, exercice 3 : la discipline respectee.
// Suit les appels faits a une chaine d'echange
// et refuse chacun de ceux qui violent la discipline.
#include <iostream>
#include <string>

int main() {
    int n = 1;
    long long nbOrdres = 0;
    if (!(std::cin >> n >> nbOrdres)) {
        std::cout << "SOUMISES 0\nREFUS 0\nHORS EMPRUNT 0\n";
        return 0;
    }
    if (n < 1) {
        n = 1;
    }

    int prochaine = 0;
    bool empruntee = false;
    int imageEmpruntee = 0;
    bool dessinee = false;            // l'image empruntee a recu un draw
    bool renduDepuisSoumission = false;
    bool derniereRendueDessinee = false;

    long long soumises = 0;
    long long refus = 0;
    long long horsEmprunt = 0;

    for (long long i = 0; i < nbOrdres; ++i) {
        std::string ordre;
        if (!(std::cin >> ordre)) {
            break;
        }

        if (ordre == "acquire") {
            if (empruntee) {
                std::cout << "ACQUIRE REFUSE\n";
                ++refus;
            } else {
                empruntee = true;
                imageEmpruntee = prochaine;
                dessinee = false;
                std::cout << "ACQUIRE " << imageEmpruntee << '\n';
            }
        } else if (ordre == "draw") {
            if (empruntee) {
                dessinee = true;
                std::cout << "DRAW " << imageEmpruntee << '\n';
            } else {
                std::cout << "DRAW HORS EMPRUNT\n";
                ++horsEmprunt;
            }
        } else if (ordre == "release") {
            if (!empruntee) {
                std::cout << "RELEASE REFUSE\n";
                ++refus;
            } else {
                prochaine = (prochaine + 1) % n;
                empruntee = false;
                renduDepuisSoumission = true;
                derniereRendueDessinee = dessinee;
                std::cout << "RELEASE OK\n";
            }
        } else if (ordre == "submit") {
            if (empruntee) {
                std::cout << "SUBMIT REFUSE EMPRUNTEE\n";
                ++refus;
            } else if (!renduDepuisSoumission) {
                std::cout << "SUBMIT REFUSE RIEN\n";
                ++refus;
            } else {
                const int j = (prochaine + n - 1) % n;
                std::cout << "SUBMIT " << j << (derniereRendueDessinee ? "" : " VIDE") << '\n';
                ++soumises;
                renduDepuisSoumission = false;
            }
        }
    }

    std::cout << "SOUMISES " << soumises << '\n';
    std::cout << "REFUS " << refus << '\n';
    std::cout << "HORS EMPRUNT " << horsEmprunt << '\n';
    return 0;
}
