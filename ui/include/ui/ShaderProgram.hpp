#pragma once

#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <span>

class Color;
class Texture;

class ShaderProgram
{
public:
    ShaderProgram(const std::string &filename, const char *vss, const char *fss, const char *gss, const char *css, const char *ess);
    ~ShaderProgram();
    void Set(const char *name, const glm::mat4 &mat);
    void Set(const char *name, const glm::vec2 &vec);
    void SetActive();
    void Set(const char *name, int value);
    void Set(const char *name, unsigned int value);
    void Set(const char *name, float value);
    void Set(const char *name, const glm::vec3 &value);
    void Set(const char *name, const Color &value);
    void Set(const char *name, Texture &value);
    void UnbindTextures();
private:
    std::string filename;
    int curtex;
    int getUniformLocation(const char *name);
    int vs, fs, gs, cs, es;
    int prog;
    std::map<std::string, int> uniforms;
    std::map<std::string, int> texUnits;
};


