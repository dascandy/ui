#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <span>
#include <filesystem>
#include <bini/reader.h>

struct vec2 {
  vec2() = default;
  vec2(float x, float y)
  : x(x)
  , y(y)
  {}
  auto operator<=>(const vec2&) const = default;
  float len2() const {
    return x*x + y*y;
  }
  float len() const {
    return sqrt(len2());
  }
  float dot(const vec2& v) const {
    return x * v.x + y * v.y;
  }
  float cross(const vec2& v) const {
    return x * v.y - y * v.x;
  }
  vec2 operator-(const vec2& v) const {
    return { x - v.x, y - v.y };
  }
  vec2 operator+(const vec2& v) const {
    return { x + v.x, y + v.y };
  }
  friend vec2 operator*(float s, const vec2& v) {
    return {v.x * s, v.y * s};
  }
  vec2 operator*(float s) const {
    return {x * s, y * s};
  }
  vec2 project(const vec2& e1, const vec2& e2) const {
    return { (*this).dot(e1), (*this).dot(e2) };
  }
  float x = 0.0f, y = 0.0f;
  const float& operator[](size_t index) const {
    return (index == 0) ? x : y;
  }
  float& operator[](size_t index) {
    return (index == 0) ? x : y;
  }
  vec2& operator+=(const vec2& v) {
    x += v.x;
    y += v.y;
    return *this;
  }
};

struct ttf_point : vec2
{
  ttf_point(float x, float y, bool OnCurve)
  : vec2(x, y)
  , OnCurve(OnCurve)
  {}
  ttf_point()
  {}
  bool OnCurve;
};

struct Mesh
{
  struct face { 
    face(int v1, int v2, int v3) 
    : v1(v1)
    , v2(v2)
    , v3(v3)
    {}
    int v1; 
    int v2; 
    int v3; 
  };
  std::vector<vec2> vertices;
  std::vector<face> faces;
};

struct Transform2D {
  vec2 transform(vec2 in) {
    return { a * in.x + c * in.y + e * m, b * in.x + d * in.y + f * n};
  }
  void CalculateMN() {
    m = std::max(a, b);
    n = std::max(c, d);

    // No idea why, but the spec says this.
    if (std::abs(a - c) < 33/65536.0) m *= 2;
    if (std::abs(b - d) < 33/65536.0) n *= 2;
  }
  float a = 1.0f, b = 0.0f, c = 0.0f, d = 1.0f, e = 0.0f, f = 0.0f;
  float m, n;
};

struct Outline {
  struct Contour {
    Contour();
    std::vector<vec2> points;
    std::vector<Contour> holes;
    void DrawStraightLine(vec2 end);
    void DrawBezier(vec2 mid, vec2 end);
    bool IsHole();
    bool Contains(const Contour&);
    void AvoidDuplicates();
    
  };
  std::vector<Contour> contours;
  void MergeOutline(Outline outline, Transform2D transform);
  void MatchHoles();
  Mesh ToMesh();
};

class TtfFont;

class Glyph
{
public:
  Glyph(Bini::reader r, int16_t numberOfContours);

  Outline ToOutline(TtfFont& font);
private:
  void loadSimple(Bini::reader r, uint16_t numberOfContours);
  void loadCompound(Bini::reader r, uint16_t numberOfContours);
public:

  float advanceWidth;
  float leftSideBearing;

private:
  enum {
    Arg1And2AreWords = 1,
    ArgsAreXyValues = 2,
    RoundXyToGrid = 4,
    WeHaveAScale = 8,
    MoreComponents = 32,
    WeHaveAnXAndYScale = 64,
    WeHaveATwoByTwo = 128,
    WeHaveInstructions = 256,
    UseMyMetrics = 512,
    OverlapCompound = 1024,
    ScaledComponentOffset = 2048,
    UnscaledComponentOffset = 4096,
    TransformationType = WeHaveAScale | WeHaveAnXAndYScale | WeHaveATwoByTwo
  };

  struct CompoundElement {
    Transform2D transform;
    uint16_t flags;
    uint16_t glyphIndex;
  };

  std::vector<CompoundElement> elements;
  std::vector<std::vector<ttf_point>> contourPoints;
};

struct CmapTable {
public:
  CmapTable();
  CmapTable(uint16_t platformId, uint16_t platformSpecificId, std::span<const uint8_t> cmapdata);
  uint32_t char2glyph(char32_t c);
  bool isUnicodeVariation();
  size_t score();
private:
  uint16_t platformId = 0;
  uint16_t platformSpecificId = 0;
  uint16_t format = 0;
  std::span<const uint8_t> cmap;
};

struct Vm;

class TtfFont
{
public:
  TtfFont(std::vector<uint8_t> data);
  TtfFont(std::filesystem::path path);
  ~TtfFont();
  Glyph& get_glyph(char32_t c);
private:
  friend class Glyph;
  void parse_glyf(std::vector<uint32_t> loca, std::span<const uint8_t> glyf);
  void parse_colr(std::span<const uint8_t> colr, std::span<const uint8_t> cmap);

  void loadSimple(Bini::reader r, uint16_t numberOfContours);
  void loadCompound(Bini::reader r, uint16_t numberOfContours);
  void parse_horizontal_metrics(Bini::reader hhea, Bini::reader hmtx);

  void load();
  std::vector<uint8_t> ttffile;
  std::vector<Glyph> glyphs;
  CmapTable cmap;
  std::unique_ptr<Vm> vm;
//  CmapTable unicodeVariations;

  std::map<uint16_t, std::map<std::string, std::string>> string_values;

public:
  float ascender;           /* Typographic ascent (Distance from baseline of highest ascender) */
  float descender;          /* Typographic descent (Distance from baseline of lowest descender) */
  float lineGap;            /* Typographic line gap (Distance from line1 descender to line2 ascender) */
  float advanceWidthMax;    /* Maximum advance width value */
  float minLeftSideBearing;    /* Minimum left sidebearing value */
  float minRightSideBearing;    /* Minimum right sidebearing value; calculated as Min(aw - lsb - (xMax - xMin)). */
  float xMaxExtent;         /* Max(lsb + (xMax - xMin)) */
  float caretSlope;         /* The slope of the cursor in radians, ~0 for horizontal not italic font and ~0.2 for italic font */

  enum {
      CopyrightNotice = 0,
      FontFamily = 1,
      FontSubfamily = 2,
      UniqueSubfamily = 3,
      FullFontName = 4,
      VersionInformation = 5,
      PostScriptFontName = 6,
      TrademarkNotice = 7,
      ManufacturerName = 8,
      Designer = 9,
      Description = 10,
      FontVendorURL = 11,
      FontDesignerURL = 12,
      LicenseDescription = 13,
      LicenseInformationURL = 14,
      NewFontFamily = 16,
      NewFontSubfamily = 17,
      SampleText = 19,
  };
};



