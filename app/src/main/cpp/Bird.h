//
// Created by vrodin on 02.03.2025.
//

#ifndef VRODIN_S_SCRL_SHTER_BIRD_H
#define VRODIN_S_SCRL_SHTER_BIRD_H
#include "BaseObject.h"

class Bird : public BaseObject {
public:
    Bird(glm::vec2 position, glm::vec2 size, glm::vec2 velocity = glm::vec2(0.0f, -50.0f));

    void update(float deltaTime) override;
    void render() const override;

private:
    float time;
    float initialX;
    float frequency;
    bool shouldRemove;
    float amplitude;
};
#endif //VRODIN_S_SCRL_SHTER_BIRD_H
