//
// Created by vrodin on 04.03.2025.
//
#include "Bullet.h"

Bullet::Bullet(glm::vec2 position, glm::vec2 size, glm::vec2 velocity)
        : BaseObject(position, size, velocity), damage(1) {
}

void Bullet::update(float deltaTime) {
    setPosition(getPosition() + getVelocity() * deltaTime);

    if (getPosition().x + getSize().x < 0 || getPosition().x > 800) {
        markForRemoval();
    }
}

void Bullet::render() const {
    // Отрисовка пули (например, прямоугольник или спрайт)
    // Здесь можно использовать OpenGL для отрисовки примитивов
    // Например:
    // drawRectangle(getPosition(), getSize());
}

int Bullet::getDamage() const {
    return damage;
}