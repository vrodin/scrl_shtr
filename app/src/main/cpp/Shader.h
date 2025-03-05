#ifndef ANDROIDGLINVESTIGATIONS_SHADER_H
#define ANDROIDGLINVESTIGATIONS_SHADER_H

#include <string>
#include <GLES2/gl2.h>
#include <glm/glm.hpp>

class Model;

class Shader {
public:
    static Shader *loadShader(
            const std::string &vertexSource,
            const std::string &fragmentSource,
            const std::string &positionAttributeName,
            const std::string &uvAttributeName,
            const std::string &projectionMatrixUniformName);

    inline ~Shader() {
        if (program_) {
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteProgram(program_);
            program_ = 0;
        }
    }

    void activate() const;
    void deactivate() const;
    void drawModel(const Model &model, float posX, float posY) const;
    void setProjectionMatrix(glm::mat4 projectionMatrix) const;

private:
    static GLuint loadShader(GLenum shaderType, const std::string &shaderSource);

    constexpr Shader(
            GLuint program,
            GLint position,
            GLint uv,
            GLint projectionMatrix)
            : program_(program),
              position_(position),
              uv_(uv),
              projectionMatrix_(projectionMatrix) {
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    GLuint program_;
    GLint position_;
    GLint uv_;
    GLint projectionMatrix_;
    GLuint VBO, EBO;
};

#endif //ANDROIDGLINVESTIGATIONS_SHADER_H