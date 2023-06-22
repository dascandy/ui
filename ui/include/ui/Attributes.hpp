#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <string>
#include "Observer.hpp"

class Widget;

enum class Direction {
  Column,
  ColumnReverse,
  Row,
  RowReverse,
};
std::string to_string(Direction );
enum class Wrap {
  Wrap,
  Line,
  WrapReverse,
};
std::string to_string(Wrap );
enum class Justify {
  Center,
  Start,
  End,
  SpaceAround,
  SpaceBetween,
  SpaceEvenly,
};
std::string to_string(Justify );
enum class Align {
  Center,
  Start,
  End,
  Stretch,
};
std::string to_string(Align );

enum TitleBarButton {
  MinimizeButton = 1,
  MaximizeButton = 2,
  XButton = 4,
};

template <typename... Ts>
std::vector<std::unique_ptr<Widget>> Elements(Ts... ts) {
  std::vector<std::unique_ptr<Widget>> widgets;
  (widgets.push_back(std::move(ts)), ...);
  return widgets;
}

struct OnKey { char c; std::function<void()> handler; };
struct OnClick { std::function<void()> handler; };
struct OnTouchDown { std::function<void(int)> handler; };
struct OnTouchUp { std::function<void(int)> handler; };
struct OnTouchDrag { std::function<void(int, int, int)> handler; };

struct Width { float value; };
struct Height { float value; };
struct Margin { float value; };
struct Stretch {};

struct FixedText {
  std::string value;
};
inline FixedText Text(const std::string& text) { return FixedText{text}; }

struct BoundText {
  Observed<std::string>& value;
};
inline BoundText Text(Observed<std::string>& text) { return BoundText{text}; }



