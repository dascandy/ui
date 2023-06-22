#pragma once

#include <ui/VertexBuffer.hpp>

struct vertex {
  float x, y, z;
  float s, t;
  float r, g, b;
  bool operator==(const vertex &v) const {
    return !((*this < v) || (v < *this));
  }
  bool operator<(const vertex &v) const {
    return std::tie(x, y, z, s, t, r, g, b) < std::tie(v.x, v.y, v.z, v.s, v.t, v.r, v.g, v.b);
  }
  static std::vector<VertexBuffer::reference> fullrefs;
};


