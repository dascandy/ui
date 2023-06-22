#include <ui/Theme.hpp>
#include <ui/Texture.hpp>
#include <Var.h>
#include <ui/Window.hpp>
#include <ui/SdlContext.hpp>

static std::unique_ptr<Theme> _instance;

Theme& Theme::Instance() {
  if (not _instance) {
    SdlContext::Instance().Activate();
    Theme::SetTheme(std::make_unique<Theme>(Theme::Color::Red));
  }
  return *_instance;
}

void Theme::SetTheme(std::unique_ptr<Theme> theme) {
  _instance = std::move(theme);
}

Theme::Theme(Color color)
: texture(std::make_unique<Texture>("res/theme.png"))
, color(color)
{
  buttonWidth = val<float>(160.0f);
  buttonHeight = val<float>(30.0f);
  textboxWidth = val<float>(200.0f);
  textboxHeight = val<float>(30.0f);
  labelWidth = val<float>(80.0f);
  labelHeight = val<float>(20.0f);
}

std::vector<float> expand(std::span<const float> in) {
  std::vector<float> rv;
  switch(in.size()) {
    case 2:
      rv.push_back(in.front());
      rv.push_back(in.front());
      rv.push_back(in.back());
      rv.push_back(in.back());
      break;
    case 3:
      rv.push_back(in.front());
      rv.push_back(in[1]);
      rv.push_back(in[1]);
      rv.push_back(in.back());
      break;
    case 4:
      rv = std::vector<float>(in.begin(), in.end());
      break;
  }
  return rv;
}

void Theme::generateSquare(std::map<Texture*, std::vector<vertex>>& out, Texture* tex, float z, float x1, float x2, float y1, float y2, float s1, float s2, float t1, float t2) {
  out[tex].push_back({x1, y1, z, s1/tex->width(), t1/tex->height(), 1, 1, 1 });
  out[tex].push_back({x1, y2, z, s1/tex->width(), t2/tex->height(), 1, 1, 1 });
  out[tex].push_back({x2, y1, z, s2/tex->width(), t1/tex->height(), 1, 1, 1 });
  out[tex].push_back({x2, y1, z, s2/tex->width(), t1/tex->height(), 1, 1, 1 });
  out[tex].push_back({x1, y2, z, s1/tex->width(), t2/tex->height(), 1, 1, 1 });
  out[tex].push_back({x2, y2, z, s2/tex->width(), t2/tex->height(), 1, 1, 1 });
}

void Theme::generateNinepatch(std::map<Texture*, std::vector<vertex>>& out, Texture* tex, float z, std::span<const float> in_s, std::span<const float> in_t, float x, float y, float w, float h) {
  std::vector<float> s = expand(in_s), t = expand(in_t);
  std::vector<float> xv{x-w/2, x-w/2 + s[1]-s[0], x + w/2 - (s[3]-s[2]), x + w/2}, 
                     yv{y-h/2, y-h/2 + t[1]-t[0], y + h/2 - (t[3]-t[2]), y + h/2};
  for (size_t dy = 0; dy < 3; dy++) {
    for (size_t dx = 0; dx < 3; dx++) {
      if ((xv[dx] == xv[dx+1]) || (yv[dy] == yv[dy+1])) continue;
      generateSquare(out, tex, z, xv[dx], xv[dx+1], yv[dy], yv[dy+1], s[dx], s[dx+1], t[dy], t[dy+1]);
    }
  }
}

void Theme::CreateCheckbox(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h, Checked value) {
  generateSquare(out, texture.get(), z, x, x+w, y, y+h, 1.0f, 37.0f, 1.0f, 37.0f);

  switch(value) {
    case Checked::Check:
      generateSquare(out, texture.get(), z+1, x + 0.16666 * w, x + 0.83333 * w, y + 0.194444 * h, y + 0.805555 * h, 58 + coloroffset(), 82 + coloroffset(), 35, 57);
      break;
    case Checked::Cross:
      generateSquare(out, texture.get(), z+1, x + 0.25 * w, x + 0.75 * w, y + 0.25 * h, y + 0.75 * h, 61 + coloroffset(), 79 + coloroffset(), 58, 76);
      break;
    default:
      break;
  }
}

void Theme::CreateOptionbox(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h, Selected value) {
  generateSquare(out, texture.get(), z, x, x+w, y, y+h, 1, 39, 37, 75);

  if (value == Selected::Selected)
    generateSquare(out, texture.get(), z+1, x + 0.2763 * w, x + 0.7236 * w, y + 0.2763 * h, y + 0.7236 * h, 61 + coloroffset(), 78 + coloroffset(), 78, 95);
}

void Theme::CreateWhiteBox(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h) {
  generateNinepatch(out, texture.get(), z, {{ 10.0f, 19.0f, 28.0f }}, {{ 79.0f, 88.0f, 96.0f }}, x, y, w, h);
}

void Theme::CreateGradientButton(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h) {
  generateNinepatch(out, texture.get(), z, {{ 39.0f, 46.0f, 53.0f }}, {{ 1.0f, 7.0f, 40.0f, 46.0f }}, x, y, w, h);
}

void Theme::CreateRegButton(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h) {
  generateNinepatch(out, texture.get(), z, {{ 39.0f, 46.0f, 53.0f }}, {{ 47.0f, 54.0f, 60.0f }}, x, y, w, h);
}

void Theme::CreateColorBox(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h) {
  generateNinepatch(out, texture.get(), z, {{ 39.0f, 46.0f, 53.0f }}, {{ 62.0f, 69.0f, 76.0f }}, x, y, w, h);
}

void Theme::CreateRegbuttonRaised(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float w, float h) {
  generateNinepatch(out, texture.get(), z, {{ 39.0f, 46.0f, 53.0f }}, {{ 78.0f, 84.0f, 94.0f }}, x, y, w, h);
}

void Theme::CreateScroll(std::map<Texture*, std::vector<vertex>>& out, float z, float x, float y, float h) {
  generateNinepatch(out, texture.get(), z, {{ 55.0f, 83.0f }}, {{ 1.0f, 16.0f, 34.0f }}, x, y, 28, h);
}

float Theme::coloroffset() {
  return 46 * (int)color;
}

Val<float> Theme::splineTo(Val<float> from, Val<float> to) {
  int64_t currentTime = **currentTimeMs;
  return varswitch<float>(currentTimeMs, currentTime+1000, varfunc<float>([startTime = currentTime](float a, float b) {
    float t = (1 - cos((**currentTimeMs-startTime) / 318.0f)) / 2.0f;
    return a*(1-t) + b*t;
  }, from, to), to);
}

Font* Theme::GetFont(Theme::FontStyle style) {
  switch(style) {
    case Theme::FontStyle::Monospace:
    case Theme::FontStyle::Serif:
    case Theme::FontStyle::SansSerif:
    case Theme::FontStyle::Playful:
    case Theme::FontStyle::Flashy:
    case Theme::FontStyle::Dropcap:
    default:

// Gat in R ontbreekt?
//      return FontCache::Instance().loadFont("res/Fonts/Kenney Mini Square Mono.ttf");
// meerdere gaten ontbreken - 0689ABDOPQRabdopqr
//      return FontCache::Instance().loadFont("res/Fonts/Kenney_Blocks.ttf");

//      return FontCache::Instance().loadFont("res/Fonts/Kenney Rocket.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Kenney Pixel.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/kenvector_future_thin.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Kenney Mini.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Kenney Pixel Square.ttf");
      return FontCache::Instance().loadFont("res/Fonts/kenvector_future.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Kenney High Square.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Kenney Future Narrow.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Kenney High.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Kenney Mini Square.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Kenney Future.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Kenney Rocket Square.ttf");

 // Has no glyphs for regular chars
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/noto/NotoColorEmoji.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf");

// wholly broken
//      return FontCache::Instance().loadFont("res/Fonts/Hack-Italic.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Hack-Bold.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Hack-BoldItalic.ttf");
//      return FontCache::Instance().loadFont("res/Fonts/Hack-Regular.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/noto/NotoSansMono-Regular.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/freefont/FreeMono.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/ttf-khmeros-core/KhmerOS.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/ttf-khmeros-core/KhmerOSsys.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/liberation2/LiberationMono-Regular.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/liberation2/LiberationSerif-Regular.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/liberation/LiberationSerif-Regular.ttf");
//      return FontCache::Instance().loadFont("/usr/share/fonts/truetype/liberation/LiberationSansNarrow-Regular.ttf");

  }
}


