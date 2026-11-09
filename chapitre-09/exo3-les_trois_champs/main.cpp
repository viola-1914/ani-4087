// Chapitre 9, exercice 3 : les trois champs.
// Calcule, image par image, current, changed et active d'une action
// booleenne, et repere les faux relachements : ceux ou current tombe a 0
// parce que l'action est devenue inactive (manette eteinte).
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

    int precedent = 0;  // current avant l'image 1
    long long appuis = 0, relaches = 0, fauxRelaches = 0, tenues = 0;

    for (long long t = 1; t <= nbImages; ++t) {
        const char s = (t - 1 < static_cast<long long>(signes.size()))
                           ? signes[static_cast<size_t>(t - 1)] : 'x';
        const int active = (s == 'x') ? 0 : 1;
        const int current = (s == '1') ? 1 : 0;   // inactive => 0
        const int changed = (current != precedent) ? 1 : 0;

        std::cout << t << ' ' << current << ' ' << changed << ' ' << active << '\n';

        if (current == 1 && changed == 1) {
            ++appuis;
        }
        if (current == 0 && changed == 1) {
            ++relaches;
            if (active == 0) {
                ++fauxRelaches;
            }
        }
        if (current == 1) {
            ++tenues;
        }
        precedent = current;
    }

    std::cout << "APPUIS " << appuis << '\n';
    std::cout << "RELACHES " << relaches << '\n';
    std::cout << "FAUX RELACHES " << fauxRelaches << '\n';
    std::cout << "TENUES " << tenues << '\n';
    return 0;
}
