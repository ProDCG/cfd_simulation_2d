#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>

class Solver {
public:
    Solver() = default;

    void update() {
        std::cout << "1 " << getParticles()[0].x << std::endl;
        for (int i = 0; i < s_positions.size(); i++) {
            s_velocities[i] -= gravity * (s_dt);
            s_densities[i] = calculateDensity(i);

        }

        std::cout << "1.1 " << s_densities[0] << std::endl;

        std::cout << "2 " << getParticles()[1].x << std::endl;

        for (int i = 0; i < s_positions.size(); i++) {
            sf::Vector2f pressureForce = calculatePressureForce(i);
            sf::Vector2f pressureAcceleration = pressureForce / s_densities[i];
            s_velocities[i] += pressureAcceleration * s_dt;

        }

        std::cout << "3 " << getParticles()[0].x << std::endl;

//        for (int i = 0; i < s_positions.size(); i++) {
//            sf::Vector2f pressureForce = calculatePressureForce(i);
//            sf::Vector2f pressureAcceleration = pressureForce / s_densities[i];
//            s_velocities[i] += pressureAcceleration * s_dt;
//        }

        for (int i = 0; i < s_positions.size(); i++) {
            s_positions[i] += s_velocities[i] * s_dt;
            checkCollisions();
        }

        std::cout << "4 " << getParticles()[0].x << std::endl;

//        for (auto& particle : s_particles) {
//            particle.update(s_dt);
//            particle.density = calculateDensity(particle.position);
//        }
//ce
//        // apply pressure forces
//        for (auto& particle : s_particles) {
//            sf::Vector2f pressureForce = calculatePressureForce(particle.position);
//            sf::Vector2f pressureAcceleration = pressureForce / particle.density;
//            particle.velocity = pressureAcceleration * s_dt;
//        }
//
//        checkCollisions();
    }

    void checkCollisions() {
        sf::Vector2f halfBoundsSize = sf::Vector2f(500.0f, 500.0f) / 2.0f - sf::Vector2f(1.0f, 1.0f) * s_particleSize;

        for (int i = 0; i < s_positions.size(); i++) {
             if (s_positions[i].x > 500 - s_particleSize) {
                 s_positions[i].x = 500 - s_particleSize;
                 s_velocities[i].x *= -1 * s_collisionDamping;
             }

             if (s_positions[i].x < 0 + s_particleSize) {
                 s_positions[i].x = 0 + s_particleSize;
                 s_velocities[i].x *= -1 * s_collisionDamping;
             }

             if (s_positions[i].y > 500 - s_particleSize) {
                 s_positions[i].y = 500 - s_particleSize;
                 s_velocities[i].y *= -1 * s_collisionDamping;
             }

             if (s_positions[i].y < 0 + s_particleSize) {
                 s_positions[i].y = 0 + s_particleSize;
                 s_velocities[i].y *= -1 * s_collisionDamping;
             }
        }
    }

    sf::Vector2f& addParticle(sf::Vector2f position, sf::Vector2f velocity, float property) {
        s_positions.emplace_back(position);
        s_velocities.emplace_back(velocity);
        s_densities.emplace_back(property);
        s_properties.emplace_back(0.0f);
        return s_positions.back();
    }

    [[nodiscard]] std::vector<sf::Vector2f> getParticles() const {
        return s_velocities;
    }

    [[nodiscard]]
    uint32_t getParticleCount() const {
        return s_positions.size();
    }

    [[nodiscard]] float getRadius() const {
        return s_particleSize;
    }

    static float smoothingKernel(float radius, float dst) {
        float volume = M_PI * std::pow(radius, 8) / 4;
        float value = std::max(0.0f, radius * radius - dst * dst);
        return value * value * value / volume;
    }
//
    static float smoothingKernelDerivative(float radius, float dst) {
        if (dst >= radius) return 0;
        float f = radius * radius - dst * dst;
        float scale = -24.0f / (M_PI * std::pow(radius, 8));
        return scale * dst * f * f;
    }
//
    float calculateDensity(int index) {
        float density = 0.0f;
        float mass = 1.0f;

        for (int i = 0; i < s_positions.size(); i++) {
            if (i == index) continue;

            float dst = magnitude(s_positions[i] - s_positions[index]);
            float influence = smoothingKernel(s_smoothingRadius, dst);
            density += mass * influence;
        }

        return density;
    }

    float calculateProperty(sf::Vector2f samplePos) {
        float property = 0.0f;
        float mass = 1.0f;

        for (int i = 0; i < s_positions.size(); i++) {
            float dst = magnitude(s_positions[i] - samplePos);
            float influence = smoothingKernel(s_smoothingRadius, dst);
            float density = s_densities[i];
            property += s_densities[i] * influence * mass / density;
        }

        return property;
    }

    sf::Vector2f calculatePressureForce(int index) {
        sf::Vector2f pressureForce(0.0f, 0.0f);
        float mass = 1.0f;

        for (int i = 0; i < s_positions.size(); i++) {
            if (index == i) continue;

            sf::Vector2f offset = s_positions[i] - s_positions[index];
            float dst = magnitude(offset);

            sf::Vector2f dir = offset / dst;
            float slope = smoothingKernelDerivative(s_smoothingRadius, dst);
            float density = s_densities[i];
            pressureForce += convertDensityToPressure(density) * dir * slope * mass / density;
        }

        return pressureForce;
    }
//
//    sf::Vector2f calculatePressureForce(sf::Vector2f point) {
//        sf::Vector2f pressureForce;
//
//        for (auto& particle : s_particles) {
//            if (particle.position == point) continue;
//            float dst = magnitude(particle.position - point);
//
//            sf::Vector2f dir = (particle.position - point) / dst;
//            float slope = smoothingKernelDerivative(s_smoothingRadius, dst);
//            float density = particle.density;
//            pressureForce -= convertDensityToPressure(particle.density) * dir * slope * particle.mass / density;
//        } // TODO flip if needed
//
//        return pressureForce;
//    }
//
    static float magnitude(sf::Vector2f vec) {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y);
    }
//
    float convertDensityToPressure(float density) {
        float densityError = density - s_targetDensity;
        float pressure = densityError * s_pressureMultiplier;
        return pressure;
    }

private:
//    std::vector<Particle> s_particles;
    std::vector<sf::Vector2f> s_positions;
    std::vector<sf::Vector2f> s_velocities;
    std::vector<float> s_properties;
    std::vector<float> s_densities;
    const float s_particleSize = 10.0f;
//    const uint32_t s_particleCount = 100;
//    sf::Vector2f s_particlePositions[100];
//    sf::Vector2f s_particleVelocities[100];
//    sf::Vector2f s_particleDensities[100];
    const float s_dt = (1.0f / 60.0f);
    const float s_collisionDamping = 0.7f;
    const float s_smoothingRadius = 300.0f;
    const float s_targetDensity = 1.0f;
    const float s_pressureMultiplier = 1.0f;
    const float s_mass = 1.0f;
    const sf::Vector2f gravity = sf::Vector2f(0.0f, -250.0f);
};