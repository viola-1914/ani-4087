#pragma once
// Module "geometrie" : les calculs sur les murs et les pieces de MaSalle.
namespace geometrie {
    struct Point { double x, y; };
    double Distance(Point a, Point b);
    double LongueurMur(Point debut, Point fin);
    double AirePolygone(const Point* sommets, int nombre);
    double Perimetre(const Point* sommets, int nombre);
    Point  Centre(const Point* sommets, int nombre);
}
