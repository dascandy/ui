#pragma once

#include <Var.h>

class Widget {
public:
  virtual ~Widget() = default;
  virtual void Render() = 0;
  Val<double> w, h, x, y;
};


