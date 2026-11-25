// Chapitre 11, exercice 1 : la premiere passe.
// Lit un fichier source ligne par ligne, s'arrete a la fin de son en-tete
// (le bloc de lignes // du haut), et dit ce que cet en-tete annonce :
// sa taille, et les variables d'environnement NK_...= qu'il accepte.
#include <iostream>
#include <string>
#include <vector>

static bool EstMajChiffreSouligne(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

static bool EstMotChar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

// Vrai si la ligne commence par //, une fois otees espaces et tabulations.
static bool EstCommentaire(const std::string &ligne) {
    size_t i = 0;
    while (i < ligne.size() && (ligne[i] == ' ' || ligne[i] == '\t')) {
        ++i;
    }
    return ligne.compare(i, 2, "//") == 0;
}

// Ajoute les variables NK_...= annoncees dans la ligne, sans doublon.
static void ChercherVariables(const std::string &ligne, std::vector<std::string> &variables) {
    size_t pos = ligne.find("NK_");
    while (pos != std::string::npos) {
        const bool bordAvant = (pos == 0) || !EstMotChar(ligne[pos - 1]);
        size_t fin = pos + 3;
        while (fin < ligne.size() && EstMajChiffreSouligne(ligne[fin])) {
            ++fin;
        }
        const bool auMoinsUn = fin > pos + 3;
        const bool suiviEgal = fin < ligne.size() && ligne[fin] == '=';
        if (bordAvant && auMoinsUn && suiviEgal) {
            const std::string nom = ligne.substr(pos, fin - pos);
            bool deja = false;
            for (const std::string &v : variables) {
                if (v == nom) {
                    deja = true;
                    break;
                }
            }
            if (!deja) {
                variables.push_back(nom);
            }
        }
        pos = ligne.find("NK_", pos + 1);
    }
}

int main() {
    long long n = 0;
    std::string premiere;
    if (std::getline(std::cin, premiere)) {
        try {
            n = std::stoll(premiere);
        } catch (...) {
            n = 0;
        }
    }
    if (n < 0) {
        n = 0;
    }

    long long entete = 0;
    bool dansEntete = true;
    std::vector<std::string> variables;

    for (long long i = 0; i < n; ++i) {
        std::string ligne;
        if (!std::getline(std::cin, ligne)) {
            break;
        }
        if (!ligne.empty() && ligne.back() == '\r') {
            ligne.pop_back();
        }
        if (!dansEntete) {
            continue;   // tout ce qui suit l'en-tete est ignore
        }
        if (!EstCommentaire(ligne)) {
            dansEntete = false;
            continue;
        }
        ++entete;
        ChercherVariables(ligne, variables);
    }

    std::cout << "ENTETE " << entete << '\n';
    std::cout << "IGNOREES " << n - entete << '\n';
    if (variables.empty()) {
        std::cout << "VARIABLE AUCUNE\n";
    }
    for (const std::string &v : variables) {
        std::cout << "VARIABLE " << v << '\n';
    }
    std::cout << "PART " << (n == 0 ? 0 : 100 * entete / n) << '\n';
    return 0;
}
