#pragma once

#include <ui/Widget.hpp>
#include <ui/SubTexture.hpp>

class Image : public Widget {
public:
  Image(SubTexture texture);
  ~Image() override;
  void Render() override;
private:
  SubTexture texture;
};


