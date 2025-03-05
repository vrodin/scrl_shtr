//
// Created by vrodin on 04.03.2025.
//
#include "Bomber.h"

Bomber::Bomber(glm::vec2 position, glm::vec2 size, glm::vec2 velocity)
        : BaseObject(position, size, velocity), health(4) {
}

void Bomber::update(float deltaTime) {
    setPosition(getPosition() + getVelocity() * deltaTime);

    if (getPosition().x + getSize().x < 0) {
        markForRemoval();
    }
}

void Bomber::render() const {
    // Отрисовка бомбардировщика (например, прямоугольник или спрайт)
    // Здесь можно использовать OpenGL для отрисовки примитивов
    // Например:
    // drawRectangle(getPosition(), getSize());
}

void Bomber::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        markForRemoval();
    }
}