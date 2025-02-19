#include <vector>
#include <algorithm>
#include <cmath>

struct Point {
    double x, y;
    Point(double x = 0, double y = 0): x(x), y(y) {}

    bool equal(double _x, double _y, int ulp = 2) const {
        return fabs(_x - _y) <=  std::numeric_limits<double>::epsilon() * fabs(_x + _y) * static_cast<double>(ulp)
            || fabs(_x - _y) < std::numeric_limits<double>::min();
    }

    bool operator==(const Point& other) const {
        return equal(x, other.x) && equal(y, other.y);        
    }

    double norm2() const {
        return (x * x) + (y * y);
    }

    double dist2(const Point &other) const {
        const double dx = x - other.x;
        const double dy = y - other.y;
        return dx * dx + dy * dy;
    }

    double dist(const Point& other) const {
        return hypot(x - other.x, y - other.y);
    }
};

struct Triangle{
    bool isBad = false;
    Point *p1, *p2, *p3;
    Triangle(Point& p1, Point& p2, Point& p3):p1(&p1), p2(&p2), p3(&p3) {}

    bool operator==(const Triangle& other) const {
        return (p1 == other.p1 && p2 == other.p2 && p3 == other.p3) ||
               (p1 == other.p2 && p2 == other.p3 && p3 == other.p1) ||
               (p1 == other.p3 && p2 == other.p1 && p3 == other.p2);
    }

    bool hasPoint(const Point& other) {
        return (*p1 == other || *p2 == other || *p3 == other);
    }

    bool circumCircleContains(const Point& other) const {
        const double ab = p1->norm2();
        const double cd = p2->norm2();
        const double ef = p3->norm2();

        const double ax = p1->x;
        const double ay = p1->y;
        const double bx = p2->x;
        const double by = p2->y;
        const double cx = p3->x;
        const double cy = p3->y;

        const double circum_x = (ab * (cy - by) + cd * (ay - cy) + ef * (by - ay)) / (ax * (cy - by) + bx * (ay - cy) + cx * (by - ay));
        const double circum_y = (ab * (cx - bx) + cd * (ax - cx) + ef * (bx - ax)) / (ay * (cx - bx) + by * (ax - cx) + cy * (bx - ax));

        const Point circum(circum_x / 2, circum_y / 2);
        const double circum_radius = p1->dist2(circum);
        const double dist = other.dist2(circum);
        return dist <= circum_radius;
    }
};

struct Edge {
    bool isBad = false;
    Point *p1, *p2;

    Edge(Point& p1, Point& p2): p1(&p1), p2(&p2) {}
};

class DelaunayTriangulation {
private:
    std::vector<Point> points;
    std::vector<Triangle> triangles;
    std::vector<Edge> edges;
    // std::vector<Edge*> edges;

    bool equal(const Edge& e1, const Edge& e2) const {
        return (e1.p1 == e2.p1 && e1.p2 == e2.p2);
    }

public:
    void addPoint(double x, double y) {
        points.push_back(Point(x, y));
    }

    void delaunayTriangulate() {
        if (points.size() < 3) return;

        // std::vector<Triangle> triangles;
        
        // Generates the super triangle
        double minX = points[0].x;
        double minY = points[0].y;
        double maxX = minX;
        double maxY = minY;

        for (size_t i = 0; i < points.size(); ++i) {
            if (points[i].x < minX) minX = points[i].x;
            if (points[i].y < minY) minY = points[i].y;
            if (points[i].x > maxX) maxX = points[i].x;
            if (points[i].y > maxY) maxY = points[i].y;
        }

        double dx = maxX - minX;
        double dy = maxY - minY;
        double deltaMax = std::max(dx, dy);
        double midx = (minX + maxX) / 2.0;
        double midy = (minY + maxY) / 2.0;

        Point p1 = Point(midx - 20 * deltaMax, midy - deltaMax);
        Point p2 = Point(midx, midy + 20 * deltaMax);
        Point p3 = Point(midx + 20 * deltaMax, midy - deltaMax);

        // Add the super triangle
        triangles.push_back(Triangle(p1, p2, p3));

        for (auto i = begin(points); i != end(points); i++) {
            
            std::vector<Edge> polygon;

            for (auto & t : triangles) {
                if (t.circumCircleContains(*i)) {
                    t.isBad = true;
                    polygon.push_back(Edge(*t.p1, *t.p2));
                    polygon.push_back(Edge(*t.p2, *t.p3));
                    polygon.push_back(Edge(*t.p3, *t.p1));
                }
            }

            triangles.erase(std::remove_if(begin(triangles), end(triangles), [](Triangle &t) {
                return t.isBad;
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
                triangles.push_back(Triangle(*e.p1, *e.p2, *i));
            }
        }

        triangles.erase(std::remove_if(begin(triangles), end(triangles), [p1, p2, p3](Triangle &t) {
            return (t.hasPoint(p1) || t.hasPoint(p2) || t.hasPoint(p3));
        }), end(triangles));

        for (const auto t : triangles) {
            edges.push_back(Edge(*t.p1, *t.p2));
            edges.push_back(Edge(*t.p2, *t.p3));
            edges.push_back(Edge(*t.p3, *t.p1));
        }

        // done with triangulation
    }

    std::vector<Triangle> getTriangles() const {
        return triangles;
    }

    std::vector<Edge> getEdges() const {
        return edges;
    }

    std::vector<Point> getPoints() const {
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