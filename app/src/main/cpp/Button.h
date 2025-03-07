//
// Created by vrodin on 02.03.2025.
//

#ifndef VRODIN_S_SCRL_SHTER_BUTTON_H
#define VRODIN_S_SCRL_SHTER_BUTTON_H
#include "BaseObject.h"

class Button : public BaseObject {
public:
    Button(glm::vec2 position, glm::vec2 size, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

    void update(float deltaTime) override;
    void render() const override;
    void setStopPosition(glm::vec2 stopPosition);
    void setText(std::string text) { this->text = std::move(text); }
    std::string getText() { return this->text; }

private:
    glm::vec2 stopPosition;
    float maxSpeed = 200.0f;
    float acceleration = 100.0f;
    std::string text;
};
#endif //VRODIN_S_SCRL_SHTER_BUTTON_H
