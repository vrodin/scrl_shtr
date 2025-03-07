//
// Created by vrodin on 04.03.2025.
//
#include "Button.h"
#include <cmath>
#include <cstdlib>

Button::Button(glm::vec2 position, glm::vec2 size, glm::vec2 velocity)
        : BaseObject(position, size, velocity) {
}

void Button::update(float deltaTime) {

    if(stopPosition == getPosition()) return;

    glm::vec2 direction = stopPosition - getPosition();

    if (glm::length(direction) < 1.0f) {
        setPosition(stopPosition);
        return;
    }

    direction = glm::normalize(direction);
    glm::vec2 velocity = direction * 700.0f * deltaTime;

    setPosition(getPosition() + velocity);

}

void Button::render() const {
    // Отрисовка кнопки (например, треугольник или спрайт)
    // Здесь можно использовать OpenGL для отрисовки примитивов
    // Например:
    // drawTriangle(getPosition(), getSize());
}

void Button::setStopPosition(glm::vec2 stopPosition)
{
    this->stopPosition = stopPosition;
}
