#include <ui/Button.hpp>
#include <ui/Theme.hpp>

ButtonWidget::ButtonWidget() 
: onclick([]{})
{
  w = Theme::Instance().buttonWidth;
  h = Theme::Instance().buttonHeight;
  label.w = varfunc<float>([](float w) { return w - 10; }, varref(w));
  label.h = varfunc<float>([](float h) { return h - 10; }, varref(h));
}

void ButtonWidget::Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>& out) {
  Theme::Instance().CreateRegButton(out, parentZ, parentX + **x, parentY + **y, **w, **h);
  label.Render(parentX + **x, parentY + **y, parentZ - 0.0001, out);
}

void ButtonWidget::set(FixedText text) {
  label.set(std::move(text));
}

void ButtonWidget::set(OnClick handler) {
  onclick = handler.handler;
}


