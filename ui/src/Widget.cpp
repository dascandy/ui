#include <ui/Widget.hpp>

Widget::Widget() 
: w(val(100.0f))
, h(val(100.0f))
, x(val(0.0f))
, y(val(0.0f))
{
}

Widget::~Widget() {
}

void Widget::set(Width newWidth) {
  w = val(newWidth.value);
}

void Widget::set(Height newHeight) {
  h = val(newHeight.value);
}

void Widget::set(Stretch) {
  stretch_ = true;
}

