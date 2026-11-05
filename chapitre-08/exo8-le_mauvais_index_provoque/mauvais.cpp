// Chapitre 8, exercice 8 : le mauvais index provoque.
// Simule une chaine de n images ou la soumission lit l'index de la
// derniere image relachee a trois moments possibles :
//   A : l'index emprunte ;
//   P : GetLastReleasedIndex lu AVANT ReleaseImage (la trame d'avant) ;
//   D : GetLastReleasedIndex lu APRES ReleaseImage (la bonne lecture).
// Affiche, trame par trame, la position montree et le saut depuis la
// trame precedente, puis le bilan des sauts.
#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

// Ecrit un nombre sans zeros inutiles : 10 -> "10", 2.5 -> "2.5".
static std::string Nombre(double x) {
    if (std::fabs(x - std::round(x)) < 1e-9) {
        char tampon[32];
        std::snprintf(tampon, sizeof(tampon), "%lld", static_cast<long long>(std::llround(x)));
        return tampon;
    }
    char tampon[64];
    std::snprintf(tampon, sizeof(tampon), "%.6g", x);
    return tampon;
}

int main() {
    int n = 1;
    long long t = 0;
    double x0 = 0.0, v = 0.0;
    if (!(std::cin >> n >> t >> x0 >> v)) {
        return 0;
    }
    if (n < 1) {
        n = 1;
    }
    std::string modes;
    if (t > 0) {
        std::cin >> modes;
    }

    std::vector<long long> peinte(static_cast<size_t>(n), 0);  // derniere trame peinte par emplacement
    int prochaine = 0;

    bool precedenteMontree = false;
    double positionPrecedente = 0.0;
    bool unSaut = false;
    double sautMin = 0.0, sautMax = 0.0;

    for (long long trame = 1; trame <= t; ++trame) {
        const char mode = (trame - 1 < static_cast<long long>(modes.size()))
                              ? modes[static_cast<size_t>(trame - 1)] : 'D';

        // Emprunter la prochaine image et y peindre la trame.
        const int empruntee = prochaine;
        peinte[static_cast<size_t>(empruntee)] = trame;

        int soumise = empruntee;                 // mode A
        if (mode == 'P') {
            soumise = (prochaine + n - 1) % n;   // lu avant ReleaseImage
        }
        prochaine = (prochaine + 1) % n;         // ReleaseImage
        if (mode == 'D') {
            soumise = (prochaine + n - 1) % n;   // lu apres ReleaseImage
        }

        const long long f = peinte[static_cast<size_t>(soumise)];
        if (f == 0) {
            std::cout << trame << " NOIR -\n";
            precedenteMontree = false;
            continue;
        }

        const double position = x0 + v * static_cast<double>(f);
        std::cout << trame << ' ' << Nombre(position) << ' ';
        if (trame == 1 || !precedenteMontree) {
            std::cout << "-\n";
        } else {
            const double saut = position - positionPrecedente;
            std::cout << Nombre(saut) << '\n';
            if (!unSaut) {
                sautMin = sautMax = saut;
                unSaut = true;
            } else {
                if (saut < sautMin) {
                    sautMin = saut;
                }
                if (saut > sautMax) {
                    sautMax = saut;
                }
            }
        }
        precedenteMontree = true;
        positionPrecedente = position;
    }

    if (!unSaut) {
        std::cout << "SAUTS AUCUN\n";
    } else {
        std::cout << "SAUT MIN " << Nombre(sautMin) << '\n';
        std::cout << "SAUT MAX " << Nombre(sautMax) << '\n';
        std::cout << "TREMBLE " << (sautMin != sautMax ? "OUI" : "NON") << '\n';
    }
    return 0;
}
