#include <ui/TextboxWidget.hpp>
#include <ui/Theme.hpp>

TextBoxWidget::TextBoxWidget() 
{
  w = Theme::Instance().textboxWidth;
  h = Theme::Instance().textboxHeight;
}

void TextBoxWidget::Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>& out) {
  Theme::Instance().CreateWhiteBox(out, parentZ, parentX + **x, parentY + **y, **w, **h);
}

void TextBoxWidget::set(BoundText text) {
  boundText.emplace(text);
  observer.emplace(text.value, [this](std::string& t) { if (not myUpdate) this->text = t; });
}

void TextBoxWidget::set(FixedText text) {
  observer.reset();
  boundText.reset();
  this->text = std::move(text.value);
}

void TextBoxWidget::set(OnKey keyHandler) {
  keyHandlers.push_back(std::move(keyHandler));
}

void TextBoxWidget::SetFocused(bool focused) {
  if (focused) {
    /*
    SDL_Rect rect;
    rect.x = **x - **w / 2;
    rect.y = **y - **h / 2;
    rect.w = **w;
    rect.h = **h;
    SDL_SetTextInputRect(rect);
    */
    SDL_StartTextInput();
  } else {
    SDL_StopTextInput();
  }
}

void TextBoxWidget::HandleEvent(SDL_Event& event) {
  
}

void TextBoxWidget::UpdateBoundText() {
  if (boundText) {
    myUpdate = true;
    ***(boundText->value) = text;
    myUpdate = false;
  }
}


