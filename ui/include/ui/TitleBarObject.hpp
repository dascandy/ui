#pragma once

#include <Var.h>
#include <vector>
#include <ui/Widget.hpp>
#include <memory>
#include <ui/SubTexture.hpp>
#include <ui/Attributes.hpp>
#include <ui/LabelWidget.hpp>
#include <ui/Button.hpp>

class TitleBarObject : public Widget {
public:
  TitleBarObject();
  void Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>&) override;
  void set(FixedText text);
  void set(BoundText text);
  void set(TitleBarButton button);
  void Relayout() override {}
  using Widget::set;
private:
  LabelWidget title;
  std::vector<std::unique_ptr<ButtonWidget>> buttons;
};

