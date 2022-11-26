#pragma once

#include <stddef.h>
#include <GL/glew.h>
#include <span>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

class ShaderProgram;

class VertexBuffer {
public:
  struct reference {
    int offset;
    int count;
    unsigned int type;
  };
  template <typename T> struct refs;
  template <> struct refs<float> { static constexpr reference def { 0, 1, GL_FLOAT };};
  template <> struct refs<glm::vec2> { static constexpr reference def { 0, 2, GL_FLOAT };};
  template <> struct refs<glm::vec3> { static constexpr reference def { 0, 3, GL_FLOAT };};
  template <> struct refs<glm::vec4> { static constexpr reference def { 0, 4, GL_FLOAT };};
  template <> struct refs<int> { static constexpr reference def { 0, 1, GL_INT };};
  template <> struct refs<glm::ivec2> { static constexpr reference def { 0, 2, GL_INT };};
  template <> struct refs<glm::ivec3> { static constexpr reference def { 0, 3, GL_INT };};
  template <> struct refs<glm::ivec4> { static constexpr reference def { 0, 4, GL_INT };};

  static size_t size(unsigned int gltype) {
    switch(gltype) {
      case GL_FLOAT: return 4;
      case GL_INT: return 4;
      default: return 0;
    }
  }

  template <typename... Ts>
  static std::vector<reference> construct() {
    auto refers = std::vector<reference>{refs<Ts>::def...};
    size_t offset = 0;
    for (auto& ref : refers) {
      ref.offset = offset;
      offset += ref.count * size(ref.type);
    }
    return refers;
  }

  VertexBuffer();
  template <typename... Ts>
  VertexBuffer(std::span<const std::tuple<Ts...>> objs)
  : VertexBuffer(construct<Ts...>(), objs)
  {}
  VertexBuffer(std::span<const reference> refs, size_t sizeOfElement, size_t countToReserve)
  : count(countToReserve)
  , stride(sizeOfElement)
  {
    Construct(refs);
    glBufferData(GL_ARRAY_BUFFER, stride*countToReserve, nullptr, GL_DYNAMIC_DRAW);
  }
  template <typename T>
  VertexBuffer(std::span<const reference> refs, std::span<const T> objs)
  : count(objs.size())
  , stride(sizeof(T))
  {
    Construct(refs);
    glBufferData(GL_ARRAY_BUFFER, stride*objs.size(), objs.data(), GL_DYNAMIC_DRAW);
  }
  template <typename T>
  void replace(std::span<const T> objs) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, stride*objs.size(), objs.data(), GL_DYNAMIC_DRAW);
    count = objs.size();
  }
  template <typename T>
  void Append(std::span<const T> objs) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, count * sizeof(T), objs.size() * sizeof(T), objs.data());
    count += objs.size();
  }
  void Truncate() {
    count = 0;
  }
  ~VertexBuffer();
  VertexBuffer(VertexBuffer&&);
  VertexBuffer& operator=(VertexBuffer&&);
  void Draw(size_t start = 0, size_t count = 0);
  void Construct(std::span<const reference> refs);
  unsigned int vbo, vao;
private:
  unsigned int count;
  unsigned int stride;
};


