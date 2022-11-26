#pragma once

#include <Var.h>
#include <cstdint>
#include <ui/Widget.hpp>
#include <vector>
#include <SDL2/SDL.h>
#include <ui/RenderTarget.hpp>
#include <ui/ShaderProgram.hpp>

extern Val<int64_t> currentTimeMs; 

class Window
{
public:
  Window();
  void Render();
  ~Window();
  bool quit = false;
  SDL_Window *window;
  SDL_GLContext context;
  Val<uint32_t> width, height, x, y;
  std::vector<Widget*> widgets;
  ShaderProgram sp;
  Texture image;
  RenderTarget rt;
  VertexBuffer vb;
};


