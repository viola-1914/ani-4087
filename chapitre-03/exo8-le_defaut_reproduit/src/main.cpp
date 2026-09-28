// MaFenetre — chapitre 03, exercice 8 : le defaut reproduit.
//
// On affiche rawDeltaX A CHAQUE IMAGE, tel quel. On n'accumule rien, on ne
// filtre rien, on ne remet rien a zero soi-meme : on lit et on ecrit.
//
// A cote, et seulement pour comparer, MouseDeltaThisFrameX() -- le delta de
// l'image, celui que NkInput.NewFrame() remet a zero.
//
// Le programme ARME d'abord son compteur : il attend d'avoir vu un vrai
// mouvement (25 images ou la souris bouge) avant de surveiller l'arret. Sans
// cela il se fermerait des le lancement, puisque la souris ne bouge pas encore.
//
// Une fois arme, il s'arrete TOUT SEUL apres 20 images consecutives sans le
// moindre mouvement, et marque ces vingt lignes. Ce sont elles que l'exercice
// demande : plus besoin de les compter a la main.
//
// Protocole : cliquez dans la fenetre, bougez la souris franchement pendant
// deux ou trois secondes, puis POSEZ LA MAIN et ne touchez plus a rien.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkEventDispatcher.h"   // NkInput
#include "NKTime/NkClock.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

static const uint32 IMAGES_APRES_ARRET = 20;   // fin : images immobiles d'affilee
static const uint32 IMAGES_POUR_ARMER = 25;    // debut : images avec mouvement

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title = "MaFenetre - exo8 - bougez, puis posez la main";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid())
        return 1;

    bool tourne = true;
    uint64 images = 0;
    uint32 silence = 0;             // images consecutives sans mouvement
    uint32 actives = 0;             // images avec mouvement, depuis le debut
    bool arme = false;              // vrai quand la souris a vraiment bouge

    NkEventSystem &evenements = NkEvents();
    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { tourne = false; });
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) tourne = false;
    });

    logger.Info("[exo8] Cliquez dans la fenetre, BOUGEZ la souris, puis POSEZ LA MAIN.");
    logger.Info("[exo8] Le compteur s'arme apres {0} images de mouvement, puis l'arret est detecte apres {1} images immobiles.",
                IMAGES_POUR_ARMER, IMAGES_APRES_ARRET);

    while (tourne) {
        NkInput.NewFrame();
        evenements.PollEvents();
        ++images;

        const int32 rawX = NkInput.MouseRawDeltaX();        // ce que l'exercice demande
        const int32 imageX = NkInput.MouseDeltaThisFrameX(); // pour comparer

        const bool immobile = (imageX == 0) && (NkInput.MouseDeltaThisFrameY() == 0);

        if (!immobile) {
            ++actives;
            silence = 0;
            if (!arme && actives >= IMAGES_POUR_ARMER) {
                arme = true;
                logger.Info("[exo8] --- mouvement confirme : la surveillance de l'arret commence ---");
            }
        } else if (arme) {
            ++silence;
        }

        if (silence == 0)
            logger.Info("[exo8] img {0} : rawDeltaX={1} | deltaImageX={2}", images, rawX, imageX);
        else
            logger.Info("[exo8] img {0} : rawDeltaX={1} | deltaImageX={2}   <-- APRES L'ARRET {3}/{4}",
                        images, rawX, imageX, silence, IMAGES_APRES_ARRET);

        if (arme && silence >= IMAGES_APRES_ARRET) {
            logger.Info("[exo8] {0} images sans le moindre mouvement : arret.", IMAGES_APRES_ARRET);
            tourne = false;
        }

        NkClock::Sleep((int64)10);
    }

    fenetre.Close();
    logger.Info("[exo8] Termine apres {0} images.", images);
    return 0;
}
