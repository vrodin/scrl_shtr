//
// Created by vrodin on 02.03.2025.
//

#ifndef VRODIN_S_SCRL_SHTER_HERO_H
#define VRODIN_S_SCRL_SHTER_HERO_H
#include "BaseObject.h"
#include "Bullet.h"
#include <vector>

class Hero : public BaseObject {
public:
    Hero(glm::vec2 position, glm::vec2 size);

    void update(float deltaTime) override;
    void render() const override;

    void fireBullet();
    void takeDamage(int damage);

    void setX(int x) {position.x = x;}
    std::vector<Bullet*>& getBullets();
    int getHealth() const;
    void setHealth(int health);

private:
    int health;
    float fireCooldown;
    float fireInterval;
    bool shouldRemove;
    std::vector<Bullet*> bullets;
};
#endif //VRODIN_S_SCRL_SHTER_HERO_H
