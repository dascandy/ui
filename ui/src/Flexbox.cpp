#include <ui/Flexbox.hpp>
#include <span>
#include <numeric>
#include <string>
#include <ui/Theme.hpp>
#include <SDL2/SDL.h>

Flexbox::Flexbox()
{
}

bool Flexbox::stretch() {
  return false;
}

void Flexbox::Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>& out) {
//  Theme::Instance().CreateWhiteBox(out, parentZ, **x, **y, **w, **h);
  
  /*
  float x1 = **x + parentX - **w / 2;
  float y1 = **y + parentY - **h / 2;
  float x2 = x1 + **w;
  float y2 = y1 + **h;
  float s1 = 0;
  float t1 = 0;
  float s2 = 1;
  float t2 = 1;
  std::vector<vertex>& v = out[texture.texture.get()];
  v.push_back({x1, y1, parentZ, 0, 0, 1, s1, t1 });
  v.push_back({x1, y2, parentZ, 0, 0, 1, s1, t2 });
  v.push_back({x2, y1, parentZ, 0, 0, 1, s2, t1 });
  v.push_back({x2, y1, parentZ, 0, 0, 1, s2, t1 });
  v.push_back({x1, y2, parentZ, 0, 0, 1, s1, t2 });
  v.push_back({x2, y2, parentZ, 0, 0, 1, s2, t2 });
  */
  for (auto& widget : widgets) {
    widget->Render(parentX + **x, parentY + **y, parentZ - 0.0001, out);
  }
}

void Flexbox::set(Direction newDirection) {
  if (direction != newDirection) {
    direction = newDirection;
    if (allowRelayout) Relayout();
  }
}

void Flexbox::set(Wrap newWrap) {
  if (wrap != newWrap) {
    wrap = newWrap;
    if (allowRelayout) Relayout();
  }
}

void Flexbox::set(Justify newJustify) {
  if (justify != newJustify) {
    justify = newJustify;
    if (allowRelayout) Relayout();
  }
}

void Flexbox::setVerticalJustify(Justify newJustify) {
  if (vjustify != newJustify) {
    vjustify = newJustify;
    if (allowRelayout) Relayout();
  }
}

void Flexbox::set(Align newAlign) {
  if (align != newAlign) {
    align = newAlign;
    if (allowRelayout) Relayout();
  }
}

void Flexbox::set(Margin newMargin) {
  if (margin != newMargin.value) {
    margin = newMargin.value;
    if (allowRelayout) Relayout();
  }
}

static std::pair<float, float> getJustify(Justify justify, float margin, float outmargin, float spaceForMargin, size_t widgetCount) {
  if (spaceForMargin < 0) return { margin, margin };

  switch(justify) {
    case Justify::Start:
    default:
      return { outmargin, margin };
    case Justify::Center:
      return { outmargin + (spaceForMargin - (widgetCount - 1) * margin) / 2, margin };
    case Justify::End:
      return { outmargin + spaceForMargin - (widgetCount - 1) * margin, margin };
    case Justify::SpaceBetween:
      return { outmargin, (spaceForMargin) / (widgetCount - 1) };
    case Justify::SpaceAround:
      return { outmargin + spaceForMargin / (widgetCount) / 2, spaceForMargin / (widgetCount) };
    case Justify::SpaceEvenly:
      return { outmargin + spaceForMargin / (widgetCount + 1), spaceForMargin / (widgetCount + 1) };
  }
}

static float getAlign(Align align, float boxWidth, float itemWidth) {
  switch(align) {
    case Align::Start:
    case Align::Stretch:
    default:
      return 0;
    case Align::End:
      return boxWidth - itemWidth;
    case Align::Center:
      return (boxWidth - itemWidth) / 2.0;
  }
}

int Flexbox::stretchcount() {
  size_t count = 0;
  for (auto& e : widgets) {
    if (e->stretch()) {
      count++;
    }
  }
  return count;
}

void Flexbox::Relayout() {
  if (widgets.empty()) 
    return;

  // 1. Find the biggest and total size in each direction (ignoring the main direction of stretch elements)
  float maxWidth = 0, maxHeight = 0, totalWidth = 0, totalHeight = 0;
  for (auto& e : widgets) {
    if (e->stretch()) {
      if (direction == Direction::Row ||
        direction == Direction::RowReverse) {
        if (**e->h > maxHeight) maxHeight = **e->h;
      } else {
        if (**e->w > maxWidth) maxWidth = **e->w;
      }
    } else {
      totalWidth += **e->w;
      totalHeight += **e->h;
      if (**e->w > maxWidth) maxWidth = **e->w;
      if (**e->h > maxHeight) maxHeight = **e->h;
    }
  }
  totalWidth += (widgets.size() - 1) * margin;
  totalHeight += (widgets.size() - 1) * margin;

  // 2. When align is set to stretch, stretch each element in that direction to the width of the container
  if (align == Align::Stretch) {
    for (auto& e : widgets) {
      if (direction == Direction::Row || 
          direction == Direction::RowReverse) {
        e->h = h;
      } else {
        e->w = w;
      }
    }
  } else {
    if (direction == Direction::Row || 
        direction == Direction::RowReverse) {
      h = val(maxHeight);
    } else {
      w = val(maxWidth);
    }
  }

  // 3. If we have a stretch element, then stretch up 
  if (stretchcount()) {
    if (direction == Direction::Row || 
        direction == Direction::RowReverse) {
      float newWidth = (**w - totalWidth) / stretchcount();
      for (auto& e : widgets) {
        if (e->stretch()) {
          e->w = val(newWidth);
        }
      }
    } else {
      float newHeight = (**h - totalHeight) / stretchcount();
      for (auto& e : widgets) {
        if (e->stretch()) {
          e->h = val(newHeight);
        }
      }
    }
  }

  for (auto& w : widgets) w->Relayout();

  bool horizontal = (direction == Direction::Row || direction == Direction::RowReverse);
  bool reverse = (direction == Direction::RowReverse || direction == Direction::ColumnReverse);
  float available = (horizontal ? **w: **h);
  std::vector<Widget*> ws;
  for (auto& w : widgets) ws.push_back(w.get());
  std::vector<std::span<Widget*>> rows;
  float curSize = outmargin;
  
  size_t start = 0, index = 0;
  float totalRowHeight = 2 * outmargin;
  float rowMaxHeight = 0;
  while (index < ws.size()) {
    if (wrap != Wrap::Line && curSize > outmargin && curSize + 2 * outmargin + (horizontal ? **(ws[index]->w) : **(ws[index]->h)) > available) {
      rows.push_back(std::span<Widget*>(ws.data() + start, ws.data() + index));
      totalRowHeight += rowMaxHeight;
      start = index;
      curSize = outmargin;
      rowMaxHeight = 0;
    }
    curSize += margin + (horizontal ? **(ws[index]->w) : **(ws[index]->h));
    rowMaxHeight = std::max<float>(rowMaxHeight, (horizontal ? **(ws[index]->h) : **(ws[index]->w)));
    index++;
  }
  totalRowHeight += rowMaxHeight;
  rows.push_back(std::span<Widget*>(ws.data() + start, ws.data() + index));
  if (wrap == Wrap::WrapReverse) {
    std::reverse(rows.begin(), rows.end());
  }

  auto [topBefore, topBetween] = getJustify(vjustify, margin, outmargin, (float)(horizontal ? **h : **w) - totalRowHeight, rows.size());
  float top = topBefore - (float)(horizontal ? **h : **w) / 2;
  for (auto row : rows) {
    float totalMajor = std::accumulate(row.begin(), row.end(), 2 * outmargin, [&](float current, Widget* widget){ return current + (horizontal ? **widget->w : **widget->h); });
    float maxHeight = std::accumulate(row.begin(), row.end(), 0.0, [&](float current, Widget* widget) { return std::max<float>(current, horizontal ? **widget->h : **widget->w); });
    auto [spaceBefore, spaceBetween] = getJustify(justify, margin, outmargin, (float)(horizontal ? **w : **h) - totalMajor, row.size());
    float start = (reverse ? (horizontal ? **w : **h) - spaceBefore : spaceBefore) - (horizontal ? **w : **h) / 2;
    for (auto& widget : row) {
      if (reverse) start -= (horizontal ? **widget->w : **widget->h);
      float topOf = getAlign(align, maxHeight, (horizontal ? **widget->h : **widget->w));
      Val<float> main = val(start + (horizontal ? **widget->w : **widget->h) / 2);
      Val<float> off = val(top + topOf + (horizontal ? **widget->h : **widget->w) / 2);
      widget->x = horizontal ? std::move(main) : std::move(off);
      widget->y = horizontal ? std::move(off) : std::move(main);
      if (reverse)
        start -= spaceBetween;
      else
        start += (horizontal ? **widget->w : **widget->h) + spaceBetween;
    }
    top += maxHeight + topBetween;
  }
}

void Flexbox::addWidget(std::unique_ptr<Widget> widget, int position) {
  if (position >= 0) 
    widgets.insert(widgets.begin() + position, std::move(widget));
  else 
    widgets.push_back(std::move(widget));
  if (allowRelayout) Relayout();
}

void Flexbox::removeWidget(size_t offset) {
  widgets.erase(widgets.begin() + offset);
  if (allowRelayout) Relayout();
}

std::string to_string(Direction direction) {
  switch(direction) {
    case Direction::Column: return "Column";
    case Direction::ColumnReverse: return "ColumnReverse";
    case Direction::Row: return "Row";
    case Direction::RowReverse: return "RowReverse";
  }
}

std::string to_string(Wrap wrap) {
  switch(wrap) {
    case Wrap::Wrap: return "Wrap";
    case Wrap::Line: return "Line";
    case Wrap::WrapReverse: return "WrapReverse";
  }
}

std::string to_string(Justify justify) {
  switch (justify) {
    case Justify::Center: return "Center";
    case Justify::Start: return "Start";
    case Justify::End: return "End";
    case Justify::SpaceAround: return "SpaceAround";
    case Justify::SpaceBetween: return "SpaceBetween";
    case Justify::SpaceEvenly: return "SpaceEvenly";
  }
}

std::string to_string(Align align) {
  switch(align) {
    case Align::Center: return "Center";
    case Align::Start: return "Start";
    case Align::End: return "End";
    case Align::Stretch: return "Stretch";
  }
}

bool Flexbox::HandleEvent(const SDL_Event& event) {
  if (event.type == SDL_KEYDOWN ||
      event.type == SDL_KEYUP) {
    if (currentFocus) 
      return currentFocus->HandleEvent(event);
    return false;
  } else {
    for (auto& widget : widgets) {
      if (x) {
        bool wasHandled = widget->HandleEvent(event);
        currentFocus = widget.get();
        return wasHandled;
      }
    }
  }
  return false;
}

/*
// Widgets:
- Appear -> grow from 0% size to 120% size, shrink back to 100%
- Disappear -> inverse of appear
- Disappear -> (in flexbox) shrink in direction of flexbox resize
- Appear -> (in flexbox) grow in direction of flexbox resize
- Slide over -> -0.5 cosine movement to target position
*/


