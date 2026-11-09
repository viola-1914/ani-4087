// Chapitre 9, exercice 5 : la manette qui s'eteint.
// Fait tourner deux saisies sur les memes images : AVEC le test de active,
// et SANS. Sans le test, l'extinction de la manette ressemble a un
// relachement : l'objet tombe. Avec, il peut rester coince dans la main.
#include <iostream>
#include <string>

int main() {
    long long nbImages = 0;
    if (!(std::cin >> nbImages)) {
        nbImages = 0;
    }
    std::string signes;
    if (nbImages > 0) {
        std::cin >> signes;
    }

    int precedent = 0;
    bool tenuAvec = false;
    bool tenuSans = false;
    bool parTerre = false;  // version SANS : l'objet est tombe

    long long prisesAvec = 0, prisesSans = 0, chutesSans = 0, coincesAvec = 0, differences = 0;

    for (long long t = 1; t <= nbImages; ++t) {
        const char s = (t - 1 < static_cast<long long>(signes.size()))
                           ? signes[static_cast<size_t>(t - 1)] : 'x';
        const int active = (s == 'x') ? 0 : 1;
        const int current = (s == '1') ? 1 : 0;
        const int changed = (current != precedent) ? 1 : 0;   // calcule meme pendant la coupure
        precedent = current;

        const bool prise = (current == 1 && changed == 1);
        const bool lache = (current == 0 && changed == 1);

        // Version AVEC : rien ne change si l'action est inactive.
        if (active == 1) {
            if (prise) {
                tenuAvec = true;
                ++prisesAvec;
            } else if (lache) {
                tenuAvec = false;
            }
        }

        // Version SANS : la meme chose, sans le test de active.
        if (prise) {
            if (!parTerre) {
                tenuSans = true;
                ++prisesSans;
            }
        } else if (lache) {
            if (tenuSans && active == 0) {
                ++chutesSans;               // l'objet tombe par terre
                parTerre = true;
            }
            tenuSans = false;
        }

        if (s == '0' && tenuAvec) {
            ++coincesAvec;
        }
        if (tenuAvec != tenuSans) {
            ++differences;
        }
        std::cout << t << ' ' << (tenuAvec ? "TENU" : "LIBRE") << ' '
                  << (tenuSans ? "TENU" : "LIBRE") << '\n';
    }

    std::cout << "PRISES AVEC " << prisesAvec << '\n';
    std::cout << "PRISES SANS " << prisesSans << '\n';
    std::cout << "CHUTES SANS " << chutesSans << '\n';
    std::cout << "COINCES AVEC " << coincesAvec << '\n';
    std::cout << "DIFFERENCES " << differences << '\n';
    return 0;
}
