#ifndef FONTRENDERER_H
#define FONTRENDERER_H

#include <GLES2/gl2.h>
#include <string>
#include <map>
#include <android/asset_manager.h>
#include <glm/glm.hpp>

#include "stb_truetype.h"

class FontRenderer {
public:
    FontRenderer(AAssetManager* assetManager, const std::string& fontPath, int fontSize);
    ~FontRenderer();

    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec4& color);
    void setProjectionMatrix(glm::mat4 projectionMatrix);
private:
    struct Character {
        glm::vec2 uv1;         // UV-координаты левого верхнего угла
        glm::vec2 uv2;         // UV-координаты правого нижнего угла
        glm::ivec2 size;       // Размер символа (ширина, высота)
        glm::ivec2 bearing;    // Смещение символа (left, top)
        int advance;           // Расстояние до следующего символа
    };

    AAssetManager* assetManager;
    stbtt_fontinfo fontInfo;
    unsigned char* fontBuffer;
    std::map<char, Character> characters;
    GLuint shaderProgram;
    GLuint vbo;
    glm::mat4 projectionMatrix;
    GLuint textureID;

    void LoadFont(const std::string& fontPath, int fontSize);
    void LoadCharacters();
    GLuint CompileShader(GLenum type, const char* source);
    GLuint CreateShaderProgram(const char* vertexSource, const char* fragmentSource);
};

#endif // FONTRENDERER_H