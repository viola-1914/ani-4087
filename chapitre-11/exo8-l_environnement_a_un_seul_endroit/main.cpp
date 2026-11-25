// Chapitre 11, exercice 8 : l'environnement a un seul endroit.
// Applique des variables d'environnement par-dessus les reglages poses par
// le code, avec les regles exactes du module (une seule fonction d'entree).
// Attention aux surprises : "False" allume un interrupteur.
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

static void OterRetourChariot(std::string &ligne) {
    if (!ligne.empty() && ligne.back() == '\r') {
        ligne.pop_back();
    }
}

// Lit un nombre entier a partir de pos : espaces, signe facultatif, chiffres.
// Rend le nombre de chiffres lus ; pos avance jusqu'au premier autre signe.
static int LireNombre(const std::string &texte, size_t &pos, int64_t &valeur) {
    while (pos < texte.size() && texte[pos] == ' ') {
        ++pos;
    }
    bool negatif = false;
    if (pos < texte.size() && (texte[pos] == '+' || texte[pos] == '-')) {
        negatif = (texte[pos] == '-');
        ++pos;
    }
    int chiffres = 0;
    int64_t v = 0;
    while (pos < texte.size() && texte[pos] >= '0' && texte[pos] <= '9') {
        if (v < 100000000000000000LL) {
            v = v * 10 + (texte[pos] - '0');
        }
        ++pos;
        ++chiffres;
    }
    valeur = negatif ? -v : v;
    return chiffres;
}

static int64_t Nombre(const std::string &texte) {
    size_t pos = 0;
    int64_t v = 0;
    LireNombre(texte, pos, v);
    return v;   // sans chiffre : 0
}

static bool Interrupteur(const std::string &valeur) {
    return !(valeur[0] == '0' || valeur == "false");
}

static const char *OuiNon(bool b) {
    return b ? "OUI" : "NON";
}

int main() {
    std::string ligne;

    // Les reglages poses par le code.
    std::getline(std::cin, ligne);
    OterRetourChariot(ligne);
    std::istringstream premiere(ligne);
    std::string backend, demiTxt, profTxt, listeTxt;
    int64_t hz = 0, latence = 0;
    premiere >> backend >> hz >> demiTxt >> profTxt >> listeTxt >> latence;
    bool demi = (demiTxt == "OUI"), profondeur = (profTxt == "OUI"), liste = (listeTxt == "OUI");

    std::string loader;
    std::getline(std::cin, loader);
    OterRetourChariot(loader);

    std::getline(std::cin, ligne);
    OterRetourChariot(ligne);
    std::istringstream poseFlux(ligne);
    std::string poseMot;
    poseFlux >> poseMot;
    bool poseFixe = (poseMot == "FIXE");
    int64_t pose[5] = {0, 0, 0, 0, 0};
    if (poseFixe) {
        for (int64_t &p : pose) {
            poseFlux >> p;
        }
    }

    std::getline(std::cin, ligne);
    OterRetourChariot(ligne);
    long long m = 0;
    try {
        m = std::stoll(ligne);
    } catch (...) {
        m = 0;
    }

    long long appliquees = 0, ignorees = 0;
    for (long long i = 0; i < m; ++i) {
        if (!std::getline(std::cin, ligne)) {
            break;
        }
        OterRetourChariot(ligne);
        const size_t egal = ligne.find('=');
        const std::string nom = ligne.substr(0, egal);
        const std::string valeur = (egal == std::string::npos) ? "" : ligne.substr(egal + 1);

        bool applique = false;
        if (!valeur.empty()) {
            if (nom == "NK_XR_BACKEND") {
                if (valeur == "openxr") {
                    backend = "OPENXR";
                    applique = true;
                }
            } else if (nom == "NK_XR_HZ") {
                hz = Nombre(valeur);
                applique = true;
            } else if (nom == "NK_XR_SIM_LATENCY_MS") {
                latence = Nombre(valeur);
                applique = true;
            } else if (nom == "NK_XR_HALF_RATE") {
                demi = Interrupteur(valeur);
                applique = true;
            } else if (nom == "NK_XR_DEPTH_LAYER") {
                profondeur = Interrupteur(valeur);
                applique = true;
            } else if (nom == "NK_XR_LIST_EXT") {
                liste = Interrupteur(valeur);
                applique = true;
            } else if (nom == "NK_XR_OPENXR_LOADER") {
                loader = valeur;
                applique = true;
            } else if (nom == "NK_XR_SIM_POSE") {
                int64_t lus[5] = {0, 0, 0, 0, 0};
                int compte = 0;
                size_t pos = 0;
                while (compte < 5 && pos < valeur.size()) {
                    int64_t v = 0;
                    if (LireNombre(valeur, pos, v) == 0) {
                        break;
                    }
                    lus[compte++] = v;
                    while (pos < valeur.size() && (valeur[pos] == ',' || valeur[pos] == ' ')) {
                        ++pos;
                    }
                }
                if (compte == 5) {
                    poseFixe = true;
                    for (int k = 0; k < 5; ++k) {
                        pose[k] = lus[k];
                    }
                    applique = true;
                }
            }
        }
        if (applique) {
            ++appliquees;
        } else {
            ++ignorees;
        }
    }

    std::cout << "BACKEND " << backend << '\n';
    std::cout << "HZ " << hz << '\n';
    std::cout << "DEMI " << OuiNon(demi) << '\n';
    std::cout << "PROFONDEUR " << OuiNon(profondeur) << '\n';
    std::cout << "LISTE " << OuiNon(liste) << '\n';
    std::cout << "LOADER " << loader << '\n';
    std::cout << "LATENCE " << latence << '\n';
    if (poseFixe) {
        std::cout << "POSE FIXE " << pose[0] << ' ' << pose[1] << ' ' << pose[2] << ' ' << pose[3] << ' '
                  << pose[4] << '\n';
    } else {
        std::cout << "POSE LIBRE\n";
    }
    std::cout << "APPLIQUEES " << appliquees << '\n';
    std::cout << "IGNOREES " << ignorees << '\n';
    return 0;
}
