// MaFenetre — chapitre 03, exercice 10 : le retour de focus.
//
// L'accumulateur de l'exercice 9 est en place. On teste UNE difference :
//
//   MODE A (defaut, touche A) : on ne consomme PAS quand la fenetre n'a pas le
//                               focus. L'accumulateur continue de grossir.
//   MODE B (touche B)         : on consomme A CHAQUE IMAGE, meme sans focus.
//                               On jette le total, mais on le vide.
//
// Une seule touche separe les deux : c'est la ligne qui decide si les
// mouvements faits ailleurs reviennent d'un coup au retour du focus.
//
// PROTOCOLE, identique dans les deux modes :
//   1) cliquez dans la fenetre (elle prend le focus) ;
//   2) cliquez AILLEURS (une autre fenetre) : la fenetre perd le focus ;
//   3) bougez la souris pendant DIX SECONDES hors de la fenetre ;
//   4) revenez : cliquez dans la fenetre.
//   Puis ECHAP.
//
// Le journal marque la perte et le retour du focus, et affiche a ce moment la
// ce que l'accumulateur contenait. C'est le chiffre de l'exercice.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkMouseEvent.h"        // NkMouseRawEvent
#include "NKEvent/NkEventDispatcher.h"   // NkInput
#include "NKTime/NkClock.h"
#include "NKTime/NkChrono.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

static int32 Abs32(int32 v) { return v < 0 ? -v : v; }

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title = "MaFenetre - exo10 - A: sans vidage hors focus | B: avec";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid())
        return 1;

    bool tourne = true;
    bool viderHorsFocus = false;    // false = MODE A, true = MODE B
    bool aLeFocus = true;
    uint64 images = 0;

    int32 accumuleX = 0, accumuleY = 0;
    int64 cumulHorsFocus = 0;       // ce que la main a vraiment parcouru dehors
    float64 instantPerte = 0.0;

    NkEventSystem &evenements = NkEvents();

    evenements.AddEventCallback<NkMouseRawEvent>([&](NkMouseRawEvent *e) {
        accumuleX += e->GetDeltaX();
        accumuleY += e->GetDeltaY();
        if (!aLeFocus)
            cumulHorsFocus += Abs32(e->GetDeltaX());
    });

    evenements.AddEventCallback<NkWindowFocusLostEvent>([&](NkWindowFocusLostEvent *) {
        aLeFocus = false;
        cumulHorsFocus = 0;
        instantPerte = NkChrono::Now().milliseconds;
        logger.Info("[exo10] ===== FOCUS PERDU (mode {0}) : accumulateur = {1} =====",
                    viderHorsFocus ? "B" : "A", accumuleX);
    });

    evenements.AddEventCallback<NkWindowFocusGainedEvent>([&](NkWindowFocusGainedEvent *) {
        const int64 duree = (int64)(NkChrono::Now().milliseconds - instantPerte);
        aLeFocus = true;
        logger.Info("[exo10] ===== FOCUS RETROUVE (mode {0}) apres {1} ms hors focus =====",
                    viderHorsFocus ? "B" : "A", duree);
        logger.Info("[exo10] la main a parcouru {0} px dehors ; l'accumulateur contient {1}",
                    cumulHorsFocus, accumuleX);
    });

    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { tourne = false; });
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) tourne = false;
        if (e->GetKey() == NkKey::NK_A) {
            viderHorsFocus = false;
            logger.Info("[exo10] >>> MODE A : pas de remise a zero hors focus.");
        }
        if (e->GetKey() == NkKey::NK_B) {
            viderHorsFocus = true;
            logger.Info("[exo10] >>> MODE B : remise a zero a chaque image, focus ou non.");
        }
    });

    logger.Info("[exo10] MODE A actif (sans vidage hors focus). Touche B pour l'autre mode, A pour revenir.");
    logger.Info("[exo10] Cliquez dans la fenetre, cliquez AILLEURS, bougez 10 s, revenez. Echap pour finir.");

    while (tourne) {
        NkInput.NewFrame();
        evenements.PollEvents();
        ++images;

        int32 corrigeX = 0;

        if (aLeFocus || viderHorsFocus) {
            // On prend ET on vide. En mode B c'est fait meme sans focus :
            // le total est jete, mais il ne s'accumule pas.
            corrigeX = accumuleX;
            accumuleX = 0;
            accumuleY = 0;
        }
        // En mode A sans focus : on ne touche a rien. L'accumulateur grossit.

        if (aLeFocus && corrigeX != 0)
            logger.Info("[exo10] img {0} : consomme {1}", images, corrigeX);

        NkClock::Sleep((int64)10);
    }

    fenetre.Close();
    logger.Info("[exo10] Termine apres {0} images.", images);
    return 0;
}
