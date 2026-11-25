// Chapitre 11, exercice 9 : le mode sans tete.
// Compare deux executions du meme test sans fenetre. La pose ne bouge pas,
// mais l'instant predit vient de l'horloge : brut, il differe toujours ;
// meme compte depuis la premiere image, il varie. Seul un instant scripte
// (t x periode) est le meme dans les deux executions. Tout est sur 64 bits.
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

int main() {
    int64_t hz = 0;
    if (!(std::cin >> hz)) {
        return 0;
    }
    const int64_t periode = 1000000000LL / (hz <= 1 ? 72 : hz);

    std::string mot;
    std::cin >> mot;
    int64_t pose[5] = {0, 0, 0, 1700, 0};   // LIBRE : la pose de depart
    if (mot == "FIXE") {
        for (int64_t &p : pose) {
            std::cin >> p;
        }
    }

    long long t = 0;
    std::cin >> t;
    if (t < 0) {
        t = 0;
    }
    std::vector<int64_t> a(static_cast<size_t>(t)), b(static_cast<size_t>(t));
    for (int64_t &v : a) {
        std::cin >> v;
    }
    for (int64_t &v : b) {
        std::cin >> v;
    }

    std::cout << "PERIODE " << periode << '\n';
    std::cout << "POSE " << pose[0] << ' ' << pose[1] << ' ' << pose[2] << ' ' << pose[3] << ' '
              << pose[4] << '\n';

    long long premierBrut = 0, premierRelatif = 0;
    for (long long i = 0; i < t; ++i) {
        const size_t k = static_cast<size_t>(i);
        const int64_t preditA = a[k] + periode;
        const int64_t preditB = b[k] + periode;
        const int64_t brut = preditB - preditA;
        const int64_t relatif = (preditB - (b[0] + periode)) - (preditA - (a[0] + periode));
        const int64_t scripte = (i + 1) * periode;

        std::cout << i + 1 << " BRUT " << brut << " RELATIF " << relatif << " SCRIPTE " << scripte << '\n';
        if (brut != 0 && premierBrut == 0) {
            premierBrut = i + 1;
        }
        if (relatif != 0 && premierRelatif == 0) {
            premierRelatif = i + 1;
        }
    }

    if (premierBrut == 0) {
        std::cout << "BRUT IDENTIQUE\n";
    } else {
        std::cout << "BRUT DIFFERENT " << premierBrut << '\n';
    }
    if (premierRelatif == 0) {
        std::cout << "RELATIF IDENTIQUE\n";
    } else {
        std::cout << "RELATIF DIFFERENT " << premierRelatif << '\n';
    }
    return 0;
}
