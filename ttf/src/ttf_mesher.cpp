#include "ttf/ttf.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <vector>
#include "earcut/earcut.hpp"

Outline::Contour::Contour() {
}

void Outline::Contour::DrawStraightLine(vec2 end) {
  // Maybe split this line
  points.push_back(end);
}

void Outline::Contour::AvoidDuplicates() {
  for (auto it = points.begin(); it != points.end(); ) {
    auto next = it;
    next++;
    if (next == points.end()) next = points.begin();
    if (*it == *next) {
      it = points.erase(it);
    } else {
      ++it;
    }
  }
}

void Outline::Contour::DrawBezier(vec2 mid, vec2 end) {
  vec2 start = points.back();
  size_t splits = 4;
  for (size_t n = 1; n <= splits; n++) {
    float t = (float)n / splits;
    float omt = 1.0f - t;
    
    points.push_back((omt * omt) * start + (2 * t * omt) * mid + (t * t) * end);
  }
}

void Outline::MergeOutline(Outline outline, Transform2D transform) {
  for (auto& c : outline.contours) {
    float m = std::max(transform.a, transform.b), n = std::max(transform.c, transform.d);

    // No idea why, but the spec says this.
    if (std::abs(transform.a - transform.c) < 33/65536.0) m *= 2;
    if (std::abs(transform.b - transform.d) < 33/65536.0) n *= 2;

    for (auto& v : c.points) {
      v = transform.transform(v);
    }
    for (auto& contour : c.holes) {
      for (auto& v : contour.points) {
        v = transform.transform(v);
      }
    }

    contours.push_back(std::move(c));
  }
}

void Outline::MatchHoles() {
  // Can't have a hole in a single thing
  if (contours.size() <= 1) return;

  std::vector<Contour> holes;
  for (size_t n = 0; n < contours.size();) {
    if (contours[n].IsHole()) {
      holes.push_back(std::move(contours[n]));
      if (n != contours.size() - 1) {
        contours[n] = std::move(contours.back());
      }
      contours.pop_back();
    } else {
      n++;
    }
  }
  for (auto& h : holes) {
    for (auto& c : contours) {
      if (c.Contains(h)) {
        c.holes.push_back(std::move(h));
        break;
      }
    }
    // we ignore holes that are not inside something
  }
}

bool isBetter(vec2 a, vec2 b) {
  if (a.x < b.x) return true;
  if (a.x == b.x && a.y < b.y) return true;
  return false;
}

bool Outline::Contour::IsHole() {
  if (points.empty()) return false;
  size_t bestPoint = 0;
  for (size_t n = 0; n < points.size(); n++) {
    if (isBetter(points[n], points[bestPoint])) {
      bestPoint = n;
    }
  }
  vec2 a = points[(bestPoint + points.size() - 1) % points.size()];
  vec2 b = points[(bestPoint + points.size()) % points.size()];
  vec2 c = points[(bestPoint + points.size() + 1) % points.size()];
  float r = (b - a).cross(c - b);
  printf("%f/%f %f/%f %f/%f %f\n", a.x, a.y, b.x, b.y, c.x, c.y, r);
  return r > 0;
}

bool Outline::Contour::Contains(const Contour&) {
  return true;
}

namespace mapbox {
namespace util {

template <>
struct nth<0, vec2> {
    inline static auto get(const vec2 &t) {
        return t.x;
    };
};
template <>
struct nth<1, vec2> {
    inline static auto get(const vec2 &t) {
        return t.y;
    };
};

} // namespace util
} // namespace mapbox

Mesh Outline::ToMesh() {
  Mesh m;
  for (auto& contour : contours) {
    std::vector<std::vector<vec2>> points;
    points.push_back(contour.points);
    for (auto& hole : contour.holes) {
      points.push_back(hole.points);
    }
    std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(points);

    for (size_t n = 0; n < indices.size(); n += 3) {
      m.faces.emplace_back(m.vertices.size() + indices[n], m.vertices.size() + indices[n+1], m.vertices.size() + indices[n+2]);
    }
    for (auto& vec : points) {
      for (auto& vert : vec) {
        m.vertices.push_back(vert);
      }
    }
  }
  return m;
}

Outline Glyph::ToOutline(TtfFont& font) {
  Outline outline;
  if (elements.empty()) {
    for (auto& v : contourPoints) {
      outline.contours.emplace_back();
      Outline::Contour& contour = outline.contours.back();
      contour.points.push_back(v.back());
      bool lastOnCurve = true;
      for (size_t n = 0; n < v.size(); n++) {
        if (lastOnCurve && (v[n].OnCurve)) {
          contour.DrawStraightLine(v[n]);
          lastOnCurve = true;
        } else {
          vec2 end;
          if (v[n+1].OnCurve) {
            end = v[n+1];
            lastOnCurve = true;
            n++;
          } else {
            end = v[n] * 0.5 + v[n+1] * 0.5;
            lastOnCurve = false;
          }
          contour.DrawBezier(v[n], end);
        }
      }
      contour.AvoidDuplicates();
    }
    outline.MatchHoles();
  } else {
    for (auto& element : elements) {
      Outline o = font.glyphs[element.glyphIndex].ToOutline(font);
      outline.MergeOutline(o, element.transform);
    }
  }
  return outline;
}


