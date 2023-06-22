#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <ui/SdlContext.hpp>
#include <stdexcept>

SdlContext& SdlContext::Instance() {
  static SdlContext context;
  return context;
}

SdlContext::SdlContext()
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

  window = SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
  if (not window) throw std::runtime_error("Cannot create window");

  context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(1);

  glewExperimental = GL_TRUE;
  if(glewInit() != GLEW_OK) throw std::runtime_error("glewInit failed");
}

SdlContext::~SdlContext() {
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void SdlContext::Activate() {
  SDL_GL_MakeCurrent(window, context);
}

void SdlContext::PollEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
  }
}

