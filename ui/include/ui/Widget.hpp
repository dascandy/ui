#pragma once

#include <Var.h>
#include <vector>
#include <ui/vertex.hpp>
#include <map>

class ShaderProgram;
struct vertex;
class Texture;

class Widget {
public:
  Widget();
  virtual ~Widget() = default;
  virtual void Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>&) = 0;
  Val<float> w, h, x, y;
};


