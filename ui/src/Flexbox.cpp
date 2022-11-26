#include <ui/Flexbox.hpp>
#include <span>
#include <numeric>
#include <string>

Flexbox::Flexbox(SubTexture st) 
: texture(st)
{

}

void Flexbox::Render(float parentX, float parentY, float parentZ, std::map<Texture*, std::vector<vertex>>& out) {
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
    widget->Render(parentX + **x, parentY + **y, parentZ + 0.0001, out);
  }
}

void Flexbox::setDirection(Direction newDirection) {
  if (direction != newDirection) {
    direction = newDirection;
    if (allowRelayout) Relayout();
  }
}

void Flexbox::setWrap(Wrap newWrap) {
  if (wrap != newWrap) {
    wrap = newWrap;
    if (allowRelayout) Relayout();
  }
}

void Flexbox::setJustify(Justify newJustify) {
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

void Flexbox::setAlign(Align newAlign) {
  if (align != newAlign) {
    align = newAlign;
    if (allowRelayout) Relayout();
  }
}

void Flexbox::setMargin(float newMargin) {
  if (margin != newMargin) {
    margin = newMargin;
    if (allowRelayout) Relayout();
  }
}

static std::pair<float, float> getJustify(Flexbox::Justify justify, size_t margin, float spaceForMargin, size_t widgetCount) {
  if (spaceForMargin < 0) return { margin, margin };

  switch(justify) {
    case Flexbox::Justify::Start:
    default:
      return { margin, margin };
    case Flexbox::Justify::Center:
      return { (spaceForMargin - (widgetCount-1) * margin) / 2, margin };
    case Flexbox::Justify::End:
      return { spaceForMargin - (widgetCount * margin), margin };
    case Flexbox::Justify::SpaceBetween:
      return { margin, (spaceForMargin - margin * 2) / (widgetCount - 1) };
    case Flexbox::Justify::SpaceAround:
      return { spaceForMargin / (widgetCount) / 2, spaceForMargin / (widgetCount) };
    case Flexbox::Justify::SpaceEvenly:
      return { spaceForMargin / (widgetCount + 1), spaceForMargin / (widgetCount + 1) };
  }
}

static float getAlign(Flexbox::Align align, float boxWidth, float itemWidth) {
  switch(align) {
    case Flexbox::Align::Start:
    case Flexbox::Align::Stretch:
    default:
      return 0;
    case Flexbox::Align::End:
      return boxWidth - itemWidth;
    case Flexbox::Align::Center:
      return (boxWidth - itemWidth) / 2.0;
  }
}

void Flexbox::Relayout() {
  if (widgets.empty()) return;
  bool horizontal = (direction == Flexbox::Direction::Row || direction == Flexbox::Direction::RowReverse);
  bool reverse = (direction == Flexbox::Direction::RowReverse || direction == Flexbox::Direction::ColumnReverse);
  float available = (horizontal ? **w: **h);
  std::span<std::unique_ptr<Widget>> ws = widgets;
  std::vector<std::span<std::unique_ptr<Widget>>> rows;
  float curSize = margin;
  
  size_t start = 0, index = 0;
  float totalRowHeight = margin;
  float rowMaxHeight = 0;
  while (index < ws.size()) {
    if (wrap != Wrap::Line && curSize > margin && curSize + margin + (horizontal ? **(ws[index]->w) : **(ws[index]->h)) > available) {
      rows.push_back(ws.subspan(start, index-start));
      totalRowHeight += margin + rowMaxHeight;
      start = index;
      curSize = margin;
      rowMaxHeight = 0;
    }
    curSize += margin + (horizontal ? **(ws[index]->w) : **(ws[index]->h));
    rowMaxHeight = std::max<float>(rowMaxHeight, (horizontal ? **(ws[index]->h) : **(ws[index]->w)));
    index++;
  }
  totalRowHeight += margin + rowMaxHeight;
  rows.push_back(ws.subspan(start));
  if (wrap == Wrap::WrapReverse) {
    std::reverse(rows.begin(), rows.end());
  }

  auto [topBefore, topBetween] = getJustify(vjustify, margin, (float)(horizontal ? **h : **w) - totalRowHeight, rows.size());
  float top = topBefore - (float)(horizontal ? **h : **w) / 2;
  for (auto row : rows) {
    float totalMajor = std::accumulate(row.begin(), row.end(), margin, [&](size_t current, std::unique_ptr<Widget>& widget){ return current + margin + (horizontal ? **widget->w : **widget->h); });
    float maxHeight = std::accumulate(row.begin(), row.end(), 0.0, [&](float current, std::unique_ptr<Widget>& widget) { return std::max<float>(current, horizontal ? **widget->h : **widget->w); });
    auto [spaceBefore, spaceBetween] = getJustify(justify, margin, (float)(horizontal ? **w : **h) - totalMajor, row.size());
    float start = (reverse ? (horizontal ? **w : **h) - spaceBefore : spaceBefore) - (horizontal ? **w : **h) / 2;
    for (auto& widget : row) {
      if (reverse) start -= (horizontal ? **widget->w : **widget->h);
      if (align == Align::Stretch) (horizontal ? widget->h : widget->w) = val(maxHeight);
      float topOf = getAlign(align, maxHeight, (horizontal ? **widget->h : **widget->w));
      widget->x = horizontal ? val(start + **widget->w / 2): val(top + topOf + **widget->w / 2);
      widget->y = horizontal ? val(top + topOf + **widget->h / 2): val(start + **widget->h / 2);
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

std::string to_string(Flexbox::Direction direction) {
  switch(direction) {
    case Flexbox::Direction::Column: return "Column";
    case Flexbox::Direction::ColumnReverse: return "ColumnReverse";
    case Flexbox::Direction::Row: return "Row";
    case Flexbox::Direction::RowReverse: return "RowReverse";
  }
}

std::string to_string(Flexbox::Wrap wrap) {
  switch(wrap) {
    case Flexbox::Wrap::Wrap: return "Wrap";
    case Flexbox::Wrap::Line: return "Line";
    case Flexbox::Wrap::WrapReverse: return "WrapReverse";
  }
}

std::string to_string(Flexbox::Justify justify) {
  switch (justify) {
    case Flexbox::Justify::Center: return "Center";
    case Flexbox::Justify::Start: return "Start";
    case Flexbox::Justify::End: return "End";
    case Flexbox::Justify::SpaceAround: return "SpaceAround";
    case Flexbox::Justify::SpaceBetween: return "SpaceBetween";
    case Flexbox::Justify::SpaceEvenly: return "SpaceEvenly";
  }
}

std::string to_string(Flexbox::Align align) {
  switch(align) {
    case Flexbox::Align::Center: return "Center";
    case Flexbox::Align::Start: return "Start";
    case Flexbox::Align::End: return "End";
    case Flexbox::Align::Stretch: return "Stretch";
  }
}

/*
// Widgets:
- Appear -> grow from 0% size to 120% size, shrink back to 100%
- Disappear -> inverse of appear
- Disappear -> (in flexbox) shrink in direction of flexbox resize
- Appear -> (in flexbox) grow in direction of flexbox resize
- Slide over -> -0.5 cosine movement to target position

Window:
- Snap to -> cosine movement starting at 4 corners
- Minimize -> fold closed (from both sides) into shrinking icon
- Maximize -> fold open (from both sides) from icon to window
- Close -> shrink from 100% opaque to 70% 30% opaque, then disappear
- Open -> spawn at 70% size 30% opacity, grow to 100% opaque
- Crash -> Shatter like window
*/


