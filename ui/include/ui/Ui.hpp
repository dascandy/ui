#include <ui/Flexbox.hpp>
#include <ui/TextboxWidget.hpp>
#include <ui/LabelWidget.hpp>
#include <ui/Button.hpp>
#include <ui/TitleBarObject.hpp>

template <typename... Ts>
std::unique_ptr<Widget> vbox(Ts... ts) {
  std::unique_ptr<Flexbox> box = std::make_unique<Flexbox>();
  box->allowRelayout = false;
  box->set(Direction::Column);
  box->set(Wrap::Line);
  box->set(Justify::Start);
  box->set(Align::Stretch);
  (box->set(std::move(ts)), ...);
  box->allowRelayout = true;
  return box;
}

template <typename... Ts>
std::unique_ptr<Widget> hbox(Ts... ts) {
  std::unique_ptr<Flexbox> box = std::make_unique<Flexbox>();
  box->allowRelayout = false;
  box->set(Wrap::Line);
  box->set(Justify::Start);
  box->set(Align::Stretch);
  (box->set(std::move(ts)), ...);
  box->allowRelayout = true;
  return box;
}

template <typename... Ts>
std::unique_ptr<Widget> Label(Ts... ts) {
  std::unique_ptr<LabelWidget> label = std::make_unique<LabelWidget>();
  (label->set(ts), ...);
  return label;
}

template <typename... Ts>
std::unique_ptr<Widget> TextBox(Ts... ts) {
  std::unique_ptr<TextBoxWidget> box = std::make_unique<TextBoxWidget>();
  (box->set(ts), ...);
  return box;
}

template <typename... Ts>
std::unique_ptr<Widget> Button(Ts... ts) {
  std::unique_ptr<ButtonWidget> button = std::make_unique<ButtonWidget>();
  (button->set(ts), ...);
  return button;
}

template <typename... Ts>
std::unique_ptr<Widget> TitleBar(Ts... ts) {
  std::unique_ptr<TitleBarObject> title = std::make_unique<TitleBarObject>();
  (title->set(ts), ...);
  return title;
}

