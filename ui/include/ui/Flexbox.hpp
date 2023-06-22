#pragma once

#include <Var.h>
#include <vector>
#include <ui/Widget.hpp>
#include <memory>
#include <ui/SubTexture.hpp>
#include <ui/Attributes.hpp>

class Flexbox : public Widget {
public:
  Flexbox();
  void Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>&) override;
  void set(Direction direction);
  void set(Wrap wrap);
  void set(Justify justify);
  void setVerticalJustify(Justify justify);
  void set(Align align);
  void set(Margin margin);
  void set(std::vector<std::unique_ptr<Widget>> elements) {
    widgets = std::move(elements);
  }
  bool HandleEvent(const SDL_Event& event) override;

  void addWidget(std::unique_ptr<Widget> widget, int position = -1);
  void removeWidget(size_t position);
  bool allowRelayout = true;
  void Relayout() override;
  void autosize();
  int stretchcount();
  bool stretch() override;
  using Widget::set;
private:
  Direction direction = Direction::Row;
  Wrap wrap = Wrap::Wrap;
  Justify justify = Justify::Start;
  Justify vjustify = Justify::Start;
  Align align = Align::Start;
  std::vector<std::unique_ptr<Widget>> widgets;
  Widget* currentFocus = nullptr;
  float margin = 5;
  float outmargin = 0;
};

