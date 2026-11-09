// Chapitre 9, exercice 7 : le premier objet saisi.
// Saisit l'objet le plus proche de la paume, fige l'attache une fois
//   attache = inverse(paume) * objet
// puis fait suivre la main a chaque image :
//   objet = paume * attache
// Poses vues de dessus : (x, y) en millimetres, k en quarts de tour.
#include <iostream>
#include <vector>

struct Pose {
    long long x = 0;
    long long y = 0;
    int k = 0;
};

static int Normaliser(long long k) {
    return static_cast<int>(((k % 4) + 4) % 4);
}

// Tourne (u, v) de k quarts de tour, sens inverse des aiguilles d'une montre.
static void Tourner(long long u, long long v, int k, long long &ou, long long &ov) {
    switch (Normaliser(k)) {
        case 1: ou = -v; ov = u; break;
        case 2: ou = -u; ov = -v; break;
        case 3: ou = v; ov = -u; break;
        default: ou = u; ov = v; break;
    }
}

// A * B : B exprimee dans le repere de A.
static Pose Composer(const Pose &a, const Pose &b) {
    Pose r;
    long long bx = 0, by = 0;
    Tourner(b.x, b.y, a.k, bx, by);
    r.x = a.x + bx;
    r.y = a.y + by;
    r.k = Normaliser(a.k + b.k);
    return r;
}

static Pose Inverse(const Pose &a) {
    Pose r;
    r.k = Normaliser(4 - a.k);
    Tourner(-a.x, -a.y, r.k, r.x, r.y);
    return r;
}

int main() {
    int nbObjets = 0;
    long long nbImages = 0;
    if (!(std::cin >> nbObjets >> nbImages)) {
        return 0;
    }

    std::vector<Pose> objets(static_cast<size_t>(nbObjets > 0 ? nbObjets : 0));
    for (Pose &o : objets) {
        long long k = 0;
        std::cin >> o.x >> o.y >> k;
        o.k = Normaliser(k);
    }

    int tenu = 0;            // numero de l'objet tenu, 0 si aucun
    Pose attache;
    int bPrecedent = 0;
    long long saisies = 0, ratees = 0;

    for (long long t = 1; t <= nbImages; ++t) {
        Pose paume;
        long long k = 0;
        int b = 0;
        if (!(std::cin >> paume.x >> paume.y >> k >> b)) {
            break;
        }
        paume.k = Normaliser(k);

        bool rate = false;
        if (b == 1 && bPrecedent == 0) {
            // Appui : l'objet le plus proche, a 150 mm au plus.
            int meilleur = 0;
            long long meilleureDistance = 0;
            for (size_t j = 0; j < objets.size(); ++j) {
                const long long dx = objets[j].x - paume.x;
                const long long dy = objets[j].y - paume.y;
                const long long d2 = dx * dx + dy * dy;
                if (d2 <= 22500 && (meilleur == 0 || d2 < meilleureDistance)) {
                    meilleur = static_cast<int>(j) + 1;
                    meilleureDistance = d2;
                }
            }
            if (meilleur != 0) {
                tenu = meilleur;
                attache = Composer(Inverse(paume), objets[static_cast<size_t>(meilleur - 1)]);
                ++saisies;
            } else {
                rate = true;
                ++ratees;
            }
        } else if (b == 0 && bPrecedent == 1) {
            tenu = 0;                // l'objet reste ou il est
        }
        bPrecedent = b;

        if (rate) {
            std::cout << t << " RATE\n";
        } else if (tenu != 0) {
            Pose &objet = objets[static_cast<size_t>(tenu - 1)];
            objet = Composer(paume, attache);
            std::cout << t << " TIENT " << tenu << ' ' << objet.x << ' ' << objet.y << ' '
                      << objet.k << '\n';
        } else {
            std::cout << t << " LIBRE\n";
        }
    }

    std::cout << "SAISIES " << saisies << '\n';
    std::cout << "RATEES " << ratees << '\n';
    for (size_t j = 0; j < objets.size(); ++j) {
        std::cout << "OBJET " << j + 1 << ' ' << objets[j].x << ' ' << objets[j].y << ' '
                  << objets[j].k << '\n';
    }
    return 0;
}
