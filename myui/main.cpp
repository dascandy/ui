#include <ui/Flexbox.hpp>
#include <ui/Image.hpp>
#include <ui/Window.hpp>
#include <ui/Screen.hpp>
#include <GL/glew.h>
#include <random>

int main() {
  Screen scr;
  std::random_device dev;
  std::mt19937 r(dev());
  std::shared_ptr<Texture> tex = Texture::Create(30, 30);
  Flexbox box(tex);
  box.allowRelayout = false;
  for (size_t n = 0; n < 18; n++) {
    std::unique_ptr<Image> i = std::make_unique<Image>(tex);
    i->w = val(10.0f + 20.0f * r() / 2147483648);
    i->h = val(10.0f + 20.0f * r() / 2147483648);
    box.addWidget(std::move(i));
  }
  box.w = val(640.0f);
  box.h = val(480.0f);
  Window win;
  win.widgets.push_back(&box);
  scr.windows.push_back(&win);
  for (auto direction : { Flexbox::Direction::Column, Flexbox::Direction::ColumnReverse, Flexbox::Direction::Row, Flexbox::Direction::RowReverse }) {
    box.setDirection(direction);
    for (auto wrap : { Flexbox::Wrap::Wrap, Flexbox::Wrap::Line, Flexbox::Wrap::WrapReverse}) {
      box.setWrap(wrap);
      /*
      for (auto justify : { Flexbox::Justify::Start, Flexbox::Justify::Center, Flexbox::Justify::End, Flexbox::Justify::SpaceAround, Flexbox::Justify::SpaceBetween, Flexbox::Justify::SpaceEvenly }) {
        box.setJustify(justify);
        for (auto vjustify : { Flexbox::Justify::Start, Flexbox::Justify::Center, Flexbox::Justify::End, Flexbox::Justify::SpaceAround, Flexbox::Justify::SpaceBetween, Flexbox::Justify::SpaceEvenly }) {
          box.setVerticalJustify(vjustify);
          for (auto align : { Flexbox::Align::Start, Flexbox::Align::Center, Flexbox::Align::End, Flexbox::Align::Stretch}) {
            box.setAlign(align);
            */
            printf("direction:%s wrap:%s justify:s vjustify:s align:s\n", to_string(direction).c_str(), to_string(wrap).c_str());//, to_string(justify).c_str(), to_string(vjustify).c_str(), to_string(align).c_str());
            box.Relayout();
            time_t start = time(nullptr);
            while (time(nullptr) - start < 5.0) {
              scr.Render();
              if (scr.quit) return 0;
            }
            /*
          }
        }
      }
      */
    }
  }
}


