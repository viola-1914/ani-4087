// MaFenetre — chapitre 03, exercice 6 : etat contre evenement.
//
// Deux compteurs sur la meme touche, Espace :
//   compteurEtat      ++ a chaque tour de boucle ou NkInput.IsKeyDown(NK_SPACE)
//   compteurEvenement ++ a chaque NkKeyPressEvent sur Espace
//
// Deux compteurs de plus, pour pouvoir EXPLIQUER l'ecart au lieu de le
// constater : l'auto-repetition du systeme (NkKeyRepeatEvent) et les
// relachements (NkKeyReleaseEvent). Le moteur les distingue de l'appui.
//
// Le nombre de tours de boucle est compte lui aussi : sans lui, le compteur
// d'etat ne veut rien dire.
//
// Espace pour mesurer ; Echap ou la croix pour terminer.

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkEventDispatcher.h"   // NkInput
#include "NKTime/NkClock.h"
#include "NKTime/NkChrono.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title = "MaFenetre - exo6 - tenez Espace une seconde";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid())
        return 1;

    bool tourne = true;
    uint64 tours = 0;               // tours de boucle (les « images » ici)
    uint64 compteurEtat = 0;        // 1) etat lu : IsKeyDown
    uint64 compteurEvenement = 0;   // 2) evenement recu : NkKeyPressEvent
    uint64 compteurRepetition = 0;  // auto-repetition du systeme
    uint64 compteurRelache = 0;     // relachements
    float64 debutAppui = 0.0;

    NkEventSystem &evenements = NkEvents();

    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() != NkKey::NK_SPACE) return;
        ++compteurEvenement;
        debutAppui = NkChrono::Now().milliseconds;
        logger.Info("[exo6] APPUI  #{0} (etat = {1}, tour {2})", compteurEvenement, compteurEtat, tours);
    });

    evenements.AddEventCallback<NkKeyRepeatEvent>([&](NkKeyRepeatEvent *e) {
        if (e->GetKey() != NkKey::NK_SPACE) return;
        ++compteurRepetition;
    });

    evenements.AddEventCallback<NkKeyReleaseEvent>([&](NkKeyReleaseEvent *e) {
        if (e->GetKey() != NkKey::NK_SPACE) return;
        ++compteurRelache;
        const float64 duree = NkChrono::Now().milliseconds - debutAppui;
        logger.Info("[exo6] RELACHE #{0} apres {1} ms | etat = {2} | appuis = {3} | repetitions = {4}",
                    compteurRelache, (int64)duree, compteurEtat, compteurEvenement, compteurRepetition);
    });

    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { tourne = false; });
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) tourne = false;
    });

    logger.Info("[exo6] Pret. Tenez ESPACE une seconde, relachez. Echap pour finir.");
    const float64 depart = NkChrono::Now().milliseconds;

    while (tourne) {
        evenements.PollEvents();
        ++tours;

        if (NkInput.IsKeyDown(NkKey::NK_SPACE))
            ++compteurEtat;         // UNE fois par tour, tant que la touche est tenue

        NkClock::Sleep((int64)10);
    }

    const float64 duree = NkChrono::Now().milliseconds - depart;
    fenetre.Close();

    logger.Info("[exo6] ---------------- RESULTAT ----------------");
    logger.Info("[exo6] Duree du programme : {0} ms, {1} tours de boucle ({2} par seconde)",
                (int64)duree, tours, (int64)(tours * 1000.0 / duree));
    logger.Info("[exo6] 1) compteur ETAT      (IsKeyDown)        : {0}", compteurEtat);
    logger.Info("[exo6] 2) compteur EVENEMENT (NkKeyPressEvent)  : {0}", compteurEvenement);
    logger.Info("[exo6]    pour memoire, auto-repetitions        : {0}", compteurRepetition);
    logger.Info("[exo6]    pour memoire, relachements            : {0}", compteurRelache);
    return 0;
}
