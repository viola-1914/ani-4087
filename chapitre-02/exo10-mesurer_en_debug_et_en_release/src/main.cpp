#include <chrono>
#include <cmath>
#include <cstdio>
#include <vector>

// Calcul lourd : transformer 1 000 000 de points 3D par une matrice 4x4,
// comme le ferait un moteur pour chaque image.
struct Vec4 { float x, y, z, w; };

static Vec4 transformer(const float m[16], const Vec4& v) {
    return {
        m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12] * v.w,
        m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13] * v.w,
        m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
        m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w
    };
}

int main() {
#if defined(NDEBUG)
    const char* config = "Release";
#else
    const char* config = "Debug";
#endif
    const int nbPoints = 1000000;
    const int nbImages = 50;          // on mesure 50 "images"
    const double budgetMs = 11.0;     // une image de casque a 90 Hz

    std::vector<Vec4> points(nbPoints);
    for (int i = 0; i < nbPoints; ++i)
        points[i] = { float(i % 97), float(i % 89), float(i % 83), 1.0f };
    std::vector<Vec4> sortie(nbPoints);

    double total = 0.0, meilleur = 1e9, pire = 0.0;
    float temoin = 0.0f;              // empeche le compilateur de supprimer le calcul

    for (int image = 0; image < nbImages; ++image) {
        float a = 0.01f * image;
        float m[16] = { std::cos(a), std::sin(a), 0, 0,
                       -std::sin(a), std::cos(a), 0, 0,
                        0, 0, 1, 0,
                        1, 2, 3, 1 };

        auto debut = std::chrono::steady_clock::now();
        for (int i = 0; i < nbPoints; ++i)
            sortie[i] = transformer(m, points[i]);
        auto fin = std::chrono::steady_clock::now();

        temoin += sortie[image * 1000].x;
        double ms = std::chrono::duration<double, std::milli>(fin - debut).count();
        total += ms;
        if (ms < meilleur) meilleur = ms;
        if (ms > pire) pire = ms;
    }

    double moyenne = total / nbImages;
    std::printf("Configuration : %s\n", config);
    std::printf("Calcul        : %d points transformes par image, %d images\n", nbPoints, nbImages);
    std::printf("Temps moyen   : %.3f ms par image\n", moyenne);
    std::printf("Meilleur      : %.3f ms\n", meilleur);
    std::printf("Pire          : %.3f ms\n", pire);
    std::printf("Budget casque : %.1f ms -> %s\n", budgetMs,
                moyenne <= budgetMs ? "tient dans le budget" : "DEPASSE le budget");
    std::printf("(temoin : %.1f)\n", temoin);
    return 0;
}
