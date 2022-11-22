#pragma once

#include <Var.h>
#include <cstdint>
#include <ui/Widget.hpp>
#include <vector>
#include <SDL2/SDL.h>

extern Val<int64_t> currentTimeMs; 

class Window
{
public:
  Window();
  void Swap();
  ~Window();
  bool quit = false;
  SDL_Window *window;
  SDL_GLContext context;
  Val<uint32_t> width, height, x, y;
  std::vector<Widget*> widgets;
};


