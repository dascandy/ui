#pragma once

#include <Var.h>
#include <cstdint>
#include <ui/Widget.hpp>
#include <vector>
#include <SDL2/SDL.h>
#include <ui/RenderTarget.hpp>
#include <ui/ShaderProgram.hpp>

extern Val<int64_t> currentTimeMs; 

class Window;

class Screen
{
public:
  Screen();
  void Render();
  ~Screen();
  bool quit = false;
  SDL_Window *window;
  SDL_GLContext context;
  uint32_t width, height;
  std::vector<Window*> windows;
  std::unique_ptr<Texture> background;
  VertexBuffer svb;
  std::shared_ptr<ShaderProgram> screenShader;
  RenderTarget rt;
};


