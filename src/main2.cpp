#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include "renderer.hpp"
#include "triangulation.hpp"

int main()
{
    constexpr int32_t window_width = 800;
    constexpr int32_t window_height = 800;
    const uint32_t frame_rate = 60;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "FEM Fluid Simulation", sf::Style::Default, settings);
    window.setFramerateLimit(frame_rate);

    std::default_random_engine eng(std::random_device{}());
    std::uniform_int_distribution dist_x(0, 800);
    std::uniform_int_distribution dist_y(0, 800);

    DelaunayTriangulation delaunay;
    
    for (int i = 0; i < 45; i++) {
        delaunay.addPoint(dist_x(eng), dist_y(eng));
    }

    delaunay.delaunayTriangulate();
    // auto triangles = delaunay.getTriangles();
    auto edges = delaunay.getEdges();

    std::cout << "Edge Count: " << edges.size() << '\n';
    // std::cout << "Edge Count: " << delaunay.getEdges().size() << '\n';
    std::cout << "Point Count: "<< delaunay.getPoints().size() << '\n';

    // return -1;
    // DelaunayTriangulation dt;

    // dt.addPoint(100, 100);
    // dt.addPoint(120, 150);
    // dt.addPoint(160, 130);

    // dt.generateStructure();

    window.clear(sf::Color::Black);

    for (const auto& e : delaunay.getEdges()) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(
                e.p1->x,
                e.p1->y)),
            sf::Vertex(sf::Vector2f(
                e.p2->x,
                e.p2->y))
        };

        window.draw(line, 2, sf::Lines);
    }

    // for (auto& triangle : triangles) {
    //     sf::ConvexShape convex;
    //     convex.setPointCount(3);

    //     // std::cout << triangle.p1->x << '\n';
    //     // std::cout << triangle.p1->y << '\n'; 

    //     convex.setPoint(0, sf::Vector2f(triangle.p1->x, triangle.p1->y));
    //     convex.setPoint(1, sf::Vector2f(triangle.p2->x, triangle.p2->y));
    //     convex.setPoint(2, sf::Vector2f(triangle.p3->x, triangle.p3->y));

    //     convex.setFillColor(sf::Color::Transparent);
    //     convex.setOutlineColor(sf::Color::White);
    //     convex.setOutlineThickness(1);

    //     window.draw(convex);
    // }

    for (auto& point : delaunay.getPoints()) {
        sf::CircleShape circle;
        circle.setPosition(sf::Vector2f(point.x, point.y));
        circle.setFillColor(sf::Color::Red);
        circle.setRadius(5);
        window.draw(circle);
    }

    window.display();

    while (window.isOpen()) {
        sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
    }

    return 0;

    // sf::Clock clock;
    // while (window.isOpen()) {
    //     sf::Event event;
    //     while (window.pollEvent(event)) {
    //         if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
    //     }

        // window.clear(sf::Color::Black);
        // // const auto& triangles = dt.getTriangles();
        // for (const auto& t : triangles) {
        //     sf::ConvexShape convex;
        //     convex.setPointCount(3);

        //     convex.setPoint(0, sf::Vector2f(t.p1->x, t.p1->y));
        //     convex.setPoint(1, sf::Vector2f(t.p2->x, t.p2->y));
        //     convex.setPoint(2, sf::Vector2f(t.p3->x, t.p3->y));
        //     convex.setFillColor(sf::Color::White);
        //     // convex.setFillColor(sf::Color::Transparent);
        //     // convex.setOutlineThickness(5);
        //     // convex.setOutlineColor(sf::Color::White);
        //     window.draw(convex);
        // }

        // sf::CircleShape circle;
        // circle.setPosition(sf::Vector2f(100, 100));
        // circle.setRadius(10);
        // circle.setFillColor(sf::Color::Red);
        // window.draw(circle);

        // circle.setPosition(sf::Vector2f(120, 150));
        // // circle.setFillColor(sf::Color::Red);
        // window.draw(circle);
        
        // circle.setPosition(sf::Vector2f(160, 130));
        // // circle.setFillColor(sf::Color::Red);
        // window.draw(circle);

        // window.display();

    }