#pragma once

// Si AVEC_COMPTEUR est defini : classe complete.
// Sinon : coquille vide, la classe existe mais n'a aucun membre.
#ifdef AVEC_COMPTEUR
class Compteur {
public:
    void incrementer() { ++m_valeur; }
    int valeur() const { return m_valeur; }
private:
    int m_valeur = 0;
};
#else
class Compteur {};
#endif
