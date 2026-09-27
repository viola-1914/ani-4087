// MaFenetre — chapitre 03, exercice 5 : la taille qui change.
//
// Ecoute NkWindowResizeEvent et ecrit la nouvelle taille a chaque changement.
//
// Chaque ligne porte quatre choses, pour que les deux series soient
// comparables : le numero de l'evenement DANS LE GESTE en cours, la taille,
// l'ecart en millisecondes depuis l'evenement precedent, et le sens du
// changement (l'evenement transporte aussi l'ANCIENNE taille).
//
// Le geste lui-meme est encadre : le dorsal Win32 emet NkWindowResizeBeginEvent
// quand on attrape un bord, et NkWindowResizeEndEvent quand on le lache. On
// compte donc par geste, et non en vrac.
//
// Echap ou la croix pour terminer ; le total general s'affiche a la sortie.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKTime/NkClock.h"
#include "NKTime/NkChrono.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title = "MaFenetre - exo5 - redimensionnez-moi";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;
    cfg.resizable = true;           // sans cela, il n'y a rien a mesurer

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid())
        return 1;

    bool tourne = true;
    uint32 total = 0;               // evenements depuis le lancement
    uint32 dansLeGeste = 0;         // evenements depuis le dernier "debut"
    uint32 geste = 0;               // numero du geste
    float64 precedent = NkChrono::Now().milliseconds;

    NkEventSystem &evenements = NkEvents();

    evenements.AddEventCallback<NkWindowResizeBeginEvent>([&](NkWindowResizeBeginEvent *) {
        ++geste;
        dansLeGeste = 0;
        precedent = NkChrono::Now().milliseconds;
        logger.Info("[exo5] --- GESTE {0} : debut ---", geste);
    });

    evenements.AddEventCallback<NkWindowResizeEvent>([&](NkWindowResizeEvent *e) {
        const float64 maintenant = NkChrono::Now().milliseconds;
        const int64 ecart = (int64)(maintenant - precedent);
        precedent = maintenant;
        ++total;
        ++dansLeGeste;

        const char *sens = e->GotLarger() ? "+" : (e->GotSmaller() ? "-" : "=");
        logger.Info("[exo5] geste {0} evt {1} : {2}x{3} (avant {4}x{5}) {6} apres {7} ms",
                    geste, dansLeGeste, e->GetWidth(), e->GetHeight(),
                    e->GetPrevWidth(), e->GetPrevHeight(), sens, ecart);
    });

    evenements.AddEventCallback<NkWindowResizeEndEvent>([&](NkWindowResizeEndEvent *) {
        logger.Info("[exo5] --- GESTE {0} : fin, {1} evenements ---", geste, dansLeGeste);
    });

    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { tourne = false; });
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE)
            tourne = false;
    });

    logger.Info("[exo5] Pret. 1) tirez un bord LENTEMENT. 2) tirez-le d'UN COUP. Echap pour finir.");

    while (tourne) {
        evenements.PollEvents();
        NkClock::Sleep((int64)10);
    }

    fenetre.Close();
    logger.Info("[exo5] Total : {0} evenements de redimensionnement en {1} geste(s).", total, geste);
    return 0;
}
