// MaFenetre — chapitre 03, exercice 1 : la fenetre nue.
// Ouvrir une fenetre, ecouter sa fermeture, tourner jusque-la.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKTime/NkClock.h"

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;                       // la configuration est une structure
    cfg.title = "MaFenetre - ANI-4087 - Mafo";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;

    NkWindow fenetre(cfg);                    // le constructeur ne leve pas d'exception
    if (!fenetre.IsValid())                   // c'est IsValid() qui dit si ca a marche
        return 1;

    bool tourne = true;
    NkEventSystem &evenements = NkEvents();
    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { tourne = false; });

    while (tourne && fenetre.IsOpen()) {
        evenements.PollEvents();              // sans ca, le systeme croit l'application figee
        NkClock::Sleep((int64)10);            // pas de rendu : on rend la main au processeur
    }

    fenetre.Close();
    return 0;
}
