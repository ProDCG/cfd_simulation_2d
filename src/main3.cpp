// #include <vector>
// #include <cmath>
// #include <algorithm>
// #include <string>
// #include <iostream>
// #include <random>
// #include <SFML/Graphics.hpp>

// struct Point {
//     double x, y;
//     Point(double x = 0, double y = 0) : x(x), y(y) {}

//     bool operator==(const Point& other) const {
//         return (x == other.x) && (y == other.y);
//     }
// };

// struct Triangle {
//     Point *p1, *p2, *p3;
//     Triangle(Point *p1, Point *p2, Point *p3) : p1(p1), p2(p2), p3(p3) {}

//     bool operator==(const Triangle& other) const {
//         return (p1 == other.p1 && p2 == other.p2 && p3 == other.p3) ||
//                (p1 == other.p2 && p2 == other.p3 && p3 == other.p1) ||
//                (p1 == other.p3 && p2 == other.p1 && p3 == other.p2);
//     }
// };

// class Delaunay {
// private:
//     std::vector<Triangle> triangles;

//     static double determinant(const Point& p1, const Point& p2, const Point& p3) {
//         return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
//     }

//     static bool inCircumcircle(const Point& p, const Triangle& t) {
//         double ax = t.p1->x - p.x, ay = t.p1->y - p.y;
//         double bx = t.p2->x - p.x, by = t.p2->y - p.y;
//         double cx = t.p3->x - p.x, cy = t.p3->y - p.y;

//         double det = (ax*ax + ay*ay) * (bx*cy - cx*by) -
//                      (bx*bx + by*by) * (ax*cy - cx*ay) +
//                      (cx*cx + cy*cy) * (ax*by - bx*ay);

//         return det > 0;
//     }

// public:
//     std::vector<Triangle> triangulate(std::vector<Point>& points) {
//         triangles.clear();
//         if (points.size() < 3) return triangles;

//         // Find the bounding rectangle
//         double minX = points[0].x, maxX = points[0].x;
//         double minY = points[0].y, maxY = points[0].y;
//         for (const auto& p : points) {
//             minX = std::min(minX, p.x);
//             maxX = std::max(maxX, p.x);
//             minY = std::min(minY, p.y);
//             maxY = std::max(maxY, p.y);
//         }
        
//         double dx = maxX - minX;
//         double dy = maxY - minY;
//         double dmax = std::max(dx, dy);
//         double midx = (minX + maxX) / 2.0;
//         double midy = (minY + maxY) / 2.0;

//         Point* p1 = new Point(midx - 20 * dmax, midy - dmax);
//         Point* p2 = new Point(midx, midy + 20 * dmax);
//         Point* p3 = new Point(midx + 20 * dmax, midy - dmax);

//         triangles.push_back(Triangle(p1, p2, p3));

//         for (auto& p : points) {
//             std::vector<Triangle> badTriangles;
//             std::vector<std::pair<Point*, Point*>> edges;

//             for (auto& t : triangles) {
//                 if (inCircumcircle(p, t)) {
//                     badTriangles.push_back(t);
//                     edges.emplace_back(t.p1, t.p2);
//                     edges.emplace_back(t.p2, t.p3);
//                     edges.emplace_back(t.p3, t.p1);
//                 }
//             }

//             triangles.erase(std::remove_if(triangles.begin(), triangles.end(),
//                 [&badTriangles](const Triangle& t) {
//                     return std::find(badTriangles.begin(), badTriangles.end(), t) != badTriangles.end();
//                 }), triangles.end());

//             for (auto it = edges.begin(); it != edges.end(); ) {
//                 auto jt = std::find(it + 1, edges.end(), std::make_pair(it->second, it->first));
//                 if (jt != edges.end()) {
//                     it = edges.erase(it);
//                     jt = edges.erase(jt);
//                 } else {
//                     ++it;
//                 }
//             }

//             for (const auto& e : edges) {
//                 triangles.emplace_back(e.first, e.second, &p);
//             }
//         }

//         // Remove triangles with super-triangle vertices
//         triangles.erase(std::remove_if(triangles.begin(), triangles.end(),
//             [p1, p2, p3](const Triangle& t) {
//                 return t.p1 == p1 || t.p2 == p1 || t.p3 == p1 ||
//                        t.p1 == p2 || t.p2 == p2 || t.p3 == p2 ||
//                        t.p1 == p3 || t.p2 == p3 || t.p3 == p3;
//             }), triangles.end());

//         delete p1;
//         delete p2;
//         delete p3;

//         return triangles;
//     }
// };



// int main() {
//     constexpr int32_t window_width = 800;
//     constexpr int32_t window_height = 800;
//     const uint32_t frame_rate = 60;

//     sf::ContextSettings settings;
//     settings.antialiasingLevel = 1;

//     sf::RenderWindow window(sf::VideoMode(window_width, window_height), "FEM Fluid Simulation", sf::Style::Default, settings);
//     window.setFramerateLimit(frame_rate);

//     // std::vector<Point> points = {{100, 100}, {200, 100}, {200, 200}, {100, 200}, {150, 200}};
    
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_real_distribution<> dis(0, 800);

//     // Generate 100 random points
//     std::vector<Point> points;
//     for (int i = 0; i < 100; ++i) {
//         double x = dis(gen);
//         double y = dis(gen);
//         points.emplace_back(x, y);
//     }

//     Delaunay delaunay;
//     std::vector<Triangle> triangulation = delaunay.triangulate(points);

//     std::cout << triangulation.size() << '\n';

//     sf::Clock clock;
//     while (window.isOpen()) {
//         sf::Event event;
//         while (window.pollEvent(event)) {
//             if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
//         }

//         window.clear(sf::Color::Black);

//         for (const auto& triangle : triangulation) {
//             sf::ConvexShape convex;
//             convex.setPointCount(3);

//             convex.setPoint(0, sf::Vector2f(triangle.p1->x, triangle.p1->y));
//             convex.setPoint(1, sf::Vector2f(triangle.p2->x, triangle.p2->y));
//             convex.setPoint(2, sf::Vector2f(triangle.p3->x, triangle.p3->y));

//             convex.setFillColor(sf::Color::Transparent);
//             convex.setOutlineColor(sf::Color::White);
//             convex.setOutlineThickness(1);

//             window.draw(convex);
//         }
//         // for (const auto& t : triangles) {
//         //     sf::ConvexShape convex;
//         //     convex.setPointCount(3);

//         //     convex.setPoint(0, sf::Vector2f(t.p1->x, t.p1->y));
//         //     convex.setPoint(1, sf::Vector2f(t.p2->x, t.p2->y));
//         //     convex.setPoint(2, sf::Vector2f(t.p3->x, t.p3->y));
//         //     convex.setFillColor(sf::Color::White);
//         //     // convex.setFillColor(sf::Color::Transparent);
//         //     // convex.setOutlineThickness(5);
//         //     // convex.setOutlineColor(sf::Color::White);
//         //     window.draw(convex);
//         // }

//         // sf::CircleShape circle;
//         // circle.setPosition(sf::Vector2f(100, 100));
//         // circle.setRadius(10);
//         // circle.setFillColor(sf::Color::Red);
//         // window.draw(circle);

//         // circle.setPosition(sf::Vector2f(120, 150));
//         // // circle.setFillColor(sf::Color::Red);
//         // window.draw(circle);
        
//         // circle.setPosition(sf::Vector2f(160, 130));
//         // // circle.setFillColor(sf::Color::Red);
//         // window.draw(circle);

//         window.display();

//     }
// }

