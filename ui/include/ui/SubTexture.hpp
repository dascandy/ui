#pragma once

#include <ui/Texture.hpp>
#include <memory>

class SubTexture {
public:
  SubTexture()
  : texture(nullptr)
  {}
  SubTexture(std::shared_ptr<Texture> tex)
  : texture(tex)
  {}
  SubTexture(std::shared_ptr<Texture> tex, float x, float y, float w, float h)
  : texture(tex)
  , x(x / w)
  , y(y / h)
  , w(w)
  , h(h)
  {}
  std::shared_ptr<Texture> texture;
  float x = 0.0f, y = 0.0f, w = 1.0f, h = 1.0f;
};


