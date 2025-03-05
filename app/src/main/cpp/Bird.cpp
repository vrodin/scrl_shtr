//
// Created by vrodin on 04.03.2025.
//
#include "Bird.h"
#include <cmath>
#include <cstdlib>

Bird::Bird(glm::vec2 position, glm::vec2 size, glm::vec2 velocity)
        : BaseObject(position, size, velocity), time(0.0f) {
    initialX = position.y;
    frequency = 1.0f;
}

void Bird::update(float deltaTime) {
    time += deltaTime;

    float newX = initialX + (20.0f + (float)(std::rand() % 30)) * std::sin(frequency * time);

    setPosition(glm::vec2(getPosition().y + getVelocity().y * deltaTime, newX));

    if (getPosition().y + getSize().y < 0) {
        markForRemoval();
    }
}

void Bird::render() const {
    // Отрисовка птицы (например, треугольник или спрайт)
    // Здесь можно использовать OpenGL для отрисовки примитивов
    // Например:
    // drawTriangle(getPosition(), getSize());
}
