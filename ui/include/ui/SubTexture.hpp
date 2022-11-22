#pragma once

#include <ui/Texture.hpp>
#include <memory>

class SubTexture {
public:
  SubTexture(std::shared_ptr<Texture> tex)
  : texture(tex)
  {}
  std::shared_ptr<Texture> texture;
  float x = 0.0f, y = 0.0f, w = 1.0f, h = 1.0f;
};


