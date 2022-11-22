#include <ui/VertexBuffer.hpp>
#include <ui/ShaderProgram.hpp>

void VertexBuffer::Construct(std::span<const VertexBuffer::reference> refs)
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  int i = 0;
  for (auto& ref : refs) {
    glEnableVertexAttribArray(i);
    int type = ref.type;
    if (type == GL_INT)
      glVertexAttribIPointer(i, ref.count, ref.type, stride, (void *)(intptr_t)ref.offset);
    else
      glVertexAttribPointer(i, ref.count, ref.type, GL_FALSE, stride, (void *)(intptr_t)ref.offset);
    ++i;
  }
}

VertexBuffer::~VertexBuffer()
{
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}

void VertexBuffer::Draw()
{
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, count);
}


