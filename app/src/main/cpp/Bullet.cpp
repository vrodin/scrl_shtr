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
    shader->activate();
    shader->drawModel(*model, (float) getPosition().x,
                       (float) getPosition().y);
    shader->deactivate();
}

int Bullet::getDamage() const {
    return damage;
}