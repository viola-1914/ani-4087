// pointeur.cpp — chapitre 06, exercice 1 : le pointeur capture.
//
// Cache le pointeur dans la salle, l'empeche de sortir de la fenetre, et le
// rend par une touche :
//   Echap  : rend tout (pointeur visible et libre) ;
//   Entree : le reprend.
// (Pas F1/F2 : sur un portable HP, F1 ouvre le panneau des emojis.)
// Le programme rend aussi le pointeur avant de finir, y compris quand la
// fenetre est fermee par sa croix.
//
// ETAPE permet de rejouer les essais de essais.txt :
//   1 : ShowMouse(false) + CaptureMouse(true), et rien d'autre ;
//   2 : + ClipMouseToClient(true), sans protection du compteur de ShowMouse ;
//   3 : version rendue (protection du compteur, liberation a la sortie).

#include "pointeur.h"

#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkWindowEvent.h"

using namespace nkentseu;

#define ETAPE 3

namespace {

    // Sur Win32, ShowCursor est un COMPTEUR, pas un booleen (commentaire de
    // ShowMouse dans NkWindow.h) : deux ShowMouse(false) demandent deux
    // ShowMouse(true). Aucune lecture de la visibilite n'existe : on compte
    // donc nous-memes les caches qu'il faudra annuler.
    int sCachesEnCours = 0;

    // Le confinement SURVIT au processus s'il n'est pas relache (commentaire
    // de ClipMouseToClient) : on retient s'il est pose.
    bool sConfine = false;

    void Cacher(NkWindow &fenetre) {
#if ETAPE >= 3
        // Deja cache : un second appel enfoncerait le compteur d'un cran, et
        // une seule touche ne suffirait plus a faire revenir la fleche.
        if (sCachesEnCours > 0) {
            return;
        }
#endif
        fenetre.ShowMouse(false);
        ++sCachesEnCours;
    }

    void Montrer(NkWindow &fenetre) {
#if ETAPE >= 3
        // On defait TOUS les caches, quel que soit leur nombre.
        while (sCachesEnCours > 0) {
            fenetre.ShowMouse(true);
            --sCachesEnCours;
        }
#else
        // Version naive : un seul appel, quel que soit le nombre de caches.
        fenetre.ShowMouse(true);
        sCachesEnCours = 0;
#endif
    }

    void Confiner(NkWindow &fenetre) {
#if ETAPE >= 2
        // CaptureMouse (SetCapture sur Win32) redirige les messages souris
        // vers la fenetre, mais ne retient pas la fleche. C'est
        // ClipMouseToClient (ClipCursor) qui demande au systeme de garder le
        // pointeur dans le rectangle de la zone client.
        fenetre.ClipMouseToClient(true);
        sConfine = true;
#else
        (void)fenetre;
#endif
    }

} // namespace

void PointeurPrendre(NkWindow &fenetre) {
    Cacher(fenetre);
    fenetre.CaptureMouse(true);
    Confiner(fenetre);
}

void PointeurRendre(NkWindow &fenetre) {
    Montrer(fenetre);
    fenetre.CaptureMouse(false);
    if (sConfine) {
        fenetre.ClipMouseToClient(false);
        sConfine = false;
    }
}

void PointeurTerminer(NkWindow &fenetre) {
#if ETAPE >= 3
    // Le confinement survit au processus s'il n'est pas relache
    // (commentaire de ClipMouseToClient) : on rend tout avant de finir.
    PointeurRendre(fenetre);
#else
    // Etapes d'essai : on ne rend RIEN, pour voir ce que laisse un programme
    // qui oublie de le faire.
    (void)fenetre;
#endif
}

void PointeurTraiterEvenement(NkWindow &fenetre, NkEvent *ev) {
    if (ev == nullptr) {
        return;
    }

    if (NkKeyPressEvent *touche = ev->As<NkKeyPressEvent>()) {
        // Echap rend tout : c'est la touche a essayer avant toute autre.
        if (touche->GetKey() == NkKey::NK_ESCAPE) {
            PointeurRendre(fenetre);
        } else if (touche->GetKey() == NkKey::NK_ENTER) {
            PointeurPrendre(fenetre);
        }
        return;
    }

#if ETAPE >= 3
    // Fermeture par la croix : on rend le pointeur AVANT que la fenetre
    // disparaisse, sinon le confinement lui survit.
    if (ev->As<NkWindowCloseEvent>() != nullptr) {
        PointeurRendre(fenetre);
        return;
    }
#endif

#if ETAPE >= 2
    // Constate a l'essai : pose juste apres l'ouverture, le confinement ne
    // tenait pas. Windows le relache quand la fenetre au premier plan change,
    // et la salle ne passe au premier plan qu'APRES son ouverture. On le
    // repose donc a chaque retour au premier plan, et a chaque deplacement
    // ou redimensionnement, car le rectangle est en coordonnees ECRAN.
    if (sConfine && (ev->As<NkWindowFocusGainedEvent>() != nullptr ||
                     ev->As<NkWindowMoveEvent>() != nullptr ||
                     ev->As<NkWindowResizeEvent>() != nullptr)) {
        fenetre.ClipMouseToClient(true);
    }
#endif
}
