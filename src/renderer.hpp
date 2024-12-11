#pragma once
#include "solver.hpp"
#include <SFML/Graphics.hpp>

class Renderer {
public:
    explicit Renderer(sf::RenderTarget& target):m_target { target} {}

    void render(const Solver& solver) const {
        sf::CircleShape circle{ 1.0f };
        circle.setOrigin(1.0f, 1.0f);

        const auto& particles = solver.getParticles();
        const auto& radius = solver.getRadius();

        for (int i = 0; i < solver.getParticleCount(); i++) {
            circle.setPosition(particles[i]);
            circle.setScale(radius, radius);
            circle.setFillColor(sf::Color::White);
            m_target.draw(circle);
        }
    }

private:
    sf::RenderTarget& m_target;
};