//
// Created by vrodin on 02.03.2025.
//

#ifndef VRODIN_S_SCRL_SHTER_FIGHTER_H
#define VRODIN_S_SCRL_SHTER_FIGHTER_H
#include "BaseObject.h"
#include "Bullet.h"
#include <list>

class Fighter : public BaseObject {
public:
    Fighter(glm::vec2 position, glm::vec2 size, glm::vec2 velocity = glm::vec2(-150.0f, 0.0f));

    void update(float deltaTime) override;
    void render() const override;

    void fireBullet();
    void takeDamage(int damage);

    std::list<Bullet*>& getBullets();

private:
    int health;
    float fireCooldown;
    float fireInterval;
    bool shouldRemove;
    std::list<Bullet*> bullets;
};
#endif //VRODIN_S_SCRL_SHTER_FIGHTER_H
