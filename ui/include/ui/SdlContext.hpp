#pragma once

#include <SDL2/SDL.h>

class SdlContext {
public:
  static SdlContext& Instance();
protected:
  SdlContext();
  ~SdlContext();
public:
  void Activate();
  void PollEvents(); // ??
protected:
  SDL_Window *window;
  SDL_GLContext context;
};


