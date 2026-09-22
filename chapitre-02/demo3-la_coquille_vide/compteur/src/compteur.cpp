#include "compteur.h"

// Les definitions n'existent que si la classe complete a ete declaree.
#ifdef COMPTEUR_COMPLET
Compteur::Compteur() : m_valeur(0) {}
void Compteur::incrementer() { ++m_valeur; }
int  Compteur::valeur() const { return m_valeur; }
#endif
