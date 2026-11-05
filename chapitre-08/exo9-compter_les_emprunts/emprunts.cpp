// Chapitre 8, exercice 9 : compter les emprunts.
// Chaque trame commence par un emprunt (AcquireImage). Trois chemins :
//   normal : dessine puis rend l'image            -> t OK
//   sortie : sort aussitot apres l'emprunt, sans rendre -> t FUITE
//   double : tente un second emprunt (refuse), puis rend -> t DOUBLE
// Un emprunt refuse en debut de trame arrete la trame -> t BLOQUEE.
#include <iostream>
#include <string>

// La chaine, avec la meme discipline que NkXrSwapchain.
struct Chaine {
    int n = 1;
    int prochaine = 0;
    bool empruntee = false;

    bool Emprunter() {
        if (empruntee) {
            return false;
        }
        empruntee = true;
        return true;
    }
    bool Rendre() {
        if (!empruntee) {
            return false;
        }
        prochaine = (prochaine + 1) % n;
        empruntee = false;
        return true;
    }
};

int main() {
    int n = 1;
    long long nbTrames = 0;
    if (!(std::cin >> n >> nbTrames)) {
        return 0;
    }
    Chaine chaine;
    chaine.n = (n < 1) ? 1 : n;

    long long reussies = 0;
    long long refusees = 0;
    long long liberations = 0;
    long long premiereFuite = 0;

    for (long long t = 1; t <= nbTrames; ++t) {
        std::string chemin;
        if (!(std::cin >> chemin)) {
            break;
        }

        if (!chaine.Emprunter()) {
            ++refusees;
            std::cout << t << " BLOQUEE\n";   // rien emprunte, rien a rendre
            continue;
        }
        ++reussies;

        if (chemin == "sortie") {
            // Sortie anticipee : l'image n'est jamais rendue.
            if (premiereFuite == 0) {
                premiereFuite = t;
            }
            std::cout << t << " FUITE\n";
            continue;
        }

        if (chemin == "double") {
            if (chaine.Emprunter()) {
                ++reussies;                  // n'arrive pas : une image est deja empruntee
            } else {
                ++refusees;
            }
            if (chaine.Rendre()) {
                ++liberations;
            }
            std::cout << t << " DOUBLE\n";
            continue;
        }

        // normal : dessiner, puis rendre.
        if (chaine.Rendre()) {
            ++liberations;
        }
        std::cout << t << " OK\n";
    }

    std::cout << "REUSSIES " << reussies << '\n';
    std::cout << "REFUSEES " << refusees << '\n';
    std::cout << "LIBERATIONS " << liberations << '\n';
    std::cout << "ECART " << (reussies - liberations) << '\n';
    if (premiereFuite == 0) {
        std::cout << "PREMIERE FUITE AUCUNE\n";
    } else {
        std::cout << "PREMIERE FUITE " << premiereFuite << '\n';
    }
    return 0;
}
