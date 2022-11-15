#pragma once

extern Val<int64_t> currentTimeMs; 

class Widget {
public:
  virtual ~Widget() = default;
  Val<double> w, h, x, y;
};

class Flexbox {
  enum class Direction {
    Column,
    ColumnReverse,
    Row,
    RowReverse,
  } direction = Direction::Row;
  enum class Wrap {
    Wrap,
    Line,
    WrapReverse,
  } wrap = Wrap::Wrap;
  enum class Justify {
    Center,
    Start,
    End,
    SpaceAround,
    SpaceBetween,
  } justify = Justify::Start;
  enum class Align {
    Center,
    Start,
    End,
    Stretch,
  } align = Align::Center;
};

class Window {
  Val<uint32_t> width, height, x, y;
  std::vector<Widget*> widgets;
};


