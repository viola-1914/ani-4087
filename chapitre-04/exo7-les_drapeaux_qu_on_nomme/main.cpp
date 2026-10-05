//
// main.cpp
// =============================================================================
// Description :
//   Combine des drapeaux de sous-systemes par un OU binaire, signale les noms
//   inconnus et les dependances manquantes, puis compte les drapeaux simples
//   allumes et eteints.
//
// Caracteristiques :
//   - Combinaison par OU binaire, jamais par addition : un nom repete ou deja
//     contenu dans un compose ne compte qu'une fois.
//   - La valeur est rangee dans un entier non signe de 32 bits :
//     ALL (4294967295) ne tient pas dans un int signe.
//   - Sans aucun nom (N = 0), la configuration garde sa valeur par defaut, ALL.
// =============================================================================

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

static const std::uint32_t RENDER2D = 1u;
static const std::uint32_t RENDER3D = 2u;
static const std::uint32_t TEXT = 4u;
static const std::uint32_t UI = 8u;
static const std::uint32_t SHADOW = 16u;
static const std::uint32_t POST_PROCESS = 32u;
static const std::uint32_t VFX = 64u;
static const std::uint32_t ANIMATION = 128u;
static const std::uint32_t OVERLAY = 256u;
static const std::uint32_t SIMULATION = 512u;
static const std::uint32_t OFFSCREEN = 1024u;
static const std::uint32_t RAYTRACING = 2048u;
static const std::uint32_t GPU_CULLING = 4096u;
static const std::uint32_t ALL = 4294967295u;

// Les treize drapeaux simples, dans l'ordre de leur valeur.
static const std::uint32_t DRAPEAUX_SIMPLES[13] = {
    RENDER2D, RENDER3D, TEXT, UI, SHADOW, POST_PROCESS, VFX,
    ANIMATION, OVERLAY, SIMULATION, OFFSCREEN, RAYTRACING, GPU_CULLING,
};

// Cherche la valeur d'un nom. Rend false si le nom est inconnu.
// La casse compte : "render3d" n'est pas "RENDER3D".
static bool ValeurDuNom(const std::string &nom, std::uint32_t &valeur) {
    struct Entree {
        const char *nom;
        std::uint32_t valeur;
    };
    static const Entree table[] = {
        {"RENDER2D", RENDER2D},
        {"RENDER3D", RENDER3D},
        {"TEXT", TEXT},
        {"UI", UI},
        {"SHADOW", SHADOW},
        {"POST_PROCESS", POST_PROCESS},
        {"VFX", VFX},
        {"ANIMATION", ANIMATION},
        {"OVERLAY", OVERLAY},
        {"SIMULATION", SIMULATION},
        {"OFFSCREEN", OFFSCREEN},
        {"RAYTRACING", RAYTRACING},
        {"GPU_CULLING", GPU_CULLING},
        {"NONE", 0u},
        {"2D_ESSENTIALS", RENDER2D | TEXT},
        {"3D_BASE", RENDER3D | SHADOW | POST_PROCESS},
        {"DEBUG", OVERLAY | SIMULATION},
        {"ALL", ALL},
    };

    for (const Entree &entree : table) {
        if (nom == entree.nom) {
            valeur = entree.valeur;
            return true;
        }
    }
    return false;
}

static bool Allume(std::uint32_t valeur, std::uint32_t drapeau) {
    return (valeur & drapeau) != 0u;
}

// Affiche une ligne MANQUE si le drapeau est allume sans sa dependance.
// Un drapeau eteint ne reclame rien.
static void VerifierDependance(std::uint32_t valeur,
                               std::uint32_t drapeau,
                               const char *nomDrapeau,
                               std::uint32_t dependance,
                               const char *nomDependance) {
    if (Allume(valeur, drapeau) && !Allume(valeur, dependance)) {
        std::cout << "MANQUE " << nomDrapeau << ' ' << nomDependance << '\n';
    }
}

int main() {
    long long nombreNoms = 0;
    if (!(std::cin >> nombreNoms)) {
        nombreNoms = 0;
    }

    std::uint32_t valeur = 0u;
    for (long long i = 0; i < nombreNoms; ++i) {
        std::string nom;

        // Une entree tronquee arrete proprement la lecture.
        if (!(std::cin >> nom)) {
            break;
        }

        std::uint32_t valeurNom = 0u;
        if (ValeurDuNom(nom, valeurNom)) {
            valeur = valeur | valeurNom;
        } else {
            // Une faute de frappe ne change pas la valeur : on la signale.
            std::cout << "INCONNU " << nom << '\n' << std::flush;
        }
    }

    // Rien de nomme : la configuration garde sa valeur par defaut.
    if (nombreNoms == 0) {
        valeur = ALL;
    }

    std::cout << "VALEUR " << valeur << '\n';
    std::cout << "HEXA 0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << valeur << std::dec << '\n';

    // Ordre impose : TEXT, UI, SHADOW, OVERLAY, chacun avec ses dependances.
    VerifierDependance(valeur, TEXT, "TEXT", RENDER2D, "RENDER2D");
    VerifierDependance(valeur, UI, "UI", RENDER2D, "RENDER2D");
    VerifierDependance(valeur, UI, "UI", TEXT, "TEXT");
    VerifierDependance(valeur, SHADOW, "SHADOW", RENDER3D, "RENDER3D");
    VerifierDependance(valeur, OVERLAY, "OVERLAY", RENDER2D, "RENDER2D");
    VerifierDependance(valeur, OVERLAY, "OVERLAY", TEXT, "TEXT");

    int allumes = 0;
    for (const std::uint32_t drapeau : DRAPEAUX_SIMPLES) {
        if (Allume(valeur, drapeau)) {
            ++allumes;
        }
    }

    std::cout << "ALLUMES " << allumes << '\n';
    std::cout << "ETEINTS " << 13 - allumes << '\n';
    return 0;
}
