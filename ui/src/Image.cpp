#include <ui/Image.hpp>
#include <cstdio>

Image::Image(SubTexture texture) 
: texture(texture)
{}

Image::~Image() = default;

void Image::Render() {
  printf("%f %f %f %f\n", **x, **y, **w, **h);
}


