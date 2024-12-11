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
        for (auto& particle: particles) {
            circle.setPosition(particle.position);
            circle.setScale(particle.radius, particle.radius);
            circle.setFillColor(particle.color);
            m_target.draw(circle);
        }
    }

private:
    sf::RenderTarget& m_target;
};