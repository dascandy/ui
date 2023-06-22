#include <ui/FontRender.hpp>
#include <ui/Attributes.hpp>
#include <vector>
#include <filesystem>
#include <fstream>
#include "ttf/ttf.h"
#include <span>
#include <ui/vertex.hpp>

struct Font {
  std::unique_ptr<TtfFont> font = nullptr;
  Font() {}
  Font(std::span<const uint8_t> data) {
    font = std::make_unique<TtfFont>(std::vector<uint8_t>(data.data(), data.data() + data.size()));
  }
};

FontCache& FontCache::Instance() {
  static FontCache cache;
  return cache;
}

FontCache::FontCache() {
}

FontCache::~FontCache() {
  fonts.clear();
}

Font* FontCache::loadFont(const std::string& name) {
  if (auto it = fonts.find(name); it != fonts.end()) return &it->second;
  std::vector<uint8_t> buffer;
  buffer.resize(std::filesystem::file_size(name));
  std::ifstream(name).read((char*)buffer.data(), buffer.size());
  fonts.emplace(name, buffer);
  return &fonts.find(name)->second;
}

void FontCache::Render(std::vector<vertex>& vertices, const std::string& text, const FontFace& fontface, float left, float top, float width, float z, Align align) {
  (void)align;
  TtfFont* font = fontface.font->font.get();
  float desiredSizePx = 10.0f;
  float dpmm = 96 / 25.4;
  switch(fontface.sizeUnit) {
    case FontSizeUnit::Pixels:
      desiredSizePx = fontface.size;
      break;
    case FontSizeUnit::Points:
      desiredSizePx = fontface.size * 1.3333;
      break;
    case FontSizeUnit::Centimeter:
      desiredSizePx = fontface.size * 10 * dpmm;
      break;
    case FontSizeUnit::Millimeter:
      desiredSizePx = fontface.size * dpmm;
      break;
  }

  float lineHeight = font->ascender - font->descender + font->lineGap;
  float fontHeight = font->ascender - font->descender;
  float relativeSize = desiredSizePx / fontHeight;
  float currentX = left;
  float currentY = top + desiredSizePx;
  printf("%f %f %f ==> %f %f %f\n", font->ascender, font->descender, font->lineGap, lineHeight, fontHeight, relativeSize);

  for (size_t n = 0; n < text.size(); n++)
  {
    Glyph &glyph = font->get_glyph(text[n]);

    Mesh m = glyph.ToOutline(*font).ToMesh();

    for (auto& face : m.faces) {
      vertices.push_back({currentX + relativeSize * m.vertices[face.v1].x, currentY - relativeSize * m.vertices[face.v1].y, z, 0, 0, 1, 1, 1});
      vertices.push_back({currentX + relativeSize * m.vertices[face.v2].x, currentY - relativeSize * m.vertices[face.v2].y, z, 0, 0, 1, 1, 1});
      vertices.push_back({currentX + relativeSize * m.vertices[face.v3].x, currentY - relativeSize * m.vertices[face.v3].y, z, 0, 0, 1, 1, 1});
    }

    currentX += glyph.advanceWidth * relativeSize;
    if (currentX > left + width) {
      currentX = left;
      currentY += lineHeight * relativeSize;
    }
  }
}


