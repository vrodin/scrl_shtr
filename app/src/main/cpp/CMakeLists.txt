cmake_minimum_required(VERSION 3.22.1)

project("shooter")

include_directories(${CMAKE_SOURCE_DIR}/../lib)

add_library(shooter SHARED
        main.cpp
        AndroidOut.cpp
        Shader.cpp
        TextureAsset.cpp
        FontRenderer.cpp
        Bird.cpp
        Bomber.cpp
        Bullet.cpp
        Fighter.cpp
        GameLogic.cpp
        Hero.cpp
        Button.cpp
)

find_package(game-activity REQUIRED CONFIG)

target_include_directories(shooter PRIVATE ${CMAKE_SOURCE_DIR}/../lib)

target_link_libraries(shooter
        game-activity::game-activity
        EGL
        GLESv2
        jnigraphics
        android
        log)