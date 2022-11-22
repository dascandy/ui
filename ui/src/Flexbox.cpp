#include <ui/Flexbox.hpp>
#include <span>
#include <numeric>
#include <string>

void Flexbox::Render() {
  for (auto& widget : widgets) {
    widget->Render();
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

void Flexbox::setMargin(double newMargin) {
  if (margin != newMargin) {
    margin = newMargin;
    if (allowRelayout) Relayout();
  }
}

std::pair<double, double> getJustify(Flexbox::Justify justify, size_t margin, double spaceForMargin, size_t widgetCount) {
  if (spaceForMargin < 0) return { margin, margin };

  switch(justify) {
    case Flexbox::Justify::Start:
    default:
      return { margin, margin };
    case Flexbox::Justify::End:
      return { spaceForMargin + margin, margin };
    case Flexbox::Justify::Center:
      return { spaceForMargin / 2 + margin, margin };
    case Flexbox::Justify::SpaceBetween:
      return { margin, spaceForMargin / (widgetCount - 1) };
    case Flexbox::Justify::SpaceAround:
      return { spaceForMargin / (widgetCount) / 2, spaceForMargin / (widgetCount) };
    case Flexbox::Justify::SpaceEvenly:
      return { spaceForMargin / (widgetCount + 1), spaceForMargin / (widgetCount + 1) };
  }
}

static double getAlign(Flexbox::Align align, double boxWidth, double itemWidth) {
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
  double available = (horizontal ? **w: **h);
  std::span<std::unique_ptr<Widget>> ws = widgets;
  std::vector<std::span<std::unique_ptr<Widget>>> rows;
  double curSize = margin;
  
  size_t start = 0, index = 0;
  double totalRowHeight = margin;
  double rowMaxHeight = 0;
  while (index < ws.size()) {
    if (wrap != Wrap::Line && curSize > margin && curSize + margin + (horizontal ? **(ws[index]->w) : **(ws[index]->h)) > available) {
      rows.push_back(ws.subspan(start, index-start));
      totalRowHeight += margin + rowMaxHeight;
      start = index;
      curSize = margin;
      rowMaxHeight = 0;
    }
    curSize += margin + (horizontal ? **(ws[index]->w) : **(ws[index]->h));
    rowMaxHeight = std::max<double>(rowMaxHeight, (horizontal ? **(ws[index]->h) : **(ws[index]->w)));
    index++;
  }
  totalRowHeight += margin + rowMaxHeight;
  rows.push_back(ws.subspan(start));
  if (wrap == Wrap::WrapReverse) {
    std::reverse(rows.begin(), rows.end());
  }

  auto [topBefore, topBetween] = getJustify(vjustify, margin, (double)(horizontal ? **h : **w) - totalRowHeight, rows.size());
  double top = topBefore;
  for (auto row : rows) {
    double totalMajor = std::accumulate(row.begin(), row.end(), margin, [&](size_t current, std::unique_ptr<Widget>& widget){ return current + margin + (horizontal ? **widget->w : **widget->h); });
    double maxHeight = std::accumulate(row.begin(), row.end(), 0.0, [&](double current, std::unique_ptr<Widget>& widget) { return std::max<double>(current, horizontal ? **widget->h : **widget->w); });
    auto [spaceBefore, spaceBetween] = getJustify(justify, margin, (double)(horizontal ? **w : **h) - totalMajor, row.size());
    double start = reverse ? **w - spaceBefore : spaceBefore;
    for (auto& widget : row) {
      if (reverse) start -= (horizontal ? **widget->w : **widget->h);
      if (align == Align::Stretch) (horizontal ? widget->h : widget->w) = val(maxHeight);
      double topOf = getAlign(align, maxHeight, (horizontal ? **widget->h : **widget->w));
      widget->x = horizontal ? val(start) : val(top + topOf);
      widget->y = horizontal ? val(top + topOf) : val(start);
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


