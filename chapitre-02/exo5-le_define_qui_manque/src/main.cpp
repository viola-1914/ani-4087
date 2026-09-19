#include <iostream>
#include "compteur.h"

int main() {
    Compteur c;
    c.incrementer();
    c.incrementer();
    std::cout << "Valeur du compteur : " << c.valeur() << std::endl;
    return 0;
}
