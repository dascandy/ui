#pragma once

#include <vector>
#include <ui/Texture.hpp>
#include <memory>

class RenderTarget {
public:
  enum BlendMode {
    Overwrite,
    Alphablend,
    Additive
  };
  RenderTarget(Texture *d, Texture *t1, Texture *t2, Texture *t3, Texture *t4);
  RenderTarget(Texture *d, Texture *t1, Texture *t2, Texture *t3);
  RenderTarget(Texture *d, Texture *t1, Texture *t2);
  RenderTarget(Texture *d, Texture *t1);
  RenderTarget(int width, int height, bool depth);
  void Clear();
  void ClearDepth();
  void Activate(BlendMode bm = Overwrite);
  void AddTarget(Texture *target);
  ~RenderTarget();
private:
  RenderTarget(const RenderTarget&) = delete;
  RenderTarget(RenderTarget&&) = delete;
  const RenderTarget& operator=(const RenderTarget&) = delete;
  const RenderTarget& operator=(RenderTarget&&) = delete;
  bool depth;
  int width, height;
  int vx, vy, vw, vh;
  unsigned int fbo;
  Texture *d;
  std::vector<Texture *> targets;
};


