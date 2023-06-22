#pragma once

#include <string>
#include <map>
#include <ui/Texture.hpp>
#include <ui/SubTexture.hpp>
#include <ui/Attributes.hpp>
#include <ui/vertex.hpp>

enum class FontSizeUnit {
  Pixels,
  Points,
  Centimeter,
  Millimeter,
};

struct Font;
struct FontFace;

class FontCache {
public:
  FontCache();
  ~FontCache();
  static FontCache& Instance();
  Font* loadFont(const std::string& name);
  void Render(std::vector<vertex>& vertices, const std::string& text, const FontFace& fontface, float left, float top, float width, float z, Align align);
private:
  std::map<std::string, Font> fonts;
};

struct FontFace {
  Font* font;
  FontSizeUnit sizeUnit;
  size_t size;
  int operator<=>(const FontFace&) const = default;
};


