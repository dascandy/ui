#pragma once

#include <Var.h>
#include <vector>
#include <ui/Widget.hpp>
#include <memory>
#include <ui/SubTexture.hpp>

class TextBoxWidget : public Widget {
public:
  TextBoxWidget();
  void Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>&) override;
  void set(FixedText text);
  void set(BoundText text);
  void set(OnKey keyHandler);
  using Widget::set;
private:
  void UpdateBoundText();
  std::optional<BoundText> boundText;
  std::optional<SimpleObserver<std::string>> observer;
  std::vector<OnKey> keyHandlers;
  std::string text;
  bool myUpdate = false;
};

