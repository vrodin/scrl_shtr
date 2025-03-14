//
// Created by vrodin on 02.03.2025.
//

#ifndef VRODIN_S_SCRL_SHTER_FLIGHTBULLET_H
#define VRODIN_S_SCRL_SHTER_FLIGHTBULLET_H
#include "BaseObject.h"

class Bullet : public BaseObject {
public:
    Bullet(glm::vec2 position, glm::vec2 size, glm::vec2 velocity);

    void update(float deltaTime) override;
    void render() const override;

    int getDamage() const;

private:
    int damage;
};

#endif //VRODIN_S_SCRL_SHTER_FLIGHTBULLET_H
