//
// Created by vrodin on 04.03.2025.
//
#include "Bird.h"
#include <cmath>
#include <cstdlib>

Bird::Bird(glm::vec2 position, glm::vec2 size, glm::vec2 velocity)
        : BaseObject(position, size, velocity), time(0.0f) {
    initialX = position.x;
    frequency = 1.0f;
    immortal = true;
    amplitude = 20.0f + (float)(std::rand() % 30);
}

void Bird::update(float deltaTime) {
    time += deltaTime;

    float newX = initialX + amplitude * std::sin(frequency * time);

    setPosition(glm::vec2(newX, getPosition().y + getVelocity().y * deltaTime));

    if (getPosition().y + getSize().y < 0) {
        markForRemoval();
    }
}

void Bird::render() const {
    shader->activate();
    shader->drawModel(*model, (float) getPosition().x,
                      (float) getPosition().y);
    shader->deactivate();
}
