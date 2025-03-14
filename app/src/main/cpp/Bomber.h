//
// Created by vrodin on 02.03.2025.
//

#ifndef VRODIN_S_SCRL_SHTER_BOMBER_H
#define VRODIN_S_SCRL_SHTER_BOMBER_H
#include "BaseObject.h"

class Bomber : public BaseObject {
public:
    Bomber(glm::vec2 position, glm::vec2 size, glm::vec2 velocity = glm::vec2(0.0f, -100.0f));

    void update(float deltaTime) override;
    void render() const override;

    void takeDamage(int damage);

private:
    int health;
};
#endif //VRODIN_S_SCRL_SHTER_BOMBER_H
