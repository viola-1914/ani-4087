// Chapitre 11, exercice 6 : le champ isole.
// Relit un programme ligne par ligne et trouve les lignes qui savent quel
// backend tourne alors qu'elles ne le devraient pas : le choix se fait une
// fois a l'INIT, la FABRIQUE l'applique, la BOUCLE et la SCENE l'ignorent.
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static bool EstMotChar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

// Vrai si le nom est ecrit tel quel, et entier (bornes sans lettre, chiffre ni _).
static bool Contient(const std::string &texte, const std::string &nom) {
    if (nom.empty()) {
        return false;
    }
    size_t pos = texte.find(nom);
    while (pos != std::string::npos) {
        const bool avantOk = (pos == 0) || !EstMotChar(texte[pos - 1]);
        const size_t fin = pos + nom.size();
        const bool apresOk = (fin >= texte.size()) || !EstMotChar(texte[fin]);
        if (avantOk && apresOk) {
            return true;
        }
        pos = texte.find(nom, pos + 1);
    }
    return false;
}

static bool ContientUn(const std::string &texte, const std::vector<std::string> &noms) {
    for (const std::string &nom : noms) {
        if (Contient(texte, nom)) {
            return true;
        }
    }
    return false;
}

static bool EstCommentaire(const std::string &texte) {
    size_t i = 0;
    while (i < texte.size() && (texte[i] == ' ' || texte[i] == '\t')) {
        ++i;
    }
    return texte.compare(i, 2, "//") == 0;
}

// Lit une ligne d'en-tete "X k nom1 ... nomk".
static std::vector<std::string> LireNoms(const std::string &ligne) {
    std::istringstream flux(ligne);
    std::string lettre;
    long long k = 0;
    std::vector<std::string> noms;
    flux >> lettre >> k;
    for (long long i = 0; i < k; ++i) {
        std::string nom;
        if (!(flux >> nom)) {
            break;
        }
        noms.push_back(nom);
    }
    return noms;
}

static void OterRetourChariot(std::string &ligne) {
    if (!ligne.empty() && ligne.back() == '\r') {
        ligne.pop_back();
    }
}

int main() {
    std::string ligne;
    std::getline(std::cin, ligne);
    OterRetourChariot(ligne);
    const std::vector<std::string> nomsC = LireNoms(ligne);
    std::getline(std::cin, ligne);
    OterRetourChariot(ligne);
    const std::vector<std::string> nomsD = LireNoms(ligne);
    std::getline(std::cin, ligne);
    OterRetourChariot(ligne);
    long long n = 0;
    try {
        n = std::stoll(ligne);
    } catch (...) {
        n = 0;
    }

    long long decisions = 0, fuites = 0, definis = 0;
    for (long long l = 1; l <= n; ++l) {
        if (!std::getline(std::cin, ligne)) {
            break;
        }
        OterRetourChariot(ligne);
        const size_t espace = ligne.find(' ');
        const std::string zone = ligne.substr(0, espace);
        const std::string texte = (espace == std::string::npos) ? "" : ligne.substr(espace + 1);

        if (EstCommentaire(texte)) {
            continue;  // un commentaire ne compte jamais
        }

        if (ContientUn(texte, nomsC)) {
            if (zone == "INIT") {
                ++decisions;
            } else if (zone == "BOUCLE" || zone == "SCENE") {
                std::cout << "FUITE " << l << ' ' << zone << '\n';
                ++fuites;
            }
        }
        if (zone != "FABRIQUE" && ContientUn(texte, nomsD)) {
            std::cout << "DEFINI " << l << ' ' << zone << '\n';
            ++definis;
        }
    }

    std::cout << "DECISIONS " << decisions << '\n';
    std::cout << "FUITES " << fuites << '\n';
    std::cout << "DEFINIS " << definis << '\n';
    std::cout << "ABSTRACTION " << (fuites == 0 && definis == 0 ? "TENUE" : "FUIT") << '\n';
    return 0;
}
