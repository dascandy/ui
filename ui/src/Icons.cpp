#include <string>
#include <vector>
#include <ui/Icons.hpp>
#include <ui/Texture.hpp>
#include <ui/SubTexture.hpp>

struct IconEntry {
  std::string name;
  uint32_t x, y, w, h;
};

static std::vector<IconEntry> iconInputs{{
IconEntry{"arrowDown", 0, 1200, 100, 100},
IconEntry{"arrowLeft", 0, 1100, 100, 100},
IconEntry{"arrowRight", 0, 1000, 100, 100},
IconEntry{"arrowUp", 0, 900, 100, 100},
IconEntry{"audioOff", 0, 800, 100, 100},
IconEntry{"audioOn", 0, 700, 100, 100},
IconEntry{"barsHorizontal", 0, 600, 100, 100},
IconEntry{"barsVertical", 0, 500, 100, 100},
IconEntry{"button1", 0, 400, 100, 100},
IconEntry{"button2", 0, 300, 100, 100},
IconEntry{"button3", 0, 200, 100, 100},
IconEntry{"buttonA", 0, 100, 100, 100},
IconEntry{"buttonB", 0, 0, 100, 100},
IconEntry{"buttonL", 200, 1200, 100, 100},
IconEntry{"buttonL1", 500, 300, 100, 100},
IconEntry{"buttonL2", 500, 200, 100, 100},
IconEntry{"buttonR", 500, 100, 100, 100},
IconEntry{"buttonR1", 500, 0, 100, 100},
IconEntry{"buttonR2", 400, 1900, 100, 100},
IconEntry{"buttonSelect", 400, 1800, 100, 100},
IconEntry{"buttonStart", 400, 1700, 100, 100},
IconEntry{"buttonX", 400, 1600, 100, 100},
IconEntry{"buttonY", 400, 1500, 100, 100},
IconEntry{"checkmark", 400, 1400, 100, 100},
IconEntry{"contrast", 400, 1300, 100, 100},
IconEntry{"cross", 400, 1200, 100, 100},
IconEntry{"down", 400, 1100, 100, 100},
IconEntry{"downLeft", 400, 1000, 100, 100},
IconEntry{"downRight", 400, 900, 100, 100},
IconEntry{"exclamation", 400, 800, 100, 100},
IconEntry{"exit", 400, 700, 100, 100},
IconEntry{"exitLeft", 400, 600, 100, 100},
IconEntry{"exitRight", 400, 500, 100, 100},
IconEntry{"export", 400, 400, 100, 100},
IconEntry{"fastForward", 400, 300, 100, 100},
IconEntry{"gamepad", 400, 200, 100, 100},
IconEntry{"gamepad1", 400, 100, 100, 100},
IconEntry{"gamepad2", 400, 0, 100, 100},
IconEntry{"gamepad3", 300, 1900, 100, 100},
IconEntry{"gamepad4", 300, 1800, 100, 100},
IconEntry{"gear", 300, 1700, 100, 100},
IconEntry{"home", 300, 1600, 100, 100},
IconEntry{"import", 300, 1500, 100, 100},
IconEntry{"information", 300, 1400, 100, 100},
IconEntry{"joystick", 300, 1300, 100, 100},
IconEntry{"joystickLeft", 300, 1200, 100, 100},
IconEntry{"joystickRight", 300, 1100, 100, 100},
IconEntry{"joystickUp", 300, 1000, 100, 100},
IconEntry{"larger", 300, 900, 100, 100},
IconEntry{"leaderboardsComplex", 300, 800, 100, 100},
IconEntry{"leaderboardsSimple", 300, 700, 100, 100},
IconEntry{"left", 300, 600, 100, 100},
IconEntry{"locked", 300, 500, 100, 100},
IconEntry{"massiveMultiplayer", 300, 400, 100, 100},
IconEntry{"medal1", 300, 300, 100, 100},
IconEntry{"medal2", 300, 200, 100, 100},
IconEntry{"menuGrid", 300, 100, 100, 100},
IconEntry{"menuList", 300, 0, 100, 100},
IconEntry{"minus", 200, 1900, 100, 100},
IconEntry{"mouse", 200, 1800, 100, 100},
IconEntry{"movie", 200, 1700, 100, 100},
IconEntry{"multiplayer", 200, 1600, 100, 100},
IconEntry{"musicOff", 200, 1500, 100, 100},
IconEntry{"musicOn", 200, 1400, 100, 100},
IconEntry{"next", 200, 1300, 100, 100},
IconEntry{"open", 500, 400, 100, 100},
IconEntry{"pause", 200, 1100, 100, 100},
IconEntry{"phone", 200, 1000, 100, 100},
IconEntry{"plus", 200, 900, 100, 100},
IconEntry{"power", 200, 800, 100, 100},
IconEntry{"previous", 200, 700, 100, 100},
IconEntry{"question", 200, 600, 100, 100},
IconEntry{"return", 200, 500, 100, 100},
IconEntry{"rewind", 200, 400, 100, 100},
IconEntry{"right", 200, 300, 100, 100},
IconEntry{"save", 200, 200, 100, 100},
IconEntry{"scrollHorizontal", 200, 100, 100, 100},
IconEntry{"scrollVertical", 200, 0, 100, 100},
IconEntry{"share1", 100, 1900, 100, 100},
IconEntry{"share2", 100, 1800, 100, 100},
IconEntry{"shoppingBasket", 100, 1700, 100, 100},
IconEntry{"shoppingCart", 100, 1600, 100, 100},
IconEntry{"siganl1", 100, 1500, 100, 100},
IconEntry{"signal2", 100, 1400, 100, 100},
IconEntry{"signal3", 100, 1300, 100, 100},
IconEntry{"singleplayer", 100, 1200, 100, 100},
IconEntry{"smaller", 100, 1100, 100, 100},
IconEntry{"star", 100, 1000, 100, 100},
IconEntry{"stop", 100, 900, 100, 100},
IconEntry{"tablet", 100, 800, 100, 100},
IconEntry{"target", 100, 700, 100, 100},
IconEntry{"trashcan", 100, 600, 100, 100},
IconEntry{"trashcanOpen", 100, 500, 100, 100},
IconEntry{"trophy", 100, 400, 100, 100},
IconEntry{"unlocked", 100, 300, 100, 100},
IconEntry{"up", 100, 200, 100, 100},
IconEntry{"upLeft", 100, 100, 100, 100},
IconEntry{"upRight", 100, 0, 100, 100},
IconEntry{"video", 0, 1900, 100, 100},
IconEntry{"warning", 0, 1800, 100, 100},
IconEntry{"wrench", 0, 1700, 100, 100},
IconEntry{"zoom", 0, 1600, 100, 100},
IconEntry{"zoomDefault", 0, 1500, 100, 100},
IconEntry{"zoomIn", 0, 1400, 100, 100},
IconEntry{"zoomOut", 0, 1300, 100, 100},
}};

std::optional<SubTexture> GetIcon(const std::string& name) {
  static std::shared_ptr<Texture> texture = std::make_shared<Texture>("res/sheet_white_icons.png");
  static std::map<std::string, SubTexture> icons;
  if (icons.empty()) {
    for (auto& [name, x, y, w, h] : iconInputs) {
      icons.emplace(name, SubTexture{texture, (float)x / texture->width(), (float)y / texture->height(), (float)w / texture->width(), (float)h / texture->height()});
    }
  }
  auto it = icons.find(name);
  if (it == icons.end()) return std::nullopt;
  return it->second;
}


