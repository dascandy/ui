#include <ui/VertexBuffer.hpp>
#include <ui/ShaderProgram.hpp>

VertexBuffer::VertexBuffer() 
: vbo(4294967295)
, vao(4294967295)
, count(0)
, stride(0)
{
}

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

VertexBuffer::VertexBuffer(VertexBuffer&& o)
: vbo(o.vbo)
, vao(o.vao)
, count(o.count)
, stride(o.stride)
{
  o.vbo = 4294967295;
  o.vao = 4294967295;
  o.count = 0;
  o.stride = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& o) {
  if (vbo != 4294967295)
    glDeleteBuffers(1, &vbo);
  if (vao != 4294967295)
    glDeleteVertexArrays(1, &vao);

  vbo = o.vbo;
  vao = o.vao;
  count = o.count;
  stride = o.stride;

  o.vbo = 4294967295;
  o.vao = 4294967295;
  o.count = 0;
  o.stride = 0;
  return *this;
}

VertexBuffer::~VertexBuffer()
{
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}

void VertexBuffer::Draw(size_t start, size_t rendercount)
{
  if (rendercount == 0) {
    rendercount = count - start;
  }
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, start, rendercount);
}


