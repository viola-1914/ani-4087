#include <NKImage/Core/NkImage.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>

using nkentseu::NkImage;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    const char* source = argc > 1 ? argv[1] : "photo.jpeg";
    NkImage photo;
    if (!photo.Load(source)) {
        std::cerr << "Impossible de charger : " << source << '\n';
        return 1;
    }

    const auto largeur = photo.Width();
    const auto hauteur = photo.Height();
    std::cout << "Photo originale : " << largeur << " x " << hauteur << '\n';

    // Un sous-dossier separe les images produites de la photo originale.
    std::error_code erreur;
    fs::create_directories("images_exportees", erreur);
    if (erreur) {
        std::cerr << "Creation du dossier impossible : " << erreur.message() << '\n';
        return 1;
    }

    const char* formats[] = {"png", "jpeg", "bmp", "tga", "qoi"};
    std::ostringstream resultats;
    bool taillesDisponibles = true;

    for (int i = 0; i < 5; ++i) {
        const std::string chemin = std::string("images_exportees/mur.") + formats[i];
        // Eviter de mesurer un ancien fichier si une nouvelle ecriture echoue.
        erreur.clear();
        fs::remove(chemin, erreur);
        if (erreur) {
            std::cerr << "Ancien fichier impossible a retirer : " << chemin << '\n';
            return 1;
        }

        bool ecrit = false;
        switch (i) {
        case 0: ecrit = photo.SavePNG(chemin.c_str()); break;
        case 1: ecrit = photo.SaveJPEG(chemin.c_str(), 90); break;
        case 2: ecrit = photo.SaveBMP(chemin.c_str()); break;
        case 3: ecrit = photo.SaveTGA(chemin.c_str()); break;
        case 4: ecrit = photo.SaveQOI(chemin.c_str()); break;
        }
        if (!ecrit) {
            std::cerr << formats[i] << " : ecriture signalee en echec.\n";
        }

        // La relecture est tentee meme si l'ecriture a signale un echec.
        NkImage relue;
        const bool lisible = relue.Load(chemin.c_str());
        erreur.clear();
        const auto octets = fs::file_size(chemin, erreur);
        if (erreur) {
            std::cerr << formats[i] << " : taille non mesurable, "
                      << erreur.message() << '\n';
            taillesDisponibles = false;
            continue;
        }

        resultats << formats[i] << " : " << octets << " octets, ";
        if (!lisible) {
            resultats << "illisible\n";
            std::cerr << formats[i] << " : relecture impossible.\n";
        } else {
            resultats << relue.Width() << " x " << relue.Height() << '\n';
            const bool identiques = relue.Width() == largeur && relue.Height() == hauteur;
            std::cout << formats[i] << " : dimensions "
                      << (identiques ? "identiques a la photo" : "DIFFERENTES de la photo")
                      << '\n';
        }
    }

    std::cout << '\n' << resultats.str();
    if (!taillesDisponibles) {
        std::cerr << "Mesures incompletes : formats.txt n'est pas mis a jour.\n"
                  << "Ne pas deposer un ancien formats.txt ; corriger l'erreur et relancer.\n";
        return 2;
    }

    std::ofstream sortie("formats.txt", std::ios::trunc);
    sortie << resultats.str();
    sortie.close();
    if (!sortie) {
        std::cerr << "Impossible d'ecrire formats.txt.\n";
        return 1;
    }
    std::cout << "formats.txt a ete genere avec les mesures de cette execution.\n";
    return 0;
}
