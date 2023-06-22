#include "ttf/ttf.h"
#include <fstream>
#include "ttf_vm.h"

static constexpr uint32_t TAG(const char (&c)[5]) {
  return (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | (c[3]);
}

std::pair<std::string, std::string> parseStringEntry(const char* p, int len, uint16_t platform, uint16_t encoding, uint16_t language) {
  switch(platform) {
    case 0:
      break;
    case 1:
      // Legacy Macintosh
      if (encoding == 0 && language == 0) {
        std::string rv;
        rv.resize(len);
        memcpy(rv.data(), p, rv.size());
        return { "en", rv };
      }
      break;
    case 3:
      // Legacy Microsoft
      // All string data for platform 3 must be encoded in UTF-16BE.
      if (encoding == 1 && language == 0x0409) {
        std::string res;
        res.resize(len);
        for (int i = 0; i < len; i++)
            res[i] = p[i * 2 + 1];
        return { "en", res };
      }
      break;
  }
  return {"",""};
}

Glyph::Glyph(Bini::reader r, int16_t numberOfContours) {
  if (numberOfContours < 0) {
    loadCompound(r, -numberOfContours);
  } else if (numberOfContours > 0) {
    loadSimple(r, numberOfContours);
  }
}

void Glyph::loadSimple(Bini::reader r, uint16_t numberOfContours) {
  std::vector<uint16_t> endOfContours;
  for (size_t n = 0; n < numberOfContours; n++) {
    endOfContours.push_back(r.read16be());
  }
  uint16_t instrLen = r.read16be();
  r.skip(instrLen);

  std::vector<uint8_t> flags;
  uint16_t pointCount = endOfContours.back() + 1;
  while (flags.size() < pointCount) {
    uint8_t flag = r.read8();
    if (flag & 0x08) {
      uint8_t repeats = r.read8();
      flag &= 0xF7;
      for (size_t n = 0; n <= repeats; n++) {
        flags.push_back(flag);
      }
    } else {
      flags.push_back(flag);
    }
  }
  std::vector<float> xs;
  float lastX = 0.0f;
  for (size_t n = 0; n < pointCount; n++) {
    bool xShort = flags[n] & 0x2;
    bool xSame = flags[n] & 0x10;
    if (xShort) {
      lastX += (xSame ? -r.read8() : r.read8());
    } else if (not xSame) {
      lastX += (int16_t)r.read16be();
    }
    xs.push_back(lastX);
  }
  std::vector<float> ys;
  float lastY = 0.0f;
  for (size_t n = 0; n < pointCount; n++) {
    bool yShort = flags[n] & 0x4;
    bool ySame = flags[n] & 0x20;
    if (yShort) {
      lastY += (ySame ? -r.read8() : r.read8());
    } else if (not ySame) {
      lastY += (int16_t)r.read16be();
    }
    ys.push_back(lastY);
  }

  contourPoints.emplace_back();
  auto it = endOfContours.begin();
  for (size_t n = 0; n < pointCount; n++) {
    contourPoints.back().emplace_back(xs[n], ys[n], (flags[n] & 0x1) == 0x1);
    if (n == *it) {
      if (not contourPoints.back().back().OnCurve) {
        vec2 mid = contourPoints.back().back() * 0.5 + contourPoints.back().front() * 0.5;
        contourPoints.back().emplace_back(mid.x, mid.y, true);
      }
      ++it;
      contourPoints.emplace_back();
    }
  }
  if (contourPoints.back().empty()) contourPoints.pop_back();
}

void Glyph::loadCompound(Bini::reader r, uint16_t numberOfContours) {
  (void)numberOfContours; // Where would I use this? I don't get it
  bool moreComponents = true;
  do {
    CompoundElement element;
    element.flags = r.read16be();
    moreComponents = (element.flags & MoreComponents);
    element.glyphIndex = r.read16be();
    if ((element.flags & ArgsAreXyValues) == 0) {
      throw std::runtime_error("Not implemented: " __FILE__ ": " + std::to_string(__LINE__));
    }

    if (element.flags & Arg1And2AreWords) {
      element.transform.e = (int16_t)r.read16be();
      element.transform.f = (int16_t)r.read16be();
    } else {
      element.transform.e = (int8_t)r.read8();
      element.transform.f = (int8_t)r.read8();
    }

    switch(element.flags & TransformationType) {
    case 0:
      break;
    case WeHaveAScale:
      element.transform.a = element.transform.d = (int16_t)r.read16be() / 16384.0f;
      break;
    case WeHaveAnXAndYScale:
      element.transform.a = (int16_t)r.read16be() / 16384.0f;
      element.transform.d = (int16_t)r.read16be() / 16384.0f;
      break;
    case WeHaveATwoByTwo:
      element.transform.a = (int16_t)r.read16be() / 16384.0f;
      element.transform.b = (int16_t)r.read16be() / 16384.0f;
      element.transform.c = (int16_t)r.read16be() / 16384.0f;
      element.transform.d = (int16_t)r.read16be() / 16384.0f;
      break;
    default:
      throw std::runtime_error("Not implemented: " __FILE__ ": " + std::to_string(__LINE__));
    }
    element.transform.CalculateMN();
    elements.push_back(element);
  } while (moreComponents);
}

void TtfFont::parse_glyf(std::vector<uint32_t> loca, std::span<const uint8_t> glyf) {
  for (size_t n = 0; n < loca.size(); n++) {
    if (n != loca.size() - 1 &&
        (loca[n] == loca[n+1] ||
         loca[n] + 10 >= glyf.size())) {
      glyphs.emplace_back(Bini::reader{}, 0);
    } else {
      Bini::reader glyph(glyf.subspan(loca[n], n == loca.size() - 1 ? std::dynamic_extent : loca[n+1] - loca[n]));
      int16_t numberOfContours = (int16_t)glyph.read16be();
      glyph.skip(8);
      /*
      uint16_t xMin = glyph.read16be();
      uint16_t xMax = glyph.read16be();
      uint16_t yMin = glyph.read16be();
      uint16_t yMax = glyph.read16be();
      */
      glyphs.emplace_back(std::move(glyph), numberOfContours);
    }
  }
}

void TtfFont::parse_colr(std::span<const uint8_t> colr, std::span<const uint8_t> cpal_d) {
  /*
  Bini::reader cpal(cpal_d);
  uint16_t version = cpal.read16be();
  uint16_t numPaletteEntries = cpal.read16be();
  uint16_t numPalettes = cpal.read16be();
  uint16_t numColorRecords = cpal.read16be();
  uint32_t colorRecordsArrayOffset = cpal.read32be();
  std::vector<uint16_t> indices;
  for (size_t n = 0; n < numPalettes; n++) {
    indices.push_back(cpal.read16be());
  }
  uint32_t paletteTypesArrayOffset = 0;
  uint32_t paletteLabelsArrayOffset = 0;
  uint32_t paletteEntryLabelsArrayOffset = 0;
  if (version > 0) {
    paletteTypesArrayOffset = cpal.read32be();
    paletteLabelsArrayOffset = cpal.read32be();
    paletteEntryLabelsArrayOffset = cpal.read32be();
  }
  */
  (void)colr;
  (void)cpal_d;
}

Glyph& TtfFont::get_glyph(char32_t c) {
  uint32_t g = cmap.char2glyph(c);
  if (g >= glyphs.size()) 
    g = 0;
  return glyphs[g];
}

CmapTable::CmapTable() 
{}

CmapTable::CmapTable(uint16_t platformId, uint16_t platformSpecificId, std::span<const uint8_t> cmapdata) 
: platformId(platformId)
, platformSpecificId(platformSpecificId)
{
  Bini::reader r(cmapdata);
  format = r.read16be();
  uint32_t length;
  if (format < 8) {
    length = r.read16be();
    r.skip(2);
    cmap = r.get(length - 6);
  } else {
    r.skip(2);
    length = r.read32be();
    if (format == 14) {
      cmap = r.get(length - 8);
    } else {
      r.skip(4);
      cmap = r.get(length - 12);
    }
  }
}

bool CmapTable::isUnicodeVariation() {
  return format == 14;
}

size_t CmapTable::score() {
  int formatScore = 0; // 0 unusable, 1 usable but limited, 2 good
  int platformScore = 0; // 0 unusable, 1 limited usability, 3 Unicode BMP, 5 Unicode
  switch(format) {
  default:
    formatScore = 0; // Unknown format
    break;
  case 0:
  case 2:
  case 6:
  case 8:
  case 10:
    formatScore = 0; // obsolete and unimplemented
    break;
  case 14:
    formatScore = 0; // Not usable as generic table
    break;
  case 4:
    formatScore = 1; // Basic table
    break;
  case 12:
  case 13:
    formatScore = 2; // Good table
    break;
  }
  switch(platformId) {
  case 0:
    switch(platformSpecificId) {
    case 0:
    case 1:
    case 2:
    case 3:
      platformScore = 3;
      break;
    case 4:
    case 6:
      platformScore = 5;
      break;
    case 5: // variation sequences
    default:
      platformScore = 0;
      break;
    }
    break;
  case 1: // Mac; all deprecated & unusable
    platformScore = 0;
    break;
  case 3:
    switch(platformSpecificId) {
    case 0:
      platformScore = 1;
      break;
    case 1: // unicode bmp
      platformScore = 3;
      break;
    case 10: // unicode
      platformScore = 5;
      break;
    default:
      platformScore = 0;
      break;
    }
    break;
  default:
    platformScore = 0;
  }
  return formatScore + platformScore;
}

uint32_t CmapTable::char2glyph(char32_t c) {
  Bini::reader r(cmap);

  switch(format) {
  case 4:
  {
    uint16_t doubleSegcount = r.read16be();
    r.skip(6);
    Bini::reader endCode = r.get(doubleSegcount);
    r.skip(2);
    Bini::reader startCode = r.get(doubleSegcount);
    Bini::reader idDelta = r.get(doubleSegcount);
    Bini::reader idRangeOffset = r.get(doubleSegcount);
    for (size_t n = 0; n < doubleSegcount/2; n++) {
      char32_t start = startCode.read16be();
      char32_t end = endCode.read16be();
      int delta = (int16_t)idDelta.read16be();
      int rangeOffset = idRangeOffset.read16be();
      if (start <= c && c <= end) { // yes, this has to read <= per ttf spec
        if (rangeOffset == 0) {
          return c + delta;
        } else {
          r.skip((rangeOffset - doubleSegcount + n * 2) + (c - start) * 2);
          uint16_t value = r.read16be();
          if (value == 0) return 0;
          return (uint16_t)(value + delta);
        }
      }
    }
  }
    break;
  case 12:
  {
    uint32_t nGroups = r.read32be();
    for (size_t n = 0; n < nGroups; n++) {
      uint32_t startCharCode = r.read32be();
      uint32_t endCharCode = r.read32be();
      uint32_t startGlyphCode = r.read32be();
      if (startCharCode <= c && c < endCharCode) 
        return startGlyphCode + (c - startCharCode);
    }
  }
    break;
  case 13:
  {
    uint32_t nGroups = r.read32be();
    for (size_t n = 0; n < nGroups; n++) {
      uint32_t startCharCode = r.read32be();
      uint32_t endCharCode = r.read32be();
      uint32_t startGlyphCode = r.read32be();
      if (startCharCode <= c && c < endCharCode) 
        return startGlyphCode;
    }
  }
    break;
  default:
    throw std::runtime_error("Unsupported cmap format");
  }
  return 0;
}

TtfFont::TtfFont(std::filesystem::path path)
{
  ttffile.resize(std::filesystem::file_size(path));
  std::ifstream(path).read((char*)ttffile.data(), ttffile.size());
  load();
}

TtfFont::TtfFont(std::vector<uint8_t> data)
: ttffile(data)
{
  load();
}

TtfFont::~TtfFont() = default;

void TtfFont::parse_horizontal_metrics(Bini::reader hhea, Bini::reader hmtx) {
  if (hhea.read32be() != 0x00010000) throw std::runtime_error("Invalid hhea version");
  ascender = (int16_t)hhea.read16be();
  descender = (int16_t)hhea.read16be();
  lineGap = (int16_t)hhea.read16be();
  advanceWidthMax = (int16_t)hhea.read16be();
  minLeftSideBearing = (int16_t)hhea.read16be();
  minRightSideBearing = (int16_t)hhea.read16be();
  xMaxExtent = (int16_t)hhea.read16be();
  uint16_t caretSlopeRise = hhea.read16be();
  uint16_t caretSlopeRun = hhea.read16be();
  caretSlope = atan2(caretSlopeRun, caretSlopeRise);

  hhea.skip(12);
  uint16_t longHorMetricCount = hhea.read16be();
  uint16_t lastAdvanceWidth = 0;
  for (auto& glyph : glyphs) {
    if (longHorMetricCount && longHorMetricCount--) {
      lastAdvanceWidth = hmtx.read16be();
    }
    glyph.advanceWidth = lastAdvanceWidth;
    glyph.leftSideBearing = (int16_t)hmtx.read16be();
  }
}

void TtfFont::load() {
  Bini::reader r(ttffile);
  uint32_t tag = r.read32be();
  (void)tag;
  uint16_t numTables = r.read16be();
  r.skip(6); // skip over fields duplicating information
  // We want to handle tables in a set order, so we first read them and then use them
  std::map<uint32_t, std::span<const uint8_t>> tables;
  for (size_t n = 0; n < numTables; n++) {
    uint32_t tag = r.read32be();
    r.skip(4); // checksum
    uint32_t offset = r.read32be();
    uint32_t length = r.read32be();
    tables[tag] = std::span<const uint8_t>(ttffile).subspan(offset, length);
  }
  
  {
    Bini::reader r(tables[TAG("cmap")]);
    uint16_t version = r.read16be();
    (void)version;
    uint16_t subtablecount = r.read16be();
    for (size_t n = 0; n < subtablecount; n++) {
      uint16_t platformId = r.read16be();
      uint16_t platformSpecificId = r.read16be();
      uint32_t offset = r.read32be();
      CmapTable newTable(platformId, platformSpecificId, tables[TAG("cmap")].subspan(offset));
      if (newTable.score() > cmap.score())
        cmap = std::move(newTable);
    }
  }

  Bini::reader maxp(tables[TAG("maxp")]);
  if (maxp.read32be() != 0x00010000) throw std::runtime_error("Invalid maxp version");
  uint16_t glyphCount = maxp.read16be();
  glyphs.reserve(glyphCount);

  //parse_name(tables[TAG("name")]);

  Bini::reader head(tables[TAG("head")]);
  head.skip(18);
  uint16_t unitsPerEm = head.read16be();
  head.skip(30);
  uint16_t locaFormat = head.read16be();
  if (locaFormat > 1)
    throw std::runtime_error("invalid loca format");

  uint16_t glyfFormat = head.read16be();
  if (glyfFormat != 0)
    throw std::runtime_error("invalid glyf format");

  Bini::reader loca(tables[TAG("loca")]);
  std::vector<uint32_t> loca_out;
  for (size_t n = 0; n < glyphCount; n++) {
    loca_out.push_back((locaFormat == 0) ? loca.read16be() * 2 : loca.read32be());
  }
  
  parse_glyf(loca_out, tables[TAG("glyf")]);
  parse_horizontal_metrics(tables[TAG("hhea")], tables[TAG("hmtx")]);

  parse_colr(tables[TAG("colr")], tables[TAG("cmap")]);
  vm = std::make_unique<Vm>(unitsPerEm, tables[TAG("maxp")], tables[TAG("prep")], tables[TAG("fpgm")], tables[TAG("cvt ")]);
}


