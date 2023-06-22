#pragma once

#include <ui/Widget.hpp>
#include <ui/SubTexture.hpp>

class Image : public Widget {
public:
  Image(SubTexture texture);
  ~Image() override;
  void Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>&) override;
  using Widget::set;
private:
  SubTexture texture;
};


