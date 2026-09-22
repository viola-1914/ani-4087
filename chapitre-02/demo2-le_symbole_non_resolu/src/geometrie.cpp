#include "geometrie.h"
#include <cmath>
namespace geometrie {
double Distance(Point a, Point b) { return std::hypot(b.x - a.x, b.y - a.y); }
double LongueurMur(Point debut, Point fin) { return Distance(debut, fin); }
double AirePolygone(const Point* s, int n) {
    double a = 0; for (int i = 0; i < n; ++i) { const Point& p = s[i]; const Point& q = s[(i + 1) % n]; a += p.x * q.y - q.x * p.y; }
    return std::fabs(a) / 2;
}
double Perimetre(const Point* s, int n) { double p = 0; for (int i = 0; i < n; ++i) p += Distance(s[i], s[(i + 1) % n]); return p; }
Point Centre(const Point* s, int n) { Point c{0, 0}; for (int i = 0; i < n; ++i) { c.x += s[i].x; c.y += s[i].y; } c.x /= n; c.y /= n; return c; }
}
