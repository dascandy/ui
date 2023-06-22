#pragma once

#include <Var.h>
#include <vector>
#include <ui/Widget.hpp>
#include <memory>
#include <ui/SubTexture.hpp>
#include <ui/Attributes.hpp>
#include <ui/FontRender.hpp>

class LabelWidget : public Widget {
public:
  LabelWidget();
  void Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>&) override;
  void set(Justify justify);
  void set(Align align);
  void set(FixedText text);
  void set(BoundText text);
  void set(FontFace font);
  void Relayout() override;
  using Widget::set;
private:
  void UpdateText(const std::string& newText);
  std::optional<SimpleObserver<std::string>> observer;
  Align align;
  Justify justify;
  std::string text;
  FontFace face;

  SubTexture texture;
  size_t cacheW = 0, cacheH = 0;
  Align cacheAlign;
  Justify cacheJustify;
  std::string cacheText;
  FontFace cacheFace;
};

