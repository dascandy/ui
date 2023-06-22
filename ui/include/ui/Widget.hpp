#pragma once

#include <Var.h>
#include <vector>
#include <ui/vertex.hpp>
#include <map>
#include <SDL2/SDL.h>
#include <ui/Attributes.hpp>

class ShaderProgram;
struct vertex;
class Texture;

class Widget {
public:
  Widget();
  virtual ~Widget();
  virtual void Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>&) = 0;
  virtual void Relayout() {}
  void set(Width width);
  void set(Height height);
  void set(Stretch);
  virtual bool HandleEvent(const SDL_Event&) { return false; }
  virtual bool stretch() { return stretch_; }
  Val<float> w, h, x, y;
  bool stretch_ = false;
};

// Widget touch behavior:
// Pass to first subwidget touched (default)
// treat touches as separate cursors
// click + two finger drag/pinch/rotate
// 

