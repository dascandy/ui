#pragma once

#include <vector>
#include <ui/vertex.hpp>
#include <map>
#include <span>
#include <Var.h>
#include <ui/FontRender.hpp>

class Texture;

class Theme {
public:
  static Theme& Instance();
  static void SetTheme(std::unique_ptr<Theme> theme);
  enum class Checked {
    Check,
    Cross,
    None,
  };
  enum class Selected {
    Selected,
    None,
  };
  enum class Color {
    Red = 0,
    Yellow = 1,
    Green = 2,
    Blue = 3,
  };
  enum class FontStyle {
    Monospace,
    Serif,
    SansSerif,
    Playful,
    Flashy,
    Dropcap,
  };
  void generateSquare(std::map<Texture*, std::vector<vertex>>& out, Texture* tex, float z, float x1, float x2, float y1, float y2, float s1, float s2, float t1, float t2);
  void generateNinepatch(std::map<Texture*, std::vector<vertex>>& out, Texture* tex, float z, std::span<const float> in_s, std::span<const float> in_t, float x, float y, float w, float h);
  void CreateCheckbox(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h, Checked value);
  void CreateOptionbox(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h, Selected value);
  void CreateWhiteBox(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h);
  void CreateGradientButton(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h);
  void CreateRegButton(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h);
  void CreateColorBox(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h);
  void CreateRegbuttonRaised(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h);
  void CreateScroll(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float h);
  void SetColor(Color newColor) { this->color = newColor; }
  Val<float> splineTo(Val<float> from, Val<float> to);
  Theme(Color color);
  Font* GetFont(FontStyle style);
  Val<float> buttonWidth, buttonHeight;
  Val<float> textboxWidth, textboxHeight;
  Val<float> labelWidth, labelHeight;
private:
  std::unique_ptr<Texture> texture;
  float coloroffset();
  Color color = Color::Red;
};


