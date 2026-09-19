#include <iostream>

int main() {
#ifdef FILTRE_ACTIF
    std::cout << "FILTRE_ACTIF est defini : le filtre a ete applique." << std::endl;
#else
    std::cout << "FILTRE_ACTIF n'est pas defini : le filtre n'a pas ete applique." << std::endl;
#endif
    return 0;
}
