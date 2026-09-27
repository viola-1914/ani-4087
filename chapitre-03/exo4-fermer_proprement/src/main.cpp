// MaFenetre — chapitre 03, exercice 4 : fermer proprement.
//
// Deux facons de demander l'arret : la croix de la barre de titre, et la touche
// Echap. Les deux ne FONT rien elles-memes : elles posent le meme booleen a
// faux. La boucle porte sur ce booleen, PAS sur fenetre.IsOpen().
//
// Resultat : un seul chemin de sortie. Tout ce qu'il faudra faire en partant —
// fermer la fenetre, enregistrer, prevenir — s'ecrit UNE fois, apres la boucle.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKTime/NkClock.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title = "MaFenetre - exo4 - croix ou Echap";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid())
        return 1;

    bool tourne = true;                 // LA decision du programme
    const char *raison = "aucune";      // par ou la sortie a ete demandee

    NkEventSystem &evenements = NkEvents();

    // Chemin 1 : la croix de la barre de titre.
    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) {
        raison = "la croix (NkWindowCloseEvent)";
        tourne = false;                 // on NOTE la demande, on ne ferme pas ici
    });

    // Chemin 2 : la touche Echap.
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) {
            raison = "la touche Echap (NkKeyPressEvent)";
            tourne = false;             // exactement la meme ligne que ci-dessus
        }
    });

    logger.Info("[exo4] Boucle demarree. Fermez par la croix OU par Echap.");

    while (tourne) {                    // et NON : while (tourne && fenetre.IsOpen())
        evenements.PollEvents();
        NkClock::Sleep((int64)10);
    }

    // ---- LE SEUL CHEMIN DE SORTIE -----------------------------------------
    // Quelle que soit la demande, on passe forcement ici, et une seule fois.
    logger.Info("[exo4] Sortie demandee par : {0}", raison);
    fenetre.Close();
    logger.Info("[exo4] Fenetre fermee, terminaison propre. Code 0.");
    // ------------------------------------------------------------------------
    return 0;
}
