// MaFenetre — chapitre 03, exercice 9 : le defaut corrige.
//
// L'exercice 8 a montre que NkInput.MouseRawDeltaX() garde sa derniere valeur
// quand plus rien n'arrive : NewFrame() remet a zero le delta d'image, pas le
// delta brut. On repare ici, cote appelant, avec un ACCUMULATEUR :
//
//   1) un rappel sur NkMouseRawEvent qui AJOUTE le delta a un total ;
//   2) une consommation, une fois par image, qui PREND le total et le remet a
//      zero -- prendre et vider dans le meme geste, c'est tout le remede.
//
// Les deux series sont ecrites cote a cote a chaque image :
//   brut   = NkInput.MouseRawDeltaX()   (la lecture de l'exercice 8)
//   corrige = le total consomme          (l'accumulateur de cet exercice)
//
// Protocole identique a l'exercice 8 : cliquez dans la fenetre, bougez la
// souris, puis POSEZ LA MAIN. Le programme s'arme apres 25 images de mouvement
// et s'arrete apres 20 images immobiles, en marquant ces vingt lignes.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkMouseEvent.h"        // NkMouseRawEvent
#include "NKEvent/NkEventDispatcher.h"   // NkInput
#include "NKTime/NkClock.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

static const uint32 IMAGES_APRES_ARRET = 20;
static const uint32 IMAGES_POUR_ARMER = 25;

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title = "MaFenetre - exo9 - accumulateur";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid())
        return 1;

    bool tourne = true;
    uint64 images = 0;
    uint32 silence = 0;
    uint32 actives = 0;
    bool arme = false;

    // ---- L'ACCUMULATEUR ----------------------------------------------------
    int32 accumuleX = 0;        // rempli par le rappel, vide par la boucle
    int32 accumuleY = 0;

    NkEventSystem &evenements = NkEvents();

    // 1) LE RAPPEL QUI AJOUTE. Il ne remplace pas, il ajoute : si deux
    //    evenements bruts arrivent dans la meme image, les deux comptent.
    evenements.AddEventCallback<NkMouseRawEvent>([&](NkMouseRawEvent *e) {
        accumuleX += e->GetDeltaX();
        accumuleY += e->GetDeltaY();
    });
    // ------------------------------------------------------------------------

    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { tourne = false; });
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) tourne = false;
    });

    logger.Info("[exo9] Cliquez dans la fenetre, BOUGEZ la souris, puis POSEZ LA MAIN.");
    logger.Info("[exo9] brut = MouseRawDeltaX() (exo 8) | corrige = accumulateur consomme (exo 9)");

    while (tourne) {
        NkInput.NewFrame();
        evenements.PollEvents();        // c'est ici que le rappel ajoute
        ++images;

        // 2) LA CONSOMMATION : on prend le total ET on le remet a zero.
        const int32 corrigeX = accumuleX;
        accumuleX = 0;
        const int32 corrigeY = accumuleY;
        accumuleY = 0;

        const int32 brutX = NkInput.MouseRawDeltaX();   // la lecture non corrigee

        const bool immobile = (corrigeX == 0) && (corrigeY == 0);
        if (!immobile) {
            ++actives;
            silence = 0;
            if (!arme && actives >= IMAGES_POUR_ARMER) {
                arme = true;
                logger.Info("[exo9] --- mouvement confirme : la surveillance de l'arret commence ---");
            }
        } else if (arme) {
            ++silence;
        }

        if (silence == 0)
            logger.Info("[exo9] img {0} : brut={1} | corrige={2}", images, brutX, corrigeX);
        else
            logger.Info("[exo9] img {0} : brut={1} | corrige={2}   <-- APRES L'ARRET {3}/{4}",
                        images, brutX, corrigeX, silence, IMAGES_APRES_ARRET);

        if (arme && silence >= IMAGES_APRES_ARRET) {
            logger.Info("[exo9] {0} images sans le moindre mouvement : arret.", IMAGES_APRES_ARRET);
            tourne = false;
        }

        NkClock::Sleep((int64)10);
    }

    fenetre.Close();
    logger.Info("[exo9] Termine apres {0} images.", images);
    return 0;
}
