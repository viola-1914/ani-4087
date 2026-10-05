//
// main.cpp
// =============================================================================
// Description :
//   Rejoue la detection automatique de l'interface graphique, machine par
//   machine, puis affiche le bilan IGNOREES / LOGICIEL / DIFFERENTES.
//
// Caracteristiques :
//   - L'ordre d'essai est celui de la PLATEFORME, jamais celui de la liste lue.
//   - SOFTWARE est toujours disponible : il sert de repli meme s'il n'est pas liste.
// =============================================================================

#include <iostream>
#include <set>
#include <string>
#include <vector>

// Ordre d'essai d'une plateforme. Une plateforme inconnue n'est pas une erreur :
// elle suit l'ordre par defaut.
static std::vector<std::string> OrdrePlateforme(const std::string &plateforme) {
    if (plateforme == "WINDOWS") {
        return {"VULKAN", "DX12", "DX11", "OPENGL", "SOFTWARE"};
    }
    if (plateforme == "MACOS") {
        return {"METAL", "OPENGL", "SOFTWARE"};
    }
    if (plateforme == "IOS") {
        return {"METAL", "SOFTWARE"};
    }
    if (plateforme == "ANDROID") {
        return {"VULKAN", "OPENGL", "SOFTWARE"};
    }
    return {"VULKAN", "OPENGL", "SOFTWARE"};
}

static std::string NomLisible(const std::string &api) {
    if (api == "VULKAN") {
        return "Vulkan";
    }
    if (api == "DX12") {
        return "DirectX 12";
    }
    if (api == "DX11") {
        return "DirectX 11";
    }
    if (api == "OPENGL") {
        return "OpenGL";
    }
    if (api == "METAL") {
        return "Metal";
    }
    return "Software";
}

static bool Contient(const std::vector<std::string> &liste, const std::string &valeur) {
    for (const std::string &element : liste) {
        if (element == valeur) {
            return true;
        }
    }
    return false;
}

int main() {
    int nombreMachines = 0;
    if (!(std::cin >> nombreMachines)) {
        nombreMachines = 0;
    }

    int ignorees = 0;
    int logiciel = 0;
    std::set<std::string> nomsAffiches;

    for (int i = 0; i < nombreMachines; ++i) {
        std::string nom;
        std::string plateforme;
        int k = 0;
        // Une entree tronquee arrete proprement la lecture au lieu de
        // continuer avec des valeurs vides.
        if (!(std::cin >> nom >> plateforme >> k)) {
            break;
        }

        std::vector<std::string> disponibles;
        for (int j = 0; j < k; ++j) {
            std::string api;
            if (!(std::cin >> api)) {
                break;
            }
            disponibles.push_back(api);
        }

        const std::vector<std::string> ordre = OrdrePlateforme(plateforme);

        // Une interface qui marche mais que la plateforme n'essaie jamais
        // (OPENGLES, WEBGL, DX11 hors Windows...) est comptee comme ignoree.
        for (const std::string &api : disponibles) {
            if (!Contient(ordre, api)) {
                ++ignorees;
            }
        }

        // Repli par defaut : le rendu logiciel marche partout.
        std::string choisie = "SOFTWARE";
        for (const std::string &candidate : ordre) {
            if (Contient(disponibles, candidate)) {
                choisie = candidate;
                break;
            }
        }

        if (choisie == "SOFTWARE") {
            ++logiciel;
        }

        const std::string lisible = NomLisible(choisie);
        nomsAffiches.insert(lisible);
        // Envoi immediat de chaque ligne : si le programme est interrompu,
        // la sortie montre jusqu'ou il est alle.
        std::cout << nom << ' ' << lisible << '\n' << std::flush;
    }

    std::cout << "IGNOREES " << ignorees << '\n';
    std::cout << "LOGICIEL " << logiciel << '\n';
    std::cout << "DIFFERENTES " << nomsAffiches.size() << '\n';
    return 0;
}