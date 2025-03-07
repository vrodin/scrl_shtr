//
// Created by vrodin on 03.03.2025.
//
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES2/gl2.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>
#include <android/imagedecoder.h>

#include "GameLogic.h"
#include "Hero.h"
#include "Bullet.h"
#include "Bird.h"
#include "Bomber.h"
#include "Fighter.h"
#include "AndroidOut.h"
#include "Shader.h"
#include "TextureAsset.h"


GameLogic::GameLogic(android_app *pApp) : score_(0), gameState(GameState::Playing), app_(pApp) {
    initRenderer();
    updateRenderArea();
    hero = new Hero(glm::vec2(width_/2, height_/3), glm::vec2(200, 200));
    hero->setModel(&models_[1]);
}

GameLogic::~GameLogic() {
    for(auto bullet : hero->getBullets()) {
        delete bullet;
    }

    delete hero;

    for (auto enemy : enemies) {
        if(auto *fighter = dynamic_cast<Fighter*>(enemy)) {
            for(auto& bullet : fighter->getBullets()) {
                delete bullet;
            }
        }
        delete enemy;
    }

    if (display_ != EGL_NO_DISPLAY) {
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context_ != EGL_NO_CONTEXT) {
            eglDestroyContext(display_, context_);
            context_ = EGL_NO_CONTEXT;
        }
        if (surface_ != EGL_NO_SURFACE) {
            eglDestroySurface(display_, surface_);
            surface_ = EGL_NO_SURFACE;
        }
        eglTerminate(display_);
        display_ = EGL_NO_DISPLAY;
    }
}

void GameLogic::update(float deltaTime) {
    if (gameState != GameState::Playing) {
        return;
    }

    hero->update(deltaTime);

    for (auto& enemy : enemies) {
        enemy->update(deltaTime);
    }

    checkCollisions();
    cleanupObjects();
    spawnEnemies(deltaTime);
}

void GameLogic::render() {
    if (!isScrolling) {
        posX += velocityX;

        velocityX *= 0.95f;

        if (std::abs(velocityX) < 0.1f) velocityX = 0;
    }

    updateRenderArea();

    shader_->activate();
    if (shaderNeedsNewProjectionMatrix_) {
        shader_->setProjectionMatrix( glm::ortho(0.0f, (float)width_, 0.0f, (float)height_ , -1.0f, 1.0f));
        shaderNeedsNewProjectionMatrix_ = false;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    hero->render();
    hero->setX(std::max(std::min(posX, (float)width_ - 200.0f), 0.0f));
    shader_->drawModel(*hero->getModel(), (float)hero->getPosition().x, (float)hero->getPosition().y);
    for(auto &bullet : hero->getBullets()) {
        bullet->render();
        //shader_->drawModel(*bullet->getModel(), (float)bullet->getPosition().x, (float)bullet->getPosition().y);
        shader_->drawModel(models_[4], (float)bullet->getPosition().x, (float)bullet->getPosition().y);
    }

    for (auto& enemy : enemies) {
        enemy->render();
        shader_->drawModel(*enemy->getModel(), (float)enemy->getPosition().x, (float)enemy->getPosition().y);
        if(auto *fighter = dynamic_cast<Fighter*>(enemy)) {
            for (auto &bullet: fighter->getBullets())
                shader_->drawModel(models_[4], (float)bullet->getPosition().x, (float)bullet->getPosition().y);
        }
    }

    shader_->deactivate();

    fontRenderer->setProjectionMatrix(glm::ortho(0.0f, (float)width_, 0.0f, (float)height_ , -1.0f, 1.0f));
    fontRenderer->RenderText(std::to_string(score_), 120.0f, 100.0f, 2.0f, glm::vec4(0.8f, 0.0f, 0.0f, 1.0f));

    auto swapResult = eglSwapBuffers(display_, surface_);

}

void GameLogic::checkCollisions() {
    for (auto& enemy : enemies) {
        if (isColliding(hero, enemy)) {
            handlePlayerCollision();
            break;
        }

        if(auto *fighter = dynamic_cast<Fighter*>(enemy)) {
            for (auto& bullet : fighter->getBullets()) {
                if (isColliding(bullet, hero)) {
                    handlePlayerCollision();
                    break;
                }
            }
        }
    }

    for (auto& bullet : hero->getBullets()) {
        for (auto& enemy : enemies) {
            if (!enemy->isImmortal() && isColliding(bullet, enemy)) {
                handleBulletCollision(bullet, enemy);
                break;
            }
        }
    }
}

bool GameLogic::isColliding(BaseObject* obj1, BaseObject* obj2) {
    bool collisionX = obj1->getPosition().x + obj1->getSize().x >= obj2->getPosition().x &&
                      obj2->getPosition().x + obj2->getSize().x >= obj1->getPosition().x;

    bool collisionY = obj1->getPosition().y + obj1->getSize().y >= obj2->getPosition().y &&
                      obj2->getPosition().y + obj2->getSize().y >= obj1->getPosition().y;

    return collisionX && collisionY;
}

void GameLogic::handlePlayerCollision() {
    hero->markForRemoval();
    gameState = GameState::GameOver;
    //animation and sound boom
}

void GameLogic::handleBulletCollision(Bullet* bullet, BaseObject* enemy) {
    std::remove(hero->getBullets().begin(), hero->getBullets().end(), bullet);
    delete bullet;

    enemies.erase(std::remove(enemies.begin(), enemies.end(), enemy), enemies.end());

    score_ += 1;
    //animation and sound boom
}

void GameLogic::spawnEnemies(float deltaTime) {
    static float spawnTimer = 0.0f;
    spawnTimer += deltaTime;

    if (spawnTimer >= 2.0f) {
        spawnTimer = 0.0f;

        int enemyType = rand() % 3;
        switch (enemyType) {
            case 0: {
                auto bi = new Bird(glm::vec2(rand() % (width_ /3) - 40 , height_), glm::vec2(40, 40),
                         glm::vec2(0.0f, -30.0f));
                bi->setModel(&models_[3]);
                enemies.push_back(bi);
                break;
            }
            case 1: {
                auto bo = new Bomber(glm::vec2(rand() % (width_ * 2/3) + width_/3 - 300, height_), glm::vec2(300, 300),
                                     glm::vec2(0.0f, -100.0f));
                bo->setModel(&models_[0]);
                enemies.push_back(bo);
                break;
            }
            case 2: {
                auto f = new Fighter(glm::vec2(rand() % width_ - hero->getSize().y, height_), hero->getSize(),
                                     glm::vec2(0.0f, -150.0f));
                f->setModel(&models_[2]);
                enemies.push_back(f);

                break;
            }
        }
    }
}

void GameLogic::reset() {
    // Сброс игры
    score_ = 0;
    gameState = GameState::Playing;

    // Удаление всех врагов и пуль
    for (auto& enemy : enemies) {
        if(auto *fighter = dynamic_cast<Fighter*>(enemy)) {
            for (auto& bullet : fighter->getBullets()) {
                delete bullet;
            }
        }
        delete enemy;
    }
    enemies.clear();

    // Сброс состояния игрока
    hero->setPosition(glm::vec2(100, 300));
    hero->setHealth(1);
}

void GameLogic::cleanupObjects() {
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](BaseObject* enemy) {
        if (enemy->shouldBeRemoved()) {
            delete enemy;
            return true;
        }
        return false;
    }), enemies.end());
}

void GameLogic::handleInput() {
    // handle all queued inputs
    auto *inputBuffer = android_app_swap_input_buffers(app_);
    if (!inputBuffer) {
        // no inputs yet.
        return;
    }

    // handle motion events (motionEventsCounts can be 0).
    for (auto i = 0; i < inputBuffer->motionEventsCount; i++) {
        auto &motionEvent = inputBuffer->motionEvents[i];
        auto action = motionEvent.action;

        // Find the pointer index, mask and bitshift to turn it into a readable value.
        auto pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        aout << "Pointer(s): ";

        // get the x and y position of this event if it is not ACTION_MOVE.
        auto &pointer = motionEvent.pointers[pointerIndex];
        auto x = GameActivityPointerAxes_getX(&pointer);
        auto y = GameActivityPointerAxes_getY(&pointer);

        // determine the action type and process the event accordingly.
        switch (action & AMOTION_EVENT_ACTION_MASK) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                isScrolling = true;
                lastX = x;
                velocityX = 0;
                break;

            case AMOTION_EVENT_ACTION_MOVE:
                if (isScrolling) {
                    velocityX = x - lastX;
                    lastX = x;
                    posX += velocityX;
                    posX = std::max(std::min(posX, (float)width_ - 100.0f), 0.0f);
                }
                break;
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                isScrolling = false;
                break;
            default:
                aout << "Unknown MotionEvent Action: " << action;
        }
        aout << std::endl;
    }
    // clear the motion input count in this buffer for main thread to re-use.
    android_app_clear_motion_events(inputBuffer);

    // handle input key events.
    for (auto i = 0; i < inputBuffer->keyEventsCount; i++) {
        auto &keyEvent = inputBuffer->keyEvents[i];
        aout << "Key: " << keyEvent.keyCode <<" ";
        switch (keyEvent.action) {
            case AKEY_EVENT_ACTION_DOWN:
                aout << "Key Down";
                break;
            case AKEY_EVENT_ACTION_UP:
                aout << "Key Up";
                break;
            case AKEY_EVENT_ACTION_MULTIPLE:
                // Deprecated since Android API level 29.
                aout << "Multiple Key Actions";
                break;
            default:
                aout << "Unknown KeyEvent Action: " << keyEvent.action;
        }
        aout << std::endl;
    }
    // clear the key input count too.
    android_app_clear_key_events(inputBuffer);
}

void GameLogic::initRenderer() {

    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display_ == EGL_NO_DISPLAY) {
        __android_log_print(ANDROID_LOG_ERROR, "EGL", "Failed to get EGL display");
    }

    if (!eglInitialize(display_, nullptr, nullptr)) {
        __android_log_print(ANDROID_LOG_ERROR, "EGL", "Failed to initialize EGL");
    }

    EGLint numConfigs;
    EGLConfig eglConfig;
    constexpr EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_NONE
    };

    eglChooseConfig(display_, attribs, &eglConfig, 1, &numConfigs);
    EGLSurface surface = eglCreateWindowSurface(display_, eglConfig, app_->window, nullptr);

    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLContext context = eglCreateContext(display_, eglConfig, EGL_NO_CONTEXT, contextAttribs);

    auto madeCurrent = eglMakeCurrent(display_, surface, surface, context);


    surface_ = surface;
    context_ = context;

    width_ = -1;
    height_ = -1;

    const char* versionStr = (const char*)glGetString(GL_VERSION);

    shader_ = std::unique_ptr<Shader>(
            Shader::loadShader(vertex, fragment, "inPosition", "inUV", "uProjection"));

    fontRenderer = std::make_shared<FontRenderer>(app_->activity->assetManager, "font.ttf", 48 );


    glClearColor(CORNFLOWER_BLUE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    createModels();
}

void GameLogic::createModels() {
    auto assetManager = app_->activity->assetManager;
    models_.emplace_back(*createModel(300.0f, 300.0f, TextureAsset::loadAsset(assetManager, "bfl.png")));
    models_.emplace_back(*createModel(200.0f, 200.0f, TextureAsset::loadAsset(assetManager, "fl.png")));
    models_.emplace_back(*createModel(200.0f, 200.0f, TextureAsset::loadAsset(assetManager, "fl2.png")));
    models_.emplace_back(*createModel(40.0f, 40.0f, TextureAsset::loadAsset(assetManager, "bird.png")));
    models_.emplace_back(*createModel(5.0f, 10.0f, TextureAsset::loadAsset(assetManager, "bullet.png")));

}

std::shared_ptr<Model> GameLogic::createModel(float x, float y, const std::shared_ptr<TextureAsset>& texture) {
    return std::make_shared<Model> (
            std::vector<float>{
                    // Позиции         // Текстурные координаты
                    0.0f, y,        0.0f, 1.0f, // верхняя правая
                    x, y,           1.0f, 1.0f, // верхняя левая
                    x, 0.0f,        1.0f, 0.0f, // нижняя левая
                    0.0f, 0.0f,     0.0f, 0.0f  // нижняя правая
            },
            std::vector<unsigned int>{
                    0, 1, 2, // первый треугольник
                    2, 3, 0  // второй треугольник
            },
            texture
    );
}

void GameLogic::updateRenderArea() {
    EGLint width;
    eglQuerySurface(display_, surface_, EGL_WIDTH, &width);

    EGLint height;
    eglQuerySurface(display_, surface_, EGL_HEIGHT, &height);

    if (width != width_ || height != height_) {
        width_ = width;
        height_ = height;
        glViewport(0, 0, width, height);

        shaderNeedsNewProjectionMatrix_ = true;
    }
}