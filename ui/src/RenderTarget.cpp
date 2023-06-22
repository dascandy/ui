#include <ui/RenderTarget.hpp>
#include <cstdio>

static const unsigned int buffers[] = 
{
  GL_COLOR_ATTACHMENT0,
  GL_COLOR_ATTACHMENT1,
  GL_COLOR_ATTACHMENT2,
  GL_COLOR_ATTACHMENT3,
  GL_COLOR_ATTACHMENT4,
  GL_COLOR_ATTACHMENT5,
  GL_COLOR_ATTACHMENT6,
  GL_COLOR_ATTACHMENT7,
  GL_COLOR_ATTACHMENT8,
  GL_COLOR_ATTACHMENT9,
  GL_COLOR_ATTACHMENT10,
  GL_COLOR_ATTACHMENT11,
  GL_COLOR_ATTACHMENT12,
  GL_COLOR_ATTACHMENT13,
  GL_COLOR_ATTACHMENT14,
  GL_COLOR_ATTACHMENT15
};

RenderTarget::RenderTarget(bool depth, Texture *t1, Texture *t2, Texture *t3, Texture *t4)
: RenderTarget(depth, t1, t2, t3)
{
  AddTarget(t4);
}

RenderTarget::RenderTarget(bool depth, Texture *t1, Texture *t2, Texture *t3) 
: RenderTarget(depth, t1, t2)
{
  AddTarget(t3);
}

RenderTarget::RenderTarget(bool depth, Texture *t1, Texture *t2) 
: RenderTarget(depth, t1)
{
  AddTarget(t2);
}

RenderTarget::RenderTarget(bool depth, Texture *t1)
: depth(depth)
, width(t1->width())
, height(t1->height())
, fbo(0)
{
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  if (depth) {
    glGenRenderbuffers(1, &rb);
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);    
  }

  AddTarget(t1);
}

RenderTarget::RenderTarget(int width, int height, bool depth)
: depth(depth)
, width(width)
, height(height)
, fbo(0)
{
}

void RenderTarget::AddTarget(Texture *target)
{
  targets.push_back(target);
}

RenderTarget::~RenderTarget()
{
  if (fbo)
    glDeleteFramebuffers(1, &fbo);
}

void RenderTarget::Resize(size_t newWidth, size_t newHeight) {
  width = newWidth;
  height = newHeight;
}

void RenderTarget::Activate(BlendMode bm)
{
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  if (targets.size())
  {
    for (size_t i = 0; i < targets.size(); i++)
    {
      glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, targets[i]->gl_id(), 0);
    }
    glDrawBuffers(targets.size(), buffers);
  }

  if (depth)
  {
    if (targets.size()) {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);
    }
    glEnable(GL_DEPTH_TEST);
  }
  else
  {
    glDisable(GL_DEPTH_TEST);
  }

  switch(bm)
  {
  case Overwrite:
    glDisable(GL_BLEND);
    break;
  case Alphablend:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    break;
  case Additive:
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_BLEND);
    break;
  }

  int err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  switch(err)
  {
  case GL_FRAMEBUFFER_COMPLETE: break;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: std::terminate(); break;
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:std::terminate(); break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:std::terminate(); break;
  case GL_FRAMEBUFFER_UNSUPPORTED:std::terminate(); break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:std::terminate(); break;
  case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB:std::terminate(); break;
  case GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB:std::terminate(); break;
  default: std::terminate(); break;
  }
}

void RenderTarget::Clear()
{
  Activate();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderTarget::ClearDepth()
{
  Activate();
  glClear(GL_DEPTH_BUFFER_BIT);
}


