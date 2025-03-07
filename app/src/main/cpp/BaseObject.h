#ifndef VRODIN_S_SCRL_SHTER_BASEOBJECT_H
#define VRODIN_S_SCRL_SHTER_BASEOBJECT_H

#include <glm/glm.hpp>
#include "Model.h"

class BaseObject {
public:
    BaseObject(glm::vec2 position, glm::vec2 size, glm::vec2 velocity = glm::vec2(0.0f))
            : position(position), size(size), velocity(velocity) {}

    virtual ~BaseObject() {}

    glm::vec2 getPosition() const { return position; }

    void setPosition(glm::vec2 newPosition) { position = newPosition; }

    glm::vec2 getSize() const { return size; }

    void setSize(glm::vec2 newSize) { size = newSize; }

    glm::vec2 getVelocity() const { return velocity; }

    void setVelocity(glm::vec2 newVelocity) { velocity = newVelocity; }

    virtual void update(float deltaTime) {}

    virtual void render() const {}

    void markForRemoval() { shouldRemove = true;}

    bool shouldBeRemoved() const { return shouldRemove; }

    void setModel(std::shared_ptr<Model> mod) {model = mod;}

    Model* getModel() {return model.get();}

    bool isImmortal() {return immortal;}

protected:
    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 velocity;
    bool shouldRemove;
    std::shared_ptr<Model> model;
    bool immortal = false;
};

#endif // VRODIN_S_SCRL_SHTER_BASEOBJECT_H