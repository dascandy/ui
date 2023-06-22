#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <ui/Window.hpp>
#include <ui/vertex.hpp>
#include <ui/Time.hpp>
#include <ui/Theme.hpp>
#include <ui/SdlContext.hpp>

SdlWindow::SdlWindow(size_t width, size_t height, bool resizable) 
: rt(width, height, true)
{
  SdlContext::Instance().Activate();

  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
  window = SDL_CreateWindow("", 0, 0, width, height, SDL_WINDOW_OPENGL | (resizable ? SDL_WINDOW_RESIZABLE : 0));
  if (not window) throw std::runtime_error("Cannot create window");

  context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(1);
  glEnable(GL_MULTISAMPLE);
}

SdlWindow::~SdlWindow() {
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
}

void SdlWindow::SetTitle(const std::string& sv) {
  SDL_SetWindowTitle(window, sv.c_str());
}

void SdlWindow::PollEvents() {
  updateTime();

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYUP:
      case SDL_FINGERUP:
        UserInput(event, -1);
        break;
      case SDL_KEYDOWN:
      case SDL_FINGERDOWN:
        UserInput(event, 1);
        break;
      case SDL_FINGERMOTION:
        UserInput(event, 0);
        break;
      case SDL_MOUSEMOTION:
        if (((SDL_MouseMotionEvent*)(&event))->which != SDL_TOUCH_MOUSEID) {
          UserInput(event, 0);
        }
        break;
      case SDL_MOUSEWHEEL:
        if (((SDL_MouseWheelEvent*)(&event))->which != SDL_TOUCH_MOUSEID) {
          UserInput(event, 0);
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        if (((SDL_MouseButtonEvent*)(&event))->which != SDL_TOUCH_MOUSEID) {
          UserInput(event, event.type == SDL_MOUSEBUTTONDOWN ? 1 : -1);
        }
        break;
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_CLOSE:
            done = true;
            break;
          case SDL_WINDOWEVENT_SIZE_CHANGED:
          {
            size_t width = event.window.data1;
            size_t height = event.window.data2;
            glViewport(0, 0, width, height);
            rt.Resize(width, height);
            Resized(width, height);
          }
            break;
          default:
            break;
        }
        break;
      case SDL_QUIT:
        done = true;
        break;
      default:
        break;
    }
  }
}

