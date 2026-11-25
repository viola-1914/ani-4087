// Chapitre 11, exercice 4 : une question, un itineraire.
// Suit le chemin registre -> manifeste -> bibliotheque du runtime, comme le
// backend OpenXR sans loader : un simple scan de texte, sans lecteur JSON.
#include <iostream>
#include <string>

static void OterRetourChariot(std::string &ligne) {
    if (!ligne.empty() && ligne.back() == '\r') {
        ligne.pop_back();
    }
}

static long long LireEntier(const std::string &ligne) {
    try {
        return std::stoll(ligne);
    } catch (...) {
        return 0;
    }
}

int main() {
    std::string ligne;
    long long k = 0;
    if (std::getline(std::cin, ligne)) {
        OterRetourChariot(ligne);
        k = LireEntier(ligne);
    }

    for (long long m = 1; m <= k; ++m) {
        std::string registre;
        if (!std::getline(std::cin, registre)) {
            break;
        }
        OterRetourChariot(registre);
        std::string ligneL;
        std::getline(std::cin, ligneL);
        OterRetourChariot(ligneL);
        const long long l = LireEntier(ligneL);

        // Le texte du manifeste : chaque ligne suivie d'un saut de ligne.
        std::string texte;
        for (long long i = 0; i < l; ++i) {
            std::string ligneManifeste;
            std::getline(std::cin, ligneManifeste);
            OterRetourChariot(ligneManifeste);
            texte += ligneManifeste;
            texte += '\n';
        }

        if (registre == "-") {
            std::cout << m << " REGISTRE AUCUN\n";
            continue;
        }
        std::cout << m << " MANIFESTE " << registre << '\n';
        if (l < 0) {
            std::cout << m << " ECHEC ILLISIBLE\n";
            continue;
        }

        const std::string cle = "\"library_path\"";
        const size_t posCle = texte.find(cle);
        if (posCle == std::string::npos) {
            std::cout << m << " ECHEC CLE\n";
            continue;
        }
        const size_t posDeuxPoints = texte.find(':', posCle + cle.size());
        if (posDeuxPoints == std::string::npos) {
            std::cout << m << " ECHEC DEUX_POINTS\n";
            continue;
        }
        const size_t ouvrant = texte.find('"', posDeuxPoints);
        const size_t fermant = (ouvrant == std::string::npos) ? std::string::npos
                                                              : texte.find('"', ouvrant + 1);
        if (fermant == std::string::npos) {
            std::cout << m << " ECHEC GUILLEMET\n";
            continue;
        }

        // Recopier en sautant un antislash suivi d'un autre antislash.
        const std::string brut = texte.substr(ouvrant + 1, fermant - ouvrant - 1);
        std::string valeur;
        for (size_t i = 0; i < brut.size(); ++i) {
            if (brut[i] == '\\' && i + 1 < brut.size() && brut[i + 1] == '\\') {
                continue;
            }
            valeur += brut[i];
        }
        if (valeur.empty()) {
            std::cout << m << " ECHEC VIDE\n";
            continue;
        }
        std::cout << m << " VALEUR " << valeur << '\n';

        const bool absolu = (valeur.size() >= 2 && valeur[1] == ':') || valeur[0] == '\\' ||
                            valeur[0] == '/';
        std::string charge;
        if (absolu) {
            std::cout << m << " ABSOLU\n";
            charge = valeur;
        } else {
            std::cout << m << " RELATIF\n";
            const size_t sep = registre.find_last_of("\\/");
            const std::string dossier = (sep == std::string::npos) ? "" : registre.substr(0, sep + 1);
            charge = dossier + valeur;
        }
        std::cout << m << " CHARGE " << charge << '\n';
    }
    return 0;
}
