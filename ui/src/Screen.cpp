#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <ui/Screen.hpp>
#include <ui/Window.hpp>
#include <ui/vertex.hpp>

#define SHADER(x) "#version 330 core\n" #x

static const char* svs = 
SHADER(
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 inTexCoord;
 
out vec2 texCoord;

uniform vec2 size;
uniform vec2 position;
uniform vec2 resolution;
uniform vec2 screenoffset;
uniform float windowid;

void main()
{
  vec2 abspos = (pos * size + position - screenoffset);
  gl_Position = vec4((2.0 * abspos) / resolution, windowid / 8192, 1.0);
  texCoord = inTexCoord;
}
);
static const char* fs = 
SHADER(

in vec2 texCoord;
uniform sampler2D tex;

layout (location = 0) out vec4 FragColor;

void main()
{
  FragColor = texture(tex, texCoord);
}
);

Screen::Screen()
: width(640)
, height(480)
, rt(640, 480, true)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

  window = SDL_CreateWindow("", 0, 0, width, height, SDL_WINDOW_OPENGL);
  if (not window) throw std::runtime_error("Cannot create window");

  context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(1);

  glewExperimental = GL_TRUE;
  if(glewInit() != GLEW_OK) throw std::runtime_error("glewInit failed");

  std::vector<std::tuple<glm::vec2, glm::vec2>> screen_square = {
    { { -1, -1 }, { 0, 0 } },
    { { -1,  1 }, { 0, 1 } },
    { {  1, -1 }, { 1, 0 } },
    { {  1, -1 }, { 1, 0 } },
    { { -1,  1 }, { 0, 1 } },
    { {  1,  1 }, { 1, 1 } },
  };
  svb = VertexBuffer(std::span<const std::tuple<glm::vec2, glm::vec2>>(screen_square.data(), screen_square.size()));
  screenShader = std::make_shared<ShaderProgram>("ScreenShader", svs, fs, nullptr, nullptr, nullptr);

  background = std::make_unique<Texture>("background.jpeg");
}

void Screen::Render()
{
  for (auto& win : windows) {
    win->Render();
  }
  rt.Activate();
  rt.Clear();
  screenShader->SetActive();

  float windowid = 0.0f;
  screenShader->Set("tex", *background.get());
  screenShader->Set("resolution", glm::vec2(width, height));
  screenShader->Set("screenoffset", glm::vec2(0, 0));
  screenShader->Set("size", glm::vec2(width, height));
  screenShader->Set("position", glm::vec2(0, 0));
  screenShader->Set("windowid", windowid);
//  svb.Draw();
  for (auto& win : windows) {
    windowid += 1.0f;
    screenShader->Set("tex", win->image);
    screenShader->Set("size", glm::vec2(**win->width, **win->height));
    screenShader->Set("position", glm::vec2(**win->x, **win->y));
    screenShader->Set("windowid", windowid);
    svb.Draw();
  }
  SDL_GL_SwapWindow(window);

  SDL_Event event;
  while (not quit && SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            quit = true;
            break;

          default:
            break;
        }
        break;
      case SDL_KEYUP:
        break;
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_CLOSE:   // exit game
            quit = true;
            break;
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            width = event.window.data1;
            height = event.window.data2;
            glViewport(0, 0, width, height);
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
}

Screen::~Screen()
{
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}


