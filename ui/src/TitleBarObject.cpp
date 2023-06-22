#include <ui/TitleBarObject.hpp>
#include <ui/FontRender.hpp>
#include <ui/Theme.hpp>

TitleBarObject::TitleBarObject()
{
  h = val(20.0f);
  title.w = varref(w);
  title.h = varref(h);
  title.set(Justify::Center);
}

void TitleBarObject::Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>&out) {
  title.Render(parentX + **x, parentY + **y, parentZ, out);
  for (auto& button : buttons) {
    button->Render(parentX + **x, parentY + **y, parentZ, out);
  }
}

void TitleBarObject::set(TitleBarButton button) {
  buttons.push_back(std::make_unique<ButtonWidget>());
  buttons.back()->set(Width{20});
  buttons.back()->set(Height{20});
  switch(button) {
    case MinimizeButton: buttons.back()->set(Text("_")); break;
    case MaximizeButton: buttons.back()->set(Text("O")); break;
    case XButton: buttons.back()->set(Text("X")); break;
  }
  for (size_t n = 0; n < buttons.size(); n++) {
    buttons[n]->x = varfunc<float>([place = buttons.size() - n](float parentW){ return parentW / 2 + 10 - 22 * place; }, varref(w));
  }
}

void TitleBarObject::set(BoundText bText) {
  title.set(std::move(bText));
}

void TitleBarObject::set(FixedText newText) {
  title.set(std::move(newText));
}


