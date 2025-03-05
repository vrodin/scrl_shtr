//
// Created by vrodin on 03.03.2025.
//

#ifndef VRODIN_S_SCRL_SHTER_GAMELOGIC_H
#define VRODIN_S_SCRL_SHTER_GAMELOGIC_H

#include <list>
#include "Hero.h"
#include "Bullet.h"
#include "BaseObject.h"
#include <EGL/egl.h>
#include <glm/glm.hpp>
#include <memory>
#include <map>
#include <string>
#include <utility>

#include "Model.h"
#include "Shader.h"
#include "FontRenderer.h"

struct android_app;

#define CORNFLOWER_BLUE 1, 1, 1, 1

static const char *vertex = R"vertex(#version 100
attribute vec2 inPosition;
attribute vec2 inUV;
varying vec2 fragUV;

uniform mat4 uProjection;
uniform mat4 model;

void main() {
    fragUV = inUV;
    gl_Position = uProjection * model * vec4(inPosition, 0.0, 1.0);
}
)vertex";

static const char *fragment = R"fragment(#version 100
precision mediump float;

varying vec2 fragUV;
uniform sampler2D uTexture;

void main() {
    //gl_FragColor = vec4(0,0,0, 1.0);
    gl_FragColor = texture2D(uTexture, fragUV);
}
)fragment";

enum class GameState {
    Playing,
    GameOver
};

class GameLogic
{
public:
    GameLogic(android_app *pApp);
    ~GameLogic();

    void update(float deltaTime);
    void render();
    GameState getGameState() const {return gameState; }
    void reset();
    uint32_t getScore() const {return score_;}

    void handleInput();
    void initRenderer();

private:
    void createModels();
    void updateRenderArea();
    void checkCollisions();
    void handlePlayerCollision();
    void handleBulletCollision(Bullet* bullet, BaseObject* enemy);
    bool isColliding(BaseObject* obj1, BaseObject* obj2);
    void cleanupObjects();
    void spawnEnemies(float deltaTime);
    uint32_t score_;
    Hero *hero;
    std::list<BaseObject*> enemies;
    GameState gameState;

    static std::shared_ptr<Model> createModel(float x, float y, const std::shared_ptr<TextureAsset>& texture);

    android_app *app_;
    EGLDisplay display_;
    EGLSurface surface_;
    EGLContext context_;
    EGLint width_;
    EGLint height_;

    bool shaderNeedsNewProjectionMatrix_;

    std::unique_ptr<Shader> shader_;
    std::vector<Model> models_;

    bool isScrolling;
    float lastX;
    float velocityX;
    float posX = 0, posY;

    glm::mat4 projection;

    std::shared_ptr<FontRenderer> fontRenderer;
};
#endif //VRODIN_S_SCRL_SHTER_GAMELOGIC_H
