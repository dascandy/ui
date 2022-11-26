#pragma once

#include <Var.h>
#include <vector>
#include <ui/Widget.hpp>
#include <memory>
#include <ui/SubTexture.hpp>

class Flexbox : public Widget {
public:
  enum class Direction {
    Column,
    ColumnReverse,
    Row,
    RowReverse,
  };
  enum class Wrap {
    Wrap,
    Line,
    WrapReverse,
  };
  enum class Justify {
    Center,
    Start,
    End,
    SpaceAround,
    SpaceBetween,
    SpaceEvenly,
  };
  enum class Align {
    Center,
    Start,
    End,
    Stretch,
  };
  friend std::string to_string(Direction );
  friend std::string to_string(Wrap );
  friend std::string to_string(Justify );
  friend std::string to_string(Align );
  Flexbox(SubTexture st);
  void Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>&) override;
  void setDirection(Direction direction);
  void setWrap(Wrap wrap);
  void setJustify(Justify justify);
  void setVerticalJustify(Justify justify);
  void setAlign(Align align);
  void setMargin(float margin);
  void addWidget(std::unique_ptr<Widget> widget, int position = -1);
  void removeWidget(size_t position);
  bool allowRelayout = true;
  void Relayout();
private:
  SubTexture texture;
  Direction direction = Direction::Row;
  Wrap wrap = Wrap::Wrap;
  Justify justify = Justify::Start;
  Justify vjustify = Justify::Start;
  Align align = Align::Start;
  std::vector<std::unique_ptr<Widget>> widgets;
  float margin = 5;
};

