// Chapitre 10, exercice 2 : la trame vide.
// Joue une boucle de rendu image par image. Chaque image soumet sa fin de
// trame : une couche (lacet + index) si l'image est rendue, zero couche
// sinon. L'anneau de la chaine n'avance que sur une image rendue.
#include <iostream>
#include <string>

int main() {
    long long n = 0;
    long long nbImages = 0;
    if (!(std::cin >> n >> nbImages)) {
        return 0;
    }
    if (n < 1 || n > 4) {
        std::cout << "CHAINE REFUSEE\n";
        return 0;
    }
    std::string signes;
    if (nbImages > 0) {
        std::cin >> signes;
    }

    long long prochaine = 0;
    long long fins = 0, rendues = 0, sautees = 0;

    for (long long t = 1; t <= nbImages; ++t) {
        const char s = (t - 1 < static_cast<long long>(signes.size()))
                           ? signes[static_cast<size_t>(t - 1)] : '0';
        const long long lacet = t - 1;  // un degre par image, rendue ou non

        if (s == '1') {
            const long long i = prochaine;      // emprunter, dessiner, rendre
            prochaine = (i + 1) % n;
            std::cout << t << " OUI 1 " << lacet << ' ' << i << '\n';
            ++rendues;
        } else {
            std::cout << t << " NON 0\n";      // fin de trame sans couche
            ++sautees;
        }
        ++fins;                                  // une fin de trame par image
    }

    std::cout << "FINS " << fins << '\n';
    std::cout << "RENDUES " << rendues << '\n';
    std::cout << "SAUTEES " << sautees << '\n';
    std::cout << "PROCHAINE " << prochaine << '\n';
    return 0;
}
