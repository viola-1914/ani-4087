#include <cstdio>
#include "compteur.h"

int main()
{
    Compteur c;              // appelle Compteur::Compteur()
    c.incrementer();         // appelle Compteur::incrementer()
    c.incrementer();
    std::printf("Valeur du compteur : %d\n", c.valeur());   // Compteur::valeur()
    return 0;
}
