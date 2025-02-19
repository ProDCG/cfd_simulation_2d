#include <vector>
#include <algorithm>
#include <cmath>

struct Point {
    int x, y;
    Point(int x = 0, int y = 0): x(x), y(y) {}

    bool operator==(const Point& other) const {
        return (x == other.x && y == other.y);
    }
};

struct Triangle{
    bool isBad = false;
    Point *p1, *p2, *p3;
    Triangle(Point *p1, Point *p2, Point *p3):p1(p1), p2(p2), p3(p3) {}

    bool operator==(const Triangle& other) const {
        return (p1 == other.p1 && p2 == other.p2 && p3 == other.p3) ||
               (p1 == other.p2 && p2 == other.p3 && p3 == other.p1) ||
               (p1 == other.p3 && p2 == other.p1 && p3 == other.p2);
    }

    bool hasPoint(const Point& other) {
        return (*p1 == other || *p2 == other || *p3 == other);
    }
};

struct Edge {
    bool isBad = false;
    Point *p1, *p2;

    Edge(Point *p1, Point *p2): p1(p1), p2(p2) {}
};

class DelaunayTriangulation {
private:
    std::vector<Point*> points;
    std::vector<Triangle*> triangles;
    std::vector<Edge*> edges;
    // std::vector<Edge*> edges;

    bool equal(const Edge& e1, const Edge& e2) const {
        return (e1.p1 == e2.p1 && e1.p2 == e2.p2);
    }

    bool circumCircleContains(const Triangle& t, const Point& p) {
        double ab = (t.p1->x * t.p1->x) + (t.p1->y * t.p1->y);
        double cd = (t.p2->x * t.p2->x) + (t.p2->y * t.p2->y);
        double ef = (t.p3->x * t.p3->x) + (t.p3->y * t.p3->y);

        double circum_x = (ab * (t.p3->y - t.p2->y) + cd * (t.p1->y - t.p3->y) + ef * (t.p2->y - t.p1->y)) /
                          (t.p1->x * (t.p3->y - t.p2->y) + t.p2->x * (t.p1->y - t.p3->y) + t.p3->x * (t.p2->y - t.p1->y)) / 2.0;
        double circum_y = (ab * (t.p3->x - t.p2->x) + cd * (t.p1->x - t.p3->x) + ef * (t.p2->x - t.p1->x)) /
                          (t.p1->y * (t.p3->x - t.p2->x) + t.p2->y * (t.p1->x - t.p3->x) + t.p3->y * (t.p2->x - t.p1->x)) / 2.0;
        double circum_radius = sqrt(((circum_x - t.p1->x) * (circum_x - t.p1->x)) +
                                    ((circum_y - t.p1->y) * (circum_y - t.p1->y)));

        double dist = sqrt(((p.x - circum_x) * (p.x - circum_x)) +
                           ((p.y - circum_y) * (p.y - circum_y)));
        return dist <= circum_radius;
    }

public:
    void addPoint(double x, double y) {
        points.push_back(new Point(x, y));
    }

    void delaunayTriangulate() {
        if (points.size() < 3) return;

        // std::vector<Triangle> triangles;
        
        // Generates the super triangle
        double minX = points[0]->x;
        double minY = points[0]->y;
        double maxX = minX;
        double maxY = minY;

        for (size_t i = 0; i < points.size(); ++i) {
            if (points[i]->x < minX) minX = points[i]->x;
            if (points[i]->y < minY) minY = points[i]->y;
            if (points[i]->x > maxX) maxX = points[i]->x;
            if (points[i]->y > maxY) maxY = points[i]->y;
        }

        double dx = maxX - minX;
        double dy = maxY - minY;
        double deltaMax = std::max(dx, dy);
        double midx = (minX + maxX) / 2.0;
        double midy = (minY + maxY) / 2.0;

        Point *p1 = new Point(midx - 20 * deltaMax, midy - deltaMax);
        Point *p2 = new Point(midx, midy + 20 * deltaMax);
        Point *p3 = new Point(midx + 20 * deltaMax, midy - deltaMax);

        // Add the super triangle
        triangles.push_back(new Triangle(p1, p2, p3));

        for (auto i = begin(points); i != end(points); i++) {
            
            std::vector<Edge> polygon;

            for (auto& t : triangles) {
                if (circumCircleContains(*t, **i)) {
                    t->isBad = true;
                    polygon.push_back(Edge(t->p1, t->p2));
                    polygon.push_back(Edge(t->p2, t->p3));
                    polygon.push_back(Edge(t->p3, t->p1));
                }
            }

            triangles.erase(std::remove_if(begin(triangles), end(triangles), [](Triangle *t) {
                return t->isBad;
            }), end(triangles));

            for (auto e1 = begin(polygon); e1 != end(polygon); ++e1) {
                for (auto e2 = e1 + 1; e2 != end(polygon); ++e2) {
                    
                    // REVISIT ALMOST EQUAL
                    if (equal(*e1, *e2)) {
                        e1->isBad = true;
                        e2->isBad = true;
                    }
                }
            }

            polygon.erase(std::remove_if(begin(polygon), end(polygon), [](Edge &e) {
                return e.isBad;
            }), end(polygon));

            for (const auto e : polygon) {
                triangles.push_back(new Triangle(e.p1, e.p2, *i));
            }
        }

        triangles.erase(std::remove_if(begin(triangles), end(triangles), [p1, p2, p3](Triangle *t) {
            return (t->hasPoint(*p1) || t->hasPoint(*p2) || t->hasPoint(*p3));
        }), end(triangles));

        for (const auto t : triangles) {
            edges.push_back(new Edge(t->p1, t->p2));
            edges.push_back(new Edge(t->p2, t->p3));
            edges.push_back(new Edge(t->p3, t->p1));
        }

        // done with triangulation
    }

    std::vector<Triangle*> getTriangles() {
        return triangles;
    }

    std::vector<Edge*> getEdges() {
        return edges;
    }

    std::vector<Point*> getPoints() {
        return points;
    }

    // void generateStructure() {
    //     if (points.size() < 3) return;

    //     double minX = points[0]->x;
    //     double minY = points[0]->y;
    //     double maxX = minX;
    //     double maxY = minY;

    //     for (size_t i = 0; i < points.size(); ++i) {
    //         if (points[i]->x < minX) minX = points[i]->x;
    //         if (points[i]->y < minY) minY = points[i]->y;
    //         if (points[i]->x > maxX) maxX = points[i]->x;
    //         if (points[i]->y > maxY) maxY = points[i]->y;
    //     }

    //     double dx = maxX - minX;
    //     double dy = maxY - minY;
    //     double deltaMax = std::max(dx, dy);
    //     double midx = (minX + maxX) / 2.0;
    //     double midy = (minY + maxY) / 2.0;

    //     Point *p1 = new Point(midx - 20 * deltaMax, midy - deltaMax);
    //     Point *p2 = new Point(midx, midy + 20 * deltaMax);
    //     Point *p3 = new Point(midx + 20 * deltaMax, midy - deltaMax);

    //     triangles.push_back(Triangle(p1, p2, p3));

    //     for (size_t i = 0; i < points.size(); ++i) {
    //         std::vector<Triangle> badTriangles;
    //         for (size_t j = 0; j < triangles.size(); ++j) {
    //             if (circumCircleContains(triangles[j], *points[i])) {
    //                 badTriangles.push_back(triangles[j]);
    //             }
    //         }

    //         std::vector<std::pair<Point*, Point*>> polygon;
    //         for (size_t j = 0; j < badTriangles.size(); ++j) {
    //             for (size_t k = j + 1; k < badTriangles.size(); ++k) {
    //                 bool shareEdge = false;
    //                 if ((badTriangles[j].p1 == badTriangles[k].p1 && badTriangles[j].p2 == badTriangles[k].p2) ||
    //                     (badTriangles[j].p1 == badTriangles[k].p2 && badTriangles[j].p2 == badTriangles[k].p1)) {
    //                     shareEdge = true;
    //                 } else if ((badTriangles[j].p2 == badTriangles[k].p2 && badTriangles[j].p3 == badTriangles[k].p3) ||
    //                            (badTriangles[j].p2 == badTriangles[k].p3 && badTriangles[j].p3 == badTriangles[k].p2)) {
    //                     shareEdge = true;
    //                 } else if ((badTriangles[j].p3 == badTriangles[k].p3 && badTriangles[j].p1 == badTriangles[k].p1) ||
    //                            (badTriangles[j].p3 == badTriangles[k].p1 && badTriangles[j].p1 == badTriangles[k].p3)) {
    //                     shareEdge = true;
    //                 }

    //                 if (!shareEdge) {
    //                     polygon.push_back(std::make_pair(badTriangles[j].p1, badTriangles[j].p2));
    //                     polygon.push_back(std::make_pair(badTriangles[j].p2, badTriangles[j].p3));
    //                     polygon.push_back(std::make_pair(badTriangles[j].p3, badTriangles[j].p1));
    //                 }
    //             }
    //         }

    //         for (size_t j = 0; j < badTriangles.size(); ++j) {
    //             triangles.erase(std::remove(triangles.begin(), triangles.end(), badTriangles[j]), triangles.end());
    //         }

    //         for (size_t j = 0; j < polygon.size(); ++j) {
    //             triangles.push_back(Triangle(polygon[j].first, polygon[j].second, points[i]));
    //         }
    //     }

    //     triangles.erase(std::remove_if(triangles.begin(), triangles.end(),
    //         [p1, p2, p3](const Triangle& t) {
    //             return t.p1 == p1 || t.p2 == p1 || t.p3 == p1 ||
    //                    t.p1 == p2 || t.p2 == p2 || t.p3 == p2 ||
    //                    t.p1 == p3 || t.p2 == p3 || t.p3 == p3;
    //         }), triangles.end());

    //     delete p1;
    //     delete p2;
    //     delete p3;
    // }

    // const std::vector<Triangle*> getTriangles() const {
    //     return triangles;
    // }
};