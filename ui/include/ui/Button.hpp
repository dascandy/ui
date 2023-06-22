#pragma once

#include <Var.h>
#include <vector>
#include <ui/Widget.hpp>
#include <memory>
#include <ui/SubTexture.hpp>
#include <ui/LabelWidget.hpp>

class ButtonWidget : public Widget {
public:
  ButtonWidget();
  void Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>&) override;
  void set(FixedText text);
  void set(OnClick handler);
  using Widget::set;
private:
  LabelWidget label;
  std::function<void()> onclick;
};

