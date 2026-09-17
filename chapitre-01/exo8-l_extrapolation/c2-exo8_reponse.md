# Exercice 8 — L'extrapolation

L'extrapolation consiste à estimer une pose future en supposant que les
vitesses linéaire et angulaire restent constantes pendant une durée `dt`.

La position est avancée avec la vitesse linéaire, tandis que l'orientation
est avancée à partir de la vitesse angulaire.

## Code C++

```cpp
#include <iostream>
#include <iomanip>
#include <cmath>

struct Vec3
{
    double x, y, z;
};

struct Quaternion
{
    double w, x, y, z;
};

struct Pose
{
    Vec3 position;
    Quaternion orientation;
};

double norme(const Vec3& v)
{
    return std::sqrt(
        v.x * v.x +
        v.y * v.y +
        v.z * v.z
    );
}

Quaternion multiplierQuaternion(
    const Quaternion& a,
    const Quaternion& b)
{
    return {
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w
    };
}

Quaternion normaliser(const Quaternion& q)
{
    double n = std::sqrt(
        q.w*q.w +
        q.x*q.x +
        q.y*q.y +
        q.z*q.z
    );

    if (n < 1e-12)
        return {1.0, 0.0, 0.0, 0.0};

    return {
        q.w / n,
        q.x / n,
        q.y / n,
        q.z / n
    };
}

Pose extrapoler(
    const Pose& pose,
    const Vec3& vitesseLineaire,
    const Vec3& vitesseAngulaire,
    double dt)
{
    Pose resultat = pose;

    // Avancement de la position
    resultat.position.x += vitesseLineaire.x * dt;
    resultat.position.y += vitesseLineaire.y * dt;
    resultat.position.z += vitesseLineaire.z * dt;

    // Norme de la vitesse angulaire
    double omega = norme(vitesseAngulaire);

    // Cas particulier : aucune rotation
    if (omega < 1e-12)
    {
        resultat.orientation = pose.orientation;
        return resultat;
    }

    // Axe unitaire de rotation
    Vec3 axe{
        vitesseAngulaire.x / omega,
        vitesseAngulaire.y / omega,
        vitesseAngulaire.z / omega
    };

    // Angle parcouru pendant dt
    double angle = omega * dt;
    double demiAngle = angle / 2.0;

    Quaternion delta{
        std::cos(demiAngle),
        axe.x * std::sin(demiAngle),
        axe.y * std::sin(demiAngle),
        axe.z * std::sin(demiAngle)
    };

    // Mise à jour de l'orientation
    resultat.orientation =
        normaliser(multiplierQuaternion(delta, pose.orientation));

    return resultat;
}

int main()
{
    Pose pose;
    Vec3 vitesseLineaire;
    Vec3 vitesseAngulaire;
    double dt;

    // Position initiale
    std::cin >> pose.position.x
             >> pose.position.y
             >> pose.position.z;

    // Quaternion : w x y z
    std::cin >> pose.orientation.w
             >> pose.orientation.x
             >> pose.orientation.y
             >> pose.orientation.z;

    // Vitesse linéaire en m/s
    std::cin >> vitesseLineaire.x
             >> vitesseLineaire.y
             >> vitesseLineaire.z;

    // Vitesse angulaire en rad/s
    std::cin >> vitesseAngulaire.x
             >> vitesseAngulaire.y
             >> vitesseAngulaire.z;

    // Durée en secondes
    std::cin >> dt;

    Pose future = extrapoler(
        pose,
        vitesseLineaire,
        vitesseAngulaire,
        dt
    );

    std::cout << std::fixed << std::setprecision(4);

    std::cout << "Position extrapolee : "
              << future.position.x << " "
              << future.position.y << " "
              << future.position.z << '\n';

    std::cout << "Orientation extrapolee : "
              << future.orientation.w << " "
              << future.orientation.x << " "
              << future.orientation.y << " "
              << future.orientation.z << '\n';

    return 0;
}
```

## Principe de l'extrapolation

Pour la position, j'utilise :

```text
position_future = position + vitesse_lineaire * dt
```

La position future est donc obtenue en ajoutant à la position initiale le
déplacement effectué pendant la durée `dt`.

Pour l'orientation, la norme de la vitesse angulaire donne la vitesse de
rotation en radians par seconde.

L'angle parcouru pendant `dt` est donc :

```text
angle = |vitesse_angulaire| * dt
```

La direction de la vitesse angulaire donne l'axe de rotation.

À partir de cet axe et de cet angle, je construis un quaternion `delta`
représentant la rotation effectuée pendant `dt`.

Ce quaternion est ensuite composé avec l'orientation initiale afin d'obtenir
l'orientation extrapolée.

## Cas particulier : vitesse angulaire nulle

Il faut traiter séparément le cas où la vitesse angulaire est nulle.

En effet, le calcul normal de l'axe de rotation serait :

```text
axe = vitesse_angulaire / |vitesse_angulaire|
```

Mais lorsque :

```text
|vitesse_angulaire| = 0
```

ce calcul provoquerait une division par zéro.

J'utilise donc le garde-fou suivant :

```cpp
if (omega < 1e-12)
{
    resultat.orientation = pose.orientation;
    return resultat;
}
```

Dans ce cas, aucune rotation n'a lieu et l'orientation initiale est
simplement conservée.

## Test du cas de vitesse angulaire nulle

Pour vérifier réellement ce comportement, j'ai exécuté le programme avec un
cas dont je connaissais le résultat à l'avance.

J'ai choisi :

```text
Position initiale :       (0, 0, 0)
Orientation initiale :    (1, 0, 0, 0)
Vitesse lineaire :        (0, 0, -1) m/s
Vitesse angulaire :       (0, 0, 0) rad/s
dt :                      1 seconde
```

Dans la convention utilisée ici, l'avant correspond à la direction `-Z`.

Avec une vitesse de 1 m/s vers l'avant pendant une seconde, la position doit
donc avancer exactement d'un mètre.

Comme la vitesse angulaire est nulle, l'orientation doit rester inchangée.

## Valeurs saisies

J'ai donné au programme les valeurs suivantes :

```text
0 0 0
1 0 0 0
0 0 -1
0 0 0
1
```

Elles correspondent successivement à :

```text
position initiale
orientation initiale
vitesse linéaire
vitesse angulaire
dt
```

## Résultat réel de l'exécution

Après exécution, mon programme a affiché :

```text
Position extrapolee : 0.0000 0.0000 -1.0000
Orientation extrapolee : 1.0000 0.0000 0.0000 0.0000
```

Le résultat obtenu correspond exactement à celui attendu.

## Vérification de la position

Le calcul attendu était :

```text
position_future
= position_initiale + vitesse_lineaire * dt

= (0, 0, 0) + (0, 0, -1) * 1

= (0, 0, -1)
```

Le programme affiche effectivement :

```text
Position extrapolee : 0.0000 0.0000 -1.0000
```

La position a donc avancé exactement d'un mètre vers l'avant.

## Vérification de l'orientation

Pour ce test :

```text
vitesse_angulaire = (0, 0, 0)
```

donc :

```text
|vitesse_angulaire| = 0
```

Le programme entre alors dans le cas particulier :

```cpp
if (omega < 1e-12)
{
    resultat.orientation = pose.orientation;
    return resultat;
}
```

L'orientation initiale était :

```text
(1, 0, 0, 0)
```

et le programme affiche :

```text
Orientation extrapolee : 1.0000 0.0000 0.0000 0.0000
```

L'orientation est donc bien restée inchangée.

Le garde-fou évite ainsi la division par zéro tout en produisant le
comportement attendu.

## Ce que ce test vérifie

Ce test permet de vérifier directement deux éléments du programme.

Premièrement, l'extrapolation linéaire fonctionne : avec une vitesse de
1 m/s pendant 1 seconde, la position avance bien de 1 mètre.

Deuxièmement, le traitement d'une vitesse angulaire nulle fonctionne :
aucune division par zéro n'est effectuée et l'orientation initiale est
conservée.

Il s'agit volontairement d'un cas simple dont le résultat pouvait être
calculé avant l'exécution. Cela permet de comparer directement le résultat
attendu au résultat réellement produit par le code.

## Conclusion

L'extrapolation permet d'estimer une pose future à partir d'une pose initiale,
d'une vitesse linéaire, d'une vitesse angulaire et d'une durée `dt`.

La position est avancée selon :

```text
position_future = position + vitesse_lineaire * dt
```

et l'orientation est avancée à partir de l'axe et de l'angle obtenus avec la
vitesse angulaire.

Le cas d'une vitesse angulaire nulle doit être traité séparément afin
d'éviter une division par zéro.

L'exécution réalisée avec une vitesse linéaire de 1 m/s vers l'avant, une
vitesse angulaire nulle et `dt = 1 s` a donné :

```text
Position extrapolee : 0.0000 0.0000 -1.0000
Orientation extrapolee : 1.0000 0.0000 0.0000 0.0000
```

Le test confirme donc expérimentalement que la position est correctement
extrapolée d'un mètre et que l'orientation reste inchangée lorsque la vitesse
angulaire est nulle.
