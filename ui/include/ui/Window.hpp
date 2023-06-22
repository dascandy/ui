#pragma once

#include <Var.h>
#include <cstdint>
#include <ui/Widget.hpp>
#include <vector>
#include <SDL2/SDL.h>
#include <ui/RenderTarget.hpp>
#include <ui/ShaderProgram.hpp>

extern Val<int64_t> currentTimeMs; 

class SdlWindow {
protected:
  SdlWindow(size_t width, size_t height, bool resizable);
  ~SdlWindow();
  virtual void UserInput(const SDL_Event& event, int deltaEvent) = 0;
  virtual void Resized(size_t newWidth, size_t newHeight) = 0;
public:
  void HandleEvent(const SDL_Event& event);
  void PollEvents();
  void SetTitle(const std::string& sv);
  void SetResizable(bool resizable);
protected:
  SDL_Window *window;
  SDL_GLContext context;
public:
  bool done = false;
  RenderTarget rt;
};

class Window : public SdlWindow
{
public:
  Window(std::unique_ptr<Widget> content);
  ~Window() = default;
  void Render();
  void UserInput(const SDL_Event& event, int deltaEvent) override;
  void Resized(size_t newWidth, size_t newHeight) override;
  using SdlWindow::PollEvents;
  using SdlWindow::done;
  Val<uint32_t> width, height;
  std::unique_ptr<Widget> content;
  ShaderProgram sp;
  VertexBuffer vb;
  int currentEvent = 0;
};


