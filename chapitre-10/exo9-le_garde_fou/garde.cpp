// Chapitre 10, exercice 9 : le garde-fou.
// Une fausse session (Wait, Begin, End) et une classe GardeTrame qui appelle
// End dans son destructeur : la trame est terminee sur TOUS les chemins de
// sortie du corps de boucle (bas du corps, continue, return).
// Les deux boucles sont ecrites avec de vrais continue et return.
#include <iostream>
#include <string>

class Session {
public:
    bool Wait() {
        if (mOuverte) {
            ++mRefus;
            return false;
        }
        return true;
    }
    bool Begin() {
        if (mOuverte) {
            ++mRefus;
            return false;
        }
        mOuverte = true;
        ++mDebuts;
        return true;
    }
    bool End() {
        if (!mOuverte) {
            ++mRefus;
            return false;
        }
        mOuverte = false;
        ++mFins;
        return true;
    }

    long long Debuts() const { return mDebuts; }
    long long Fins() const { return mFins; }
    long long Refus() const { return mRefus; }
    bool Ouverte() const { return mOuverte; }

private:
    bool mOuverte = false;
    long long mDebuts = 0;
    long long mFins = 0;
    long long mRefus = 0;
};

// Termine la trame quand on sort de sa portee, par n'importe quel chemin.
class GardeTrame {
public:
    explicit GardeTrame(Session &session) : mSession(session) {}
    ~GardeTrame() { mSession.End(); }

    // Une copie fermerait la trame deux fois : interdit.
    GardeTrame(const GardeTrame &) = delete;
    GardeTrame &operator=(const GardeTrame &) = delete;

private:
    Session &mSession;
};

// Le chemin de l'image : sans etat de Wait, N et C deviennent V.
static char Chemin(char signe, bool attenteOk) {
    if (!attenteOk && (signe == 'N' || signe == 'C')) {
        return 'V';
    }
    return signe;
}

static long long BoucleSansGarde(Session &session, const std::string &signes) {
    long long images = 0;
    for (char signe : signes) {
        const bool attenteOk = session.Wait();
        session.Begin();
        const char chemin = Chemin(signe, attenteOk);

        if (chemin == 'C') {
            continue;            // saute le End du bas : la trame reste ouverte
        }
        if (chemin == 'R') {
            return images;       // saute le End aussi, et la boucle s'arrete
        }
        if (chemin == 'N') {
            ++images;            // rendu
        }
        session.End();           // seulement si l'on arrive en bas du corps
    }
    return images;
}

static long long BoucleAvecGarde(Session &session, const std::string &signes) {
    long long images = 0;
    for (char signe : signes) {
        const bool attenteOk = session.Wait();
        session.Begin();
        GardeTrame garde(session);   // End sera appele a la sortie de ce corps
        const char chemin = Chemin(signe, attenteOk);

        if (chemin == 'C') {
            continue;            // le destructeur du garde appelle End
        }
        if (chemin == 'R') {
            return images;       // le destructeur du garde appelle End
        }
        if (chemin == 'N') {
            ++images;
        }
    }                            // chemin normal : le destructeur appelle End
    return images;
}

static void Afficher(const char *nom, const Session &session, long long images) {
    std::cout << nom << " DEBUTS " << session.Debuts() << " FINS " << session.Fins()
              << " REFUS " << session.Refus() << " IMAGES " << images << " OUVERTE "
              << (session.Ouverte() ? "OUI" : "NON") << '\n';
}

int main() {
    long long nbImages = 0;
    std::string signes;
    if (std::cin >> nbImages && nbImages > 0) {
        std::cin >> signes;
        if (static_cast<long long>(signes.size()) > nbImages) {
            signes.resize(static_cast<size_t>(nbImages));
        }
    }

    Session sans;
    const long long imagesSans = BoucleSansGarde(sans, signes);
    Afficher("SANS", sans, imagesSans);

    Session avec;
    const long long imagesAvec = BoucleAvecGarde(avec, signes);
    Afficher("AVEC", avec, imagesAvec);
    return 0;
}
