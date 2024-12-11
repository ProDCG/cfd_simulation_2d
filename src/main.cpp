#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include "solver.hpp"
#include "renderer.hpp"

int main()
{
    constexpr int32_t window_width = 500;
    constexpr int32_t window_height = 500;
    const uint32_t frame_rate = 60;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "FEM Fluid Simulation", sf::Style::Default, settings);
    window.setFramerateLimit(frame_rate);

    Solver solver;
    Renderer renderer{window};

    unsigned int seed = 1;
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> distrib(0, window_width);

    for (int i = 0; i < 50; i++) {
        solver.addParticle(sf::Vector2f(distrib(gen), distrib(gen)), sf::Vector2f(0.0f, 0.0f), 0.0f);
    }

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
        }

        solver.update();
        window.clear(sf::Color::Black);
        renderer.render(solver);
        window.display();

        std::cout << "0 " << solver.getParticles()[0].x << std::endl;
//        break;
    }
}
