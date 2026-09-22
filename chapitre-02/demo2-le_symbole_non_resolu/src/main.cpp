#include "salle.h"
#include "geometrie.h"
#include <cstdio>
int main()
{
    std::printf("MaSalle - demo 2\n");
    DecrireSalle();
    geometrie::Point porte{2.0, 0.0}, fenetre{5.0, 2.0};
    std::printf("Porte -> fenetre : %.2f m\n", geometrie::Distance(porte, fenetre));
    return 0;
}
