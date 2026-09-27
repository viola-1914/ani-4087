// MaFenetre — chapitre 03, exercice 2 : la fenetre qui ne repond pas.
//
// Meme programme qu'a l'exercice 1, a UNE chose pres : le corps de la boucle
// est commente. Plus de PollEvents, plus de Sleep. La fenetre s'ouvre, puis
// l'application cesse de repondre au systeme.
//
// La ligne de journal avant la boucle donne l'instant T0, horodate a la
// milliseconde dans logs/app.log : c'est le depart du chronometre.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKTime/NkClock.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title = "MaFenetre - exo2 - sans PollEvents";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid())
        return 1;

    bool tourne = true;
    NkEventSystem &evenements = NkEvents();
    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { tourne = false; });

    logger.Info("[exo2] T0 : entree dans la boucle SANS PollEvents.");

    while (tourne && fenetre.IsOpen()) {
        // evenements.PollEvents();          <-- RETIRE : c'est tout l'exercice.
        // NkClock::Sleep((int64)10);        <-- retire aussi : le corps est vide.
        //
        // Plus personne ne depile les messages du systeme. Windows va finir par
        // declarer la fenetre bloquee. Au passage, la boucle tourne a vide : un
        // coeur du processeur est occupe a 100 % pour ne rien faire.
    }

    fenetre.Close();
    logger.Info("[exo2] Termine.");
    return 0;
}
