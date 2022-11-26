#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <ui/Window.hpp>
#include <ui/vertex.hpp>

#define SHADER(x) #x

static const char* vs = 
"#version 330 core\n"
SHADER(
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 inTexCoord;
 
uniform vec2 size;
out vec2 texCoord;

void main()
{
  gl_Position = vec4((pos.xy) / size, 0.0, 1.0);
  texCoord = inTexCoord;
}
);
static const char* fs = 
"#version 330 core\n"
SHADER(

in vec2 texCoord;
uniform sampler2D tex;

layout (location = 0) out vec4 FragColor;

void main()
{
//  FragColor = texture(tex, texCoord);
  FragColor = vec4(1, 0, 1, 0);
}
);

Window::Window()
: width(val(500u))
, height(val(300u))
, x(val(0u))
, y(val(0u))
, sp(__FILE__, vs, fs, nullptr, nullptr, nullptr)
, image(**width, **height)
, rt(true, &image)
, vb(vertex::fullrefs, sizeof(vertex), 3*65536)
{
}

void Window::Render() {
  std::map<Texture*, std::vector<vertex>> vertices;
  for (auto& widget : widgets) {
    widget->Render(0, 0, 0, vertices);
  }
  rt.Activate();
  rt.Clear();
  sp.SetActive();
  sp.Set("size", glm::vec2(**width, **height));
  vb.Truncate();
  for (auto& [_, v] : vertices) {
    vb.Append(std::span<const vertex>(v.data(), v.size()));
  }

  size_t start = 0;
  for (auto& [t, v] : vertices) {
    sp.Set("tex", *t);
    vb.Draw(start, v.size());
    start += v.size();
  }
}

Window::~Window() = default;


