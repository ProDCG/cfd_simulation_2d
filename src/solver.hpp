#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float radius = 10.0f;
    float property = 0.0f;
    float density = 0.0f;
    float mass = 1.0f;
    sf::Color color = sf::Color::White;

    Particle() = default;
    explicit Particle(sf::Vector2f position_)
    :position{position_} {}
    Particle(sf::Vector2f position_, float radius_)
    :position{position_}, radius{radius_} {}

    void update(float dt) {
        sf::Vector2f cur_accel = sf::Vector2f(0.0f, 0);
        velocity += cur_accel * dt;
        position += velocity * dt;
    }
};

class Solver {
public:
    Solver() = default;

    void update() {

        for (auto& particle : s_particles) {
            particle.update(s_dt);
            particle.density = calculateDensity(particle.position);
        }

        // apply pressure forces
        for (auto& particle : s_particles) {
            sf::Vector2f pressureForce = calculatePressureForce(particle.position);
            sf::Vector2f pressureAcceleration = pressureForce / particle.density;
            particle.velocity = pressureAcceleration * s_dt;
        }

        checkCollisions();
    }

    void checkCollisions() {
        for (auto& particle : s_particles) {
            sf::Vector2f pos = particle.position;
            if (pos.x > 500 - particle.radius) {
                particle.position.x = 500 - particle.radius;
                particle.velocity.x *= -1 * s_collisionDamping;
            } else if (pos.x < 0 + particle.radius) {
                particle.position.x = 0 + particle.radius;
                particle.velocity.x *= -1 * s_collisionDamping;
            }

            if (pos.y > 500 - particle.radius) {
                particle.position.y = 500 - particle.radius;
                particle.velocity.y *= -1 * s_collisionDamping;
            } else if (pos.y < 0 + particle.radius) {
                particle.position.y = 0 + particle.radius;
                particle.velocity.y *= -1 * s_collisionDamping;
            }
        }
    }

    Particle& addParticle(sf::Vector2f position) {
        s_particles.emplace_back(position);
        return s_particles.back();
    }

    [[nodiscard]] const std::vector<Particle>& getParticles() const {
        return s_particles;
    }

    [[nodiscard]]
    uint32_t getParticleCount() const {
        return s_particles.size();
    }

    static float smoothingKernel(float radius, float dst) {
        float volume = M_PI * std::pow(radius, 8) / 4;
        float value = std::max(0.0f, radius * radius - dst * dst);
        return value * value * value / volume;
    }

    static float smoothingKernelDerivative(float radius, float dst) {
        if (dst >= radius) return 0;
        float f = radius * radius - dst * dst;
        float scale = -24.0f / (M_PI * std::pow(radius, 8));
        return scale * dst * f * f;
    }

    float calculateDensity(sf::Vector2f& point) {
        float density = 0.0f;

        for (auto& particle : s_particles) {
            float dst = magnitude(particle.position - point);
            float influence = smoothingKernel(s_smoothingRadius, dst);
            density += particle.mass * influence;
        }

        return density;
    }

    float calculateProperty(sf::Vector2f point) {
        float property = 0.0f;
        for (auto& particle : s_particles) {
            float dst = magnitude(particle.position - point);
            float influence = smoothingKernel(s_smoothingRadius, dst);
            float density = calculateDensity(particle.position);
            property += particle.property * influence * particle.mass / density;
        }
        return property;
    }

    sf::Vector2f calculatePressureForce(sf::Vector2f point) {
        sf::Vector2f pressureForce;

        for (auto& particle : s_particles) {
            if (particle.position == point) continue;
            float dst = magnitude(particle.position - point);

            sf::Vector2f dir = (particle.position - point) / dst;
            float slope = smoothingKernelDerivative(s_smoothingRadius, dst);
            float density = particle.density;
            pressureForce -= convertDensityToPressure(particle.density) * dir * slope * particle.mass / density;
        } // TODO flip if needed

        return pressureForce;
    }

    static float magnitude(sf::Vector2f vec) {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y);
    }

    float convertDensityToPressure(float density) {
        float densityError = density - s_targetDensity;
        float pressure = densityError * s_pressureMultiplier;
        return pressure;
    }

private:
    std::vector<Particle> s_particles;
    const float s_dt = (1.0f / 60.0f);
    const float s_collisionDamping = 0.7f;
    const float s_smoothingRadius = 100.0f;
    const float s_targetDensity = 10.0f;
    const float s_pressureMultiplier = 1.0f;
    const sf::Vector2f gravity = sf::Vector2f(0.0f, 0.0f);

};