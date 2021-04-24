#pragma once

#include <GL/glew.h>
#include <SDL_opengl.h>

#define GLM_FORCE_SILENT_WARNINGS

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "logger.hpp"

#include <string>

class GLShader final
{
public:
    GLShader();

    bool LoadSource(const char * vertex_Source, const char* fragment_Source);
    bool LoadFromFile(const char* vertex_Path, const char* fragment_Path);

    GLuint GetProgramID();
    GLuint GetAttributeLocation(const char* attr);

    void UniformMatrix4fv(const char* name, glm::mat4 matrix);
    void UniformVec2(const char* name, glm::vec2 vector);
    void UniformVec3(const char* name, glm::vec3 vector);
    void UniformVec4(const char* name, glm::vec4 vector);
    void Uniform1i(const char* name, GLint v);

    void Use();
    void UnUse();

    void Free();

private:
    GLuint mVertexID = 0;
    GLuint mFragmentID = 0;
    GLuint mProgramID = 0;
    GLenum mShaderType = 0;

    GLuint CompileShader(const char* source, GLenum shaderType);
};

extern const char* gShader_TextureVSH;
extern const char* gShader_TextureFSH;