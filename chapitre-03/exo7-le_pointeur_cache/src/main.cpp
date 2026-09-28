// MaFenetre — chapitre 03, exercice 7 : le pointeur cache.
//
// Curseur cache (ShowMouse(false)) et confine a la zone client
// (ClipMouseToClient(true)). A chaque image : la position x, y et le rawDelta.
//
// ⚠️ LE CONFINEMENT SURVIT AU PROCESSUS s'il n'est pas relache : NkWindow.h le
//    dit noir sur blanc. Tout appelant doit appeler ClipMouseToClient(false)
//    avant de quitter, sinon la souris de l'utilisateur reste prisonniere d'un
//    rectangle apres la fermeture du programme. Idem pour ShowMouse, qui est un
//    COMPTEUR sur Win32 : un false demande un true.
//
// NkInput.NewFrame() est appele UNE fois par tour, AVANT de depiler : sans lui,
// les deltas d'image ne sont jamais remis a zero.
//
// Pour ne pas noyer le journal a 90 lignes par seconde, seules les images ou
// quelque chose bouge sont ecrites ; les images muettes sont comptees.
//
// Echap pour terminer.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkEventDispatcher.h"   // NkInput
#include "NKTime/NkClock.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

static int32 Abs32(int32 v) { return v < 0 ? -v : v; }

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title = "MaFenetre - exo7 - curseur cache et confine";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid())
        return 1;

    fenetre.ShowMouse(false);           // 1) cacher
    fenetre.ClipMouseToClient(true);    // 2) confiner
    logger.Info("[exo7] Curseur cache et confine a la zone client. Bougez jusqu'au bord. Echap pour finir.");

    bool tourne = true;
    uint64 images = 0;                  // tours de boucle
    uint64 imagesMuettes = 0;           // rien n'a bouge
    uint64 imagesBloquees = 0;          // rawDelta != 0 MAIS position figee
    int64 cumulRawX = 0, cumulRawY = 0; // somme des |rawDelta|
    int32 precX = -1, precY = -1;

    NkEventSystem &evenements = NkEvents();
    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { tourne = false; });
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) tourne = false;
    });

    while (tourne) {
        NkInput.NewFrame();             // AVANT de depiler : remet les deltas d'image a zero
        evenements.PollEvents();
        ++images;

        const int32 x = NkInput.MouseX();
        const int32 y = NkInput.MouseY();
        const int32 rdx = NkInput.MouseRawDeltaX();
        const int32 rdy = NkInput.MouseRawDeltaY();

        const bool positionBouge = (x != precX) || (y != precY);
        const bool brutBouge = (rdx != 0) || (rdy != 0);

        cumulRawX += Abs32(rdx);
        cumulRawY += Abs32(rdy);

        if (brutBouge && !positionBouge) {
            ++imagesBloquees;
            logger.Info("[exo7] img {0} : x={1} y={2} FIGEE | rawDelta=({3},{4}) <-- le bord",
                        images, x, y, rdx, rdy);
        } else if (positionBouge || brutBouge) {
            logger.Info("[exo7] img {0} : x={1} y={2} | rawDelta=({3},{4})", images, x, y, rdx, rdy);
        } else {
            ++imagesMuettes;
        }

        precX = x;
        precY = y;
        NkClock::Sleep((int64)10);
    }

    // ---- RENDRE LA SOURIS, QUOI QU'IL ARRIVE -------------------------------
    fenetre.ClipMouseToClient(false);
    fenetre.ShowMouse(true);
    // ------------------------------------------------------------------------

    fenetre.Close();
    logger.Info("[exo7] ---------------- RESULTAT ----------------");
    logger.Info("[exo7] Images : {0} au total, {1} muettes, {2} avec position FIGEE et rawDelta non nul",
                images, imagesMuettes, imagesBloquees);
    logger.Info("[exo7] Cumul des |rawDelta| : X={0} Y={1}", cumulRawX, cumulRawY);
    logger.Info("[exo7] Derniere position lue : x={0} y={1}", precX, precY);
    logger.Info("[exo7] Curseur rendu visible et confinement relache.");
    return 0;
}
