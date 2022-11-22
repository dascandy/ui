#pragma once

#include <glm/gtc/matrix_transform.hpp>

class Camera;
class ShaderProgram;

class Renderable
{
public:
    virtual ~Renderable() {}
    virtual void Draw(ShaderProgram *prog, Camera *cam, glm::mat4 mat_vp) = 0;
};


