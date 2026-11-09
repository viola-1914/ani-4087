// Chapitre 9, exercice 4 : synchroniser une fois par image.
// Suit les appels a SyncActions d'une boucle de rendu : changed se calcule
// a la synchronisation. Sans synchronisation, un front se repete ; avec
// deux, le second efface le premier. Sans le focus, l'appel est refuse.
#include <iostream>

int main() {
    long long nbImages = 0;
    if (!(std::cin >> nbImages)) {
        nbImages = 0;
    }

    int current = 0, changed = 0, active = 0;
    int pPrecedent = 0;
    long long appuisReels = 0, saisies = 0, syncRefusees = 0;

    for (long long t = 1; t <= nbImages; ++t) {
        int p = 0, s = 0, f = 0;
        if (!(std::cin >> p >> s >> f)) {
            break;
        }

        for (int appel = 0; appel < s; ++appel) {
            if (f == 0) {
                ++syncRefusees;          // refuse : les trois champs restent tels quels
                continue;
            }
            changed = (p != current) ? 1 : 0;
            current = p;
            active = 1;
        }

        const bool saisit = (current == 1 && changed == 1);
        std::cout << t << ' ' << current << ' ' << changed << ' ' << active << ' '
                  << (saisit ? "SAISIT" : "-") << '\n';
        if (saisit) {
            ++saisies;
        }
        if (p == 1 && pPrecedent == 0) {
            ++appuisReels;
        }
        pPrecedent = p;
    }

    std::cout << "APPUIS REELS " << appuisReels << '\n';
    std::cout << "SAISIES " << saisies << '\n';
    std::cout << "SYNC REFUSEES " << syncRefusees << '\n';
    return 0;
}
