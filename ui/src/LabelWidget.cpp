#include <ui/LabelWidget.hpp>
#include <ui/FontRender.hpp>
#include <ui/Theme.hpp>

LabelWidget::LabelWidget()
: face{Theme::Instance().GetFont(Theme::FontStyle::SansSerif), FontSizeUnit::Pixels, (size_t)**Theme::Instance().labelHeight}
, texture()
{
  w = Theme::Instance().labelWidth;
  h = Theme::Instance().labelHeight;
}

void LabelWidget::Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>&out) {
  float nw = **w;
  float nh = **h;
  if (nw < 0 || nh < 0) return;

  FontCache::Instance().Render(out[nullptr], text, face, parentX + **x - **w / 2, parentY + **y - **h / 2, **w, parentZ, Align::Start);
}

void LabelWidget::set(BoundText bText) {
  observer.emplace(bText.value, [this](const std::string& newText) { 
    text = newText; 
  });
}

void LabelWidget::Relayout() {
}

void LabelWidget::set(FixedText newText) {
  observer.reset();
  text = newText.value;
}

void LabelWidget::set(Justify newJustify) {
  justify = newJustify;
}

void LabelWidget::set(Align newAlign) {
  align = newAlign;
}



