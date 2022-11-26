#pragma once

#include <ui/VertexBuffer.hpp>

struct vertex {
  float x, y, z;
  float nx, ny, nz;
  float s, t;
  bool operator==(const vertex &v) const {
    return !((*this < v) || (v < *this));
  }
  bool operator<(const vertex &v) const {
    return std::tie(x, y, z, nx, ny, nz, s, t) < std::tie(v.x, v.y, v.z, v.nx, v.ny, v.nz, v.s, v.t);
  }
  static constexpr VertexBuffer::reference fullrefs[] =
  {
    {0, 3, GL_FLOAT},
    {12, 3, GL_FLOAT},
    {24, 2, GL_FLOAT},
  };  
};


