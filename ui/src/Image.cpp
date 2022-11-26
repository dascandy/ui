#include <ui/Image.hpp>
#include <cstdio>

Image::Image(SubTexture texture) 
: texture(texture)
{}

Image::~Image() = default;

void Image::Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>& out) {
  float x1 = **x + parentX - **w / 2;
  float y1 = **y + parentY - **h / 2;
  float x2 = x1 + **w;
  float y2 = y1 + **h;
  float s1 = texture.x;
  float t1 = texture.y;
  float s2 = s1 + texture.w;
  float t2 = t1 + texture.h;
  std::vector<vertex>& v = out[texture.texture.get()];
  v.push_back({x1, y1, parentZ, 0, 0, 1, s1, t1 });
  v.push_back({x1, y2, parentZ, 0, 0, 1, s1, t2 });
  v.push_back({x2, y1, parentZ, 0, 0, 1, s2, t1 });
  v.push_back({x2, y1, parentZ, 0, 0, 1, s2, t1 });
  v.push_back({x1, y2, parentZ, 0, 0, 1, s1, t2 });
  v.push_back({x2, y2, parentZ, 0, 0, 1, s2, t2 });
}


