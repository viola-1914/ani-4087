//
// budget.cpp
// =============================================================================
// Description :
//   Lit deux configurations de sous-systemes (drapeaux + dix temps de
//   demarrage en millisecondes) et resume chaque serie par sa valeur de
//   drapeaux, sa mediane et sa moyenne, puis affiche les deux ecarts.
//
// Caracteristiques :
//   - Les drapeaux se combinent par OU binaire, jamais par addition.
//   - La valeur est rangee dans un entier non signe de 32 bits :
//     ALL (4294967295) ne tient pas dans un int signe.
//   - La mediane TRIE la serie avant de choisir : le premier lancement lent,
//     qui charge tout depuis le disque, se retrouve en bout de tri et ne
//     pese pas sur elle, alors qu'il gonfle la moyenne.
// =============================================================================

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

static const int NOMBRE_MESURES = 10;

// Valeur d'un drapeau. Un nom inconnu vaut 0 et ne change donc pas le OU.
static std::uint32_t ValeurDrapeau(const std::string &nom) {
    if (nom == "RENDER2D") {
        return 1u;
    }
    if (nom == "RENDER3D") {
        return 2u;
    }
    if (nom == "TEXT") {
        return 4u;
    }
    if (nom == "UI") {
        return 8u;
    }
    if (nom == "SHADOW") {
        return 16u;
    }
    if (nom == "POST_PROCESS") {
        return 32u;
    }
    if (nom == "ALL") {
        return 4294967295u;
    }
    return 0u;
}

// Mediane de dix valeurs : moyenne des cinquieme et sixieme une fois triees,
// en division entiere. La copie est triee, la serie d'origine reste intacte.
static long long Mediane(std::vector<long long> temps) {
    std::sort(temps.begin(), temps.end());
    return (temps[4] + temps[5]) / 2;
}

static long long Moyenne(const std::vector<long long> &temps) {
    long long somme = 0;
    for (const long long t : temps) {
        somme += t;
    }
    return somme / NOMBRE_MESURES;
}

// Lit une configuration et affiche ses trois lignes.
// Rend false si l'entree est tronquee.
static bool TraiterConfiguration(long long &mediane, long long &moyenne) {
    std::string nom;
    int nombreDrapeaux = 0;
    if (!(std::cin >> nom >> nombreDrapeaux)) {
        return false;
    }

    std::uint32_t valeur = 0u;
    for (int i = 0; i < nombreDrapeaux; ++i) {
        std::string drapeau;
        if (!(std::cin >> drapeau)) {
            return false;
        }
        valeur = valeur | ValeurDrapeau(drapeau);
    }

    std::vector<long long> temps;
    for (int i = 0; i < NOMBRE_MESURES; ++i) {
        long long t = 0;
        if (!(std::cin >> t)) {
            return false;
        }
        temps.push_back(t);
    }

    mediane = Mediane(temps);
    moyenne = Moyenne(temps);

    std::cout << nom << " VALEUR " << valeur << '\n';
    std::cout << nom << " MEDIANE " << mediane << '\n';
    std::cout << nom << " MOYENNE " << moyenne << '\n';
    return true;
}

int main() {
    long long medianePremiere = 0;
    long long moyennePremiere = 0;
    long long medianeSeconde = 0;
    long long moyenneSeconde = 0;

    if (!TraiterConfiguration(medianePremiere, moyennePremiere)) {
        return 0;
    }
    if (!TraiterConfiguration(medianeSeconde, moyenneSeconde)) {
        return 0;
    }

    // Ecarts : la premiere configuration moins la seconde.
    std::cout << "ECART MEDIANES " << medianePremiere - medianeSeconde << '\n';
    std::cout << "ECART MOYENNES " << moyennePremiere - moyenneSeconde << '\n';
    return 0;
}
