// Chapitre 8, exercice 10 : la chaine a trois images.
// Pour chaque scenario n L T : la trame u (de 1 a T) peint l'image
// (u - 1) % n puis la soumet ; le compositeur lit cette image pendant
// les L trames suivantes. La trame u est en conflit si l'image qu'elle
// peint a ete soumise par l'une des trames u - 1 a u - L (numero >= 1).
#include <iostream>
#include <vector>

int main() {
    int k = 0;
    if (!(std::cin >> k)) {
        return 0;
    }
    for (int s = 0; s < k; ++s) {
        long long n = 1, l = 0, t = 0;
        if (!(std::cin >> n >> l >> t)) {
            break;
        }
        if (n < 1) {
            n = 1;
        }

        std::vector<long long> peinte(static_cast<size_t>(t) + 1, 0);
        std::cout << "CIBLES " << n << ' ' << l;
        for (long long u = 1; u <= t; ++u) {
            peinte[static_cast<size_t>(u)] = (u - 1) % n;
            std::cout << ' ' << peinte[static_cast<size_t>(u)];
        }
        std::cout << '\n';

        long long conflits = 0;
        for (long long u = 1; u <= t; ++u) {
            bool enConflit = false;
            for (long long v = u - 1; v >= u - l && v >= 1; --v) {
                if (peinte[static_cast<size_t>(v)] == peinte[static_cast<size_t>(u)]) {
                    enConflit = true;   // une seule fois par trame
                    break;
                }
            }
            if (enConflit) {
                ++conflits;
            }
        }
        std::cout << "CONFLITS " << n << ' ' << l << ' ' << conflits << '\n';
    }
    return 0;
}
