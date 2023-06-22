#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <ui/Window.hpp>
#include <ui/vertex.hpp>
#include <ui/Time.hpp>
#include <ui/Theme.hpp>

#define SHADER(x) "#version 330 core\n" #x

static const char* vs = 
SHADER(
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inColor;
 
uniform vec2 size;
out vec2 texCoord;
out vec3 color;

void main()
{
  gl_Position = vec4(pos, 1.0) * vec4(2.0/size.x, -2.0/size.y, 1.0, 1.0);
  texCoord = inTexCoord;
  color = inColor;
}
);

static const char* fs = 
SHADER(

in vec2 texCoord;
in vec3 color;
uniform sampler2D tex;

layout (location = 0) out vec4 FragColor;

void main()
{
//  FragColor = texture(tex, texCoord) * vec4(color, 1);
  FragColor = texture(tex, texCoord);
}
);

Window::Window(std::unique_ptr<Widget> content)
: SdlWindow(640, 480, true)
, width(val(640u))
, height(val(480u))
, content(std::move(content))
, sp(__FILE__, vs, fs, nullptr, nullptr, nullptr)
, vb(vertex::fullrefs, sizeof(vertex), 3*65536)
{
  updateTime();

  this->content->w = varfunc<float>([](uint32_t w) { return w - 10.0f; }, varref(width));
  this->content->h = varfunc<float>([](uint32_t h) { return h - 10.0f; }, varref(height));
  this->content->Relayout();
}

void Window::UserInput(const SDL_Event& event, int deltaEvent) {
  (void)event;
  (void)deltaEvent;
  /*
  if (currentEvent > 0 && focusedWidget) {
    currentWindow->HandleEvent(event);
    currentEvent += deltaEvent;
    return;
  }

  currentEvent += deltaEvent;

  switch(event.type) {
    // Untargeted events, go to last targeted window
    case SDL_KEYDOWN:
    case SDL_MOUSEWHEEL:
      if (currentWindow) {
        currentWindow->HandleEvent(event);
      }
      break;
    case SDL_MOUSEMOTION:
      // update mouseover highlight?
      // update mouse cursor
      break;
    // targeting events
    case SDL_FINGERDOWN:
      break;
    case SDL_MOUSEBUTTONDOWN:
      break;
    default: 
      break;
  }
  currentWindow = nullptr;
  */
}

void Window::Resized(size_t newWidth, size_t newHeight) {
  this->width = val<uint32_t>(newWidth);
  this->height = val<uint32_t>(newHeight);
  this->content->Relayout();
}

void Window::Render() {
  static Texture white(1, 1, GL_RGBA8);
  static bool initialized = false;
  if (not initialized) {
    uint8_t b[4] = { 255, 255, 255, 255 };
    white.SetContent(1, 1, b);
    initialized = true;
  }

  rt.Activate();
  rt.Clear();

  std::map<Texture*, std::vector<vertex>> vertices;
  content->Render(0, 0, 0, vertices);
  sp.SetActive();
  sp.Set("size", glm::vec2(**width, **height));
  vb.Truncate();
  for (auto& [_, v] : vertices) {
    vb.Append(std::span<const vertex>(v.data(), v.size()));
  }

  size_t start = 0;
  for (auto& [t, v] : vertices) {
    if (t) {
      sp.Set("tex", *t);
    } else {
      sp.Set("tex", white);
    }
    vb.Draw(start, v.size());
    start += v.size();
  }
  SDL_GL_SwapWindow(window);
}

