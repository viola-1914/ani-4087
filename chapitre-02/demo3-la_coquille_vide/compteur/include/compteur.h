#pragma once
// Demo 3 : un meme en-tete, deux classes differentes selon COMPTEUR_COMPLET.

#ifdef COMPTEUR_COMPLET
// Avec le define : la classe complete, trois fonctions membres.
class Compteur {
public:
    Compteur();
    void incrementer();
    int  valeur() const;
private:
    int m_valeur;
};
#else
// Sans le define : une coquille vide, aucune fonction membre.
class Compteur {
};
#endif
