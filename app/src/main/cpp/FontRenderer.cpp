#include <android/log.h>
#include <cstring>
#include "FontRenderer.h"

#define STB_TRUETYPE_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define LOG_TAG "FontRenderer"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

FontRenderer::FontRenderer(AAssetManager* assetManager, const std::string& fontPath, int fontSize)
        : assetManager(assetManager), fontBuffer(nullptr) {
    LoadFont(fontPath, fontSize);
    LoadCharacters();

    // Шейдеры
    const char* vertexShaderSource = R"(#version 100
precision mediump float;
attribute vec4 vertex;
varying vec2 TexCoords;
uniform mat4 projection;
uniform mat4 model;

void main() {
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
    )";

    const char* fragmentShaderSource = R"(#version 100
precision mediump float;

varying vec2 TexCoords;

uniform sampler2D text;
uniform vec4 textColor;

void main() {
    float alpha = texture2D(text, TexCoords).a;
    gl_FragColor = textColor * vec4(1.0, 1.0, 1.0, alpha);
}
    )";

    shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Настраиваем атрибуты вершин
    GLint vertex = glGetAttribLocation(shaderProgram, "vertex");
    glEnableVertexAttribArray(vertex);
    glVertexAttribPointer(vertex, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

FontRenderer::~FontRenderer() {
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &textureID);
    delete[] fontBuffer;
}

void FontRenderer::LoadFont(const std::string& fontPath, int fontSize) {
    AAsset* asset = AAssetManager_open(assetManager, fontPath.c_str(), AASSET_MODE_BUFFER);
    if (!asset) {
        LOGE("Failed to load font: %s", fontPath.c_str());
        return;
    }

    size_t fontBufferSize = AAsset_getLength(asset);
    fontBuffer = new unsigned char[fontBufferSize];
    AAsset_read(asset, fontBuffer, fontBufferSize);
    AAsset_close(asset);

    if (!stbtt_InitFont(&fontInfo, fontBuffer, 0)) {
        LOGE("Failed to initialize font: %s", fontPath.c_str());
        delete[] fontBuffer;
        fontBuffer = nullptr;
    }
}

void FontRenderer::LoadCharacters() {
    if (!fontBuffer) return;

    float scale = stbtt_ScaleForPixelHeight(&fontInfo, 48);

    // Определяем размер атласа
    const int atlasWidth = 1024;
    const int atlasHeight = 512;
    unsigned char* atlasData = new unsigned char[atlasWidth * atlasHeight];
    memset(atlasData, 0, atlasWidth * atlasHeight);

    int x = 0, y = 0;
    int maxHeight = 0;

    for (unsigned char c = 0; c < 128; c++) {
        int width, height, xoff, yoff;
        unsigned char* bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0, scale, c, &width, &height, &xoff, &yoff);

        if (x + width > atlasWidth) {
            x = 0;
            y += maxHeight;
            maxHeight = 0;
        }

        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                atlasData[(y + row) * atlasWidth + (x + col)] = bitmap[row * width + col];
            }
        }

        // Сохраняем UV-координаты символа
        float u1 = (float)x / atlasWidth;
        float v1 = (float)y / atlasHeight;
        float u2 = (float)(x + width) / atlasWidth;
        float v2 = (float)(y + height) / atlasHeight;

        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(&fontInfo, c, &advanceWidth, &leftSideBearing);

        Character character = {
                glm::vec2(u1, v1),
                glm::vec2(u2, v2),
                glm::ivec2(width, height),
                glm::ivec2(leftSideBearing,yoff),
                advanceWidth
        };
        characters[c] = character;

        x += width;
        if (height > maxHeight) maxHeight = height;

        stbtt_FreeBitmap(bitmap, nullptr);
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, atlasWidth, atlasHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, atlasData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] atlasData;
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FontRenderer::RenderText(const std::string& text, float x, float y, float scale, const glm::vec4& color) {

    glUseProgram(shaderProgram);
    GLint textColorLoc = glGetUniformLocation(shaderProgram, "textColor");
    glUniform4fv(textColorLoc, 1, glm::value_ptr(color));
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    GLint vertexLoc = glGetAttribLocation(shaderProgram, "vertex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    float offset = 0;
    for (char c : text) {
        Character ch = characters[c];

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float u1 = ch.uv1.x;
        float v1 = ch.uv1.y;
        float u2 = ch.uv2.x;
        float v2 = ch.uv2.y;

        float vertices[] = {
                0.0f,     0.0f + h, u2, v2 , // Левый нижний угол
                0.0f,     0.0f,     u2, v1 , // Левый верхний угол
                0.0f + w, 0.0f,     u1, v1 , // Правый верхний угол

                0.0f,     0.0f + h, u2, v2 , // Левый нижний угол
                0.0f + w, 0.0f,     u1, v1 , // Правый верхний угол
                0.0f + w, 0.0f + h, u1, v2   // Правый нижний угол
        };

        glm::mat4 mod = glm::mat4(1.0f);
        mod = glm::translate(mod, glm::vec3(x, y + offset, 0.0f));
        mod = glm::rotate(mod, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mod));
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glEnableVertexAttribArray(vertexLoc);
        glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        offset += w;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint FontRenderer::CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        LOGE("Shader compilation failed: %s", infoLog);
    }
    return shader;
}

GLuint FontRenderer::CreateShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOGE("Shader program linking failed: %s", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void FontRenderer::setProjectionMatrix(glm::mat4 projectionMtrx) {
    projectionMatrix = projectionMtrx;
}