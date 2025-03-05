//
// Created by vrodin on 04.03.2025.
//
#include "Hero.h"
#include <algorithm>

Hero::Hero(glm::vec2 position, glm::vec2 size)
        : BaseObject(position, size, glm::vec2(0.0f)), health(1), fireCooldown(0.0f) {
    fireInterval = 0.5f;
}

void Hero::update(float deltaTime) {
    for(auto &bullet : getBullets())
        bullet->update(deltaTime);

    fireCooldown -= deltaTime;
    if (fireCooldown <= 0.0f) {
        fireBullet();
        fireCooldown = fireInterval;
    }

    float screenHeight = 600.0f;
    position.y = std::max(0.0f, std::min(screenHeight - size.y, position.y));
}

void Hero::render() const {
    // Отрисовка игрока (например, прямоугольник или спрайт)
    // Здесь можно использовать OpenGL для отрисовки примитивов
    // Например:
    // drawRectangle(getPosition(), getSize());
}

void Hero::fireBullet() {
    if (fireCooldown <= 0.0f) {
        glm::vec2 bulletPosition = getPosition() + glm::vec2(getSize().x/2, getSize().y);
        glm::vec2 bulletSize = glm::vec2(10, 5);
        glm::vec2 bulletVelocity = glm::vec2(0.0f, 200.0f);

        bullets.push_back(new Bullet(bulletPosition, bulletSize, bulletVelocity));

        fireCooldown = fireInterval;
    }
}

void Hero::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        markForRemoval();
    }
}

std::vector<Bullet*>& Hero::getBullets() {
    return bullets;
}

int Hero::getHealth() const {
    return health;
}

void Hero::setHealth(int newHealth) {
    health = newHealth;
}