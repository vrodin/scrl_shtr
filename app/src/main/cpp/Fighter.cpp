//
// Created by vrodin on 04.03.2025.
//
#include "Fighter.h"

Fighter::Fighter(glm::vec2 position, glm::vec2 size, glm::vec2 velocity)
        : BaseObject(position, size, velocity), health(2), fireCooldown(0.0f) {
    fireInterval = 2.0f;
}

void Fighter::update(float deltaTime) {
    for(auto &bullet : getBullets())
        bullet->update(deltaTime);

    setPosition(getPosition() + getVelocity() * deltaTime);

    fireCooldown -= deltaTime;
    if (fireCooldown <= 0.0f) {
        fireBullet();
        fireCooldown = fireInterval;
    }

    if (getPosition().y + getSize().y < 0) {
        markForRemoval();
    }
}

void Fighter::render() const {
    // Отрисовка истребителя (например, прямоугольник или спрайт)
    // Здесь можно использовать OpenGL для отрисовки примитивов
    // Например:
    // drawRectangle(getPosition(), getSize());
}

void Fighter::fireBullet() {
    glm::vec2 bulletPosition = getPosition() + glm::vec2( getSize().x / 2 , 0);
    glm::vec2 bulletSize = glm::vec2(5, 10);
    glm::vec2 bulletVelocity = glm::vec2(0.0f, -200.0f);

    bullets.push_back(new Bullet(bulletPosition, bulletSize, bulletVelocity));
}

void Fighter::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        markForRemoval();
    }
}

std::list<Bullet*>& Fighter::getBullets() {
    return bullets;
}