// MaFenetre — chapitre 03, exercice 3 : cinq champs de configuration.
//
// Cinq champs de NkWindowConfig que le chapitre n'a pas montres, choisis dans
// NkWindowConfig.h. Chacun est numerote ci-dessous.
//
// Le programme ne se contente PAS de les poser : apres la creation, il demande
// a la fenetre ce qu'elle EST, via ses accesseurs, et l'ecrit dans le journal.
// L'en-tete le dit lui-meme : « un accesseur decrit le monde, pas notre
// memoire ». Demande d'un cote, obtenu de l'autre : c'est la seule facon de
// savoir si un champ a ete tenu.
//
// Sans cadre (champ 2), il n'y a plus de croix de fermeture : ECHAP ferme.
//
// PREMIERE VERSION REFUSEE A L'EDITION DE LIENS. Elle appelait aussi
// fenetre.GetHideSystemUI() pour verifier le champ 5 :
//
//   undefined reference to `nkentseu::NkWindow::GetHideSystemUI() const'
//
// L'accesseur est DECLARE dans NkWindow.h pour tout le monde, mais il n'est
// DEFINI que dans les dorsaux Noop, XLib, XCB, Wayland, Emscripten, HarmonyOS
// et Android. Le dorsal Win32 ne l'implemente pas. Sur Windows le symbole
// n'existe donc pas, et l'appel est retire ci-dessous : le champ 5 reste pose,
// mais il n'y a personne pour dire ce qu'il en advient.

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
    cfg.title = "MaFenetre - exo3 - cinq champs";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;

    // ---- LES CINQ CHAMPS DE L'EXERCICE -------------------------------------
    cfg.bgColor = 0x1E5AA8FF;   // 1. couleur de fond (defaut 0x141414FF)
    cfg.frame = false;          // 2. cadre et barre de titre (defaut true)
    cfg.opacity = 0.80f;        // 3. opacite globale [0..1] (defaut 1.0)
    cfg.alwaysOnTop = true;     // 4. au-dessus des autres fenetres (defaut false)
    cfg.hideSystemUI = true;    // 5. masquer barres systeme — ANDROID (defaut false)
    // ------------------------------------------------------------------------

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid())
        return 1;

    // Ce que la fenetre repond, champ par champ.
    logger.Info("[exo3] 1 bgColor      : demande {0} / obtenu {1}", cfg.bgColor, fenetre.GetBackgroundColor());
    logger.Info("[exo3] 2 frame        : demande {0} / decoree ? {1}", cfg.frame, fenetre.IsDecorated());
    logger.Info("[exo3] 3 opacity      : demande {0} / obtenu {1}", cfg.opacity, fenetre.GetOpacity());
    logger.Info("[exo3] 4 alwaysOnTop  : demande {0} / obtenu {1}", cfg.alwaysOnTop, fenetre.IsAlwaysOnTop());
    // 5. hideSystemUI : pas de GetHideSystemUI() sur Win32 (voir l'en-tete du
    //    fichier). Le champ est pose, rien ne peut le relire ici.
    logger.Info("[exo3] 5 hideSystemUI : demande {0} / non verifiable sur Win32", cfg.hideSystemUI);
    logger.Info("[exo3] (bonus) echelle DPI de l'ecran : {0}", fenetre.GetDpiScale());

    bool tourne = true;
    NkEventSystem &evenements = NkEvents();
    evenements.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { tourne = false; });
    evenements.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE)
            tourne = false;     // indispensable : sans cadre, pas de croix
    });

    while (tourne && fenetre.IsOpen()) {
        evenements.PollEvents();
        NkClock::Sleep((int64)10);
    }

    fenetre.Close();
    logger.Info("[exo3] Termine.");
    return 0;
}
