// pointeur.h — chapitre 06, exercice 1 : le pointeur capture.
// Declarations seulement : tout le code qui touche au pointeur est dans
// pointeur.cpp.
#pragma once

#include "NKWindow/Core/NkWindow.h"
#include "NKEvent/NkEvent.h"

// A appeler juste apres l'ouverture de la fenetre.
void PointeurPrendre(nkentseu::NkWindow &fenetre);

// Rend tout : pointeur visible, capture et confinement relaches.
void PointeurRendre(nkentseu::NkWindow &fenetre);

// A appeler juste avant de finir : rend le pointeur (version rendue).
void PointeurTerminer(nkentseu::NkWindow &fenetre);

// A appeler pour chaque evenement de la pompe. Echap rend, Entree reprend.
void PointeurTraiterEvenement(nkentseu::NkWindow &fenetre, nkentseu::NkEvent *ev);
