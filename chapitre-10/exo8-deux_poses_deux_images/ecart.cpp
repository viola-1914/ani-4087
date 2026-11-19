// Chapitre 10, exercice 8 : deux poses, deux images.
// Mesure, image par image, l'ecart entre la pose du rendu et la pose relue
// a la fin de trame : distance des positions (mm) et ecart de lacet sans
// signe (milliemes de degre). Puis les moyennes et la vitesse de la tete.
#include <cmath>
#include <cstdint>
#include <iostream>

int main() {
    long long n = 0;
    int64_t d = 0;
    if (!(std::cin >> n >> d)) {
        std::cout << "AUCUNE\n";
        return 0;
    }

    int64_t sommePosition = 0;
    int64_t sommeAngle = 0;
    long long lues = 0;

    for (long long t = 1; t <= n; ++t) {
        int64_t x1 = 0, z1 = 0, a1 = 0, x2 = 0, z2 = 0, a2 = 0;
        if (!(std::cin >> x1 >> z1 >> a1 >> x2 >> z2 >> a2)) {
            break;
        }
        ++lues;
        const long double dx = static_cast<long double>(x2 - x1);
        const long double dz = static_cast<long double>(z2 - z1);
        const int64_t p = std::llround(std::sqrt(dx * dx + dz * dz));
        const int64_t a = (a2 - a1 < 0) ? (a1 - a2) : (a2 - a1);   // sans signe

        std::cout << t << " POSITION " << p << " ANGLE " << a << '\n';
        sommePosition += p;
        sommeAngle += a;
    }

    if (n == 0 || d == 0 || lues == 0) {
        std::cout << "AUCUNE\n";
        return 0;
    }
    // Ecarts positifs : la division entiere arrondit a l'entier inferieur.
    std::cout << "MOYENNE POSITION " << sommePosition / lues << '\n';
    std::cout << "MOYENNE ANGLE " << sommeAngle / lues << '\n';
    // milliemes de degre par milliseconde = degres par seconde
    std::cout << "VITESSE " << sommeAngle / (lues * d) << '\n';
    return 0;
}
