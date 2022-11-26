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
  RenderTarget(bool depth, Texture *t1, Texture *t2, Texture *t3, Texture *t4);
  RenderTarget(bool depth, Texture *t1, Texture *t2, Texture *t3);
  RenderTarget(bool depth, Texture *t1, Texture *t2);
  RenderTarget(bool depth, Texture *t1);
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
  unsigned int rb;
  std::vector<Texture *> targets;
};


