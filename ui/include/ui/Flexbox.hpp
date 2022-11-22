#pragma once

#include <Var.h>
#include <vector>
#include <ui/Widget.hpp>
#include <memory>

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
  void Render() override;
  void setDirection(Direction direction);
  void setWrap(Wrap wrap);
  void setJustify(Justify justify);
  void setVerticalJustify(Justify justify);
  void setAlign(Align align);
  void setMargin(double margin);
  void addWidget(std::unique_ptr<Widget> widget, int position = -1);
  void removeWidget(size_t position);
  bool allowRelayout = true;
  void Relayout();
private:
  Direction direction = Direction::Row;
  Wrap wrap = Wrap::Wrap;
  Justify justify = Justify::Start;
  Justify vjustify = Justify::Start;
  Align align = Align::Start;
  std::vector<std::unique_ptr<Widget>> widgets;
  double margin = 5;
};

