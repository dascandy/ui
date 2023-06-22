#include <ui/Flexbox.hpp>
#include <ui/Image.hpp>
#include <ui/Window.hpp>
#include <GL/glew.h>
#include <random>
#include <ui/Theme.hpp>
#include <ui/Time.hpp>
#include <ui/Icons.hpp>
#include <ui/Ui.hpp>

struct Entry {
  std::string title;
  std::string text;
};

std::vector<Entry> entries;

struct MyWindow {
  Observed<std::string> currentUrl = "http://example.com/";
  Observed<std::string> currentTitle = "Hello World";
  Observed<std::string> currentText = "0123456789    ABCDEFGHIJKLMNOPQRSTUVWXYZ    abcdefghijklmnopqrstuvwxyz";
  int index = 0;
  Window win;

  MyWindow()
  : win(vbox(Elements (
      TitleBar(Text("RSS Reader"), MinimizeButton, MaximizeButton, XButton),
      hbox(Height{30}, Elements (
        Label(Justify::End, Text("URL:")),
        TextBox(Stretch{}, Text(currentUrl), OnKey{'\n', [this]{ Go(); }}),
        Button(Text("Go"), OnClick{[this]{ Go(); }})
      )),
      Label(Text(currentTitle)),
      Label(Stretch{}, Text(currentText)),
      hbox(Height{30}, Justify::End, Elements (
        Button(Text("Previous"), OnClick{[this]{ Navigate(-1); }}),
        Button(Text("Next"), OnClick{[this]{ Navigate(1); }})
      ))
    )))
  {
    win.SetTitle("Yolo");
  }
  void Navigate(int delta) {
    index = (index + delta + entries.size()) % entries.size();
    currentText = entries[index].text;
    currentTitle = entries[index].title;
  }
  void Go() {
  }
  void Draw() {
    win.Render();
    win.PollEvents();
  }
};

int main() {
  MyWindow win;
  while (true) {
    win.Draw();
    if (win.win.done) break;
  }
}




