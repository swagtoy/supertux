//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_MATH_RECT_HPP
#define HEADER_SUPERTUX_MATH_RECT_HPP

#include <iosfwd>

#include <algorithm>
#include <tuple>
#include <SDL.h>
#include <type_traits>

#include "math/size.hpp"

template<typename T>
class Rect_t
{
public:
  static Rect_t<T> from_center(T center_x, T center_y, T width, T height)
  {
      return {center_x - width / static_cast<T>(2),
              center_y - height / static_cast<T>(2),
              center_x + width / static_cast<T>(2),
              center_y + height / static_cast<T>(2)};
  }

public:
  Rect_t() :
    left(0),
    top(0),
    right(0),
    bottom(0)
  {}

  Rect_t(const Rect_t& rhs) = default;
  Rect_t& operator=(const Rect_t& rhs) = default;

  Rect_t(T left_, T top_, T right_, T bottom_) :
    left(left_),
    top(top_),
    right(right_),
    bottom(bottom_)
  {}

  Rect_t(T left_, T top_, const Size& size) :
    left(left_),
    top(top_),
    right(left_ + size.width),
    bottom(top_ + size.height)
  {}

  Rect_t(const SDL_Rect& rect) :
    left(rect.x),
    top(rect.y),
    right(rect.x + rect.w),
    bottom(rect.y + rect.h)
  {}

  //explicit Rect_t(const Rect_tf& other);

  bool operator==(const Rect_t& other) const
  {
    return (left == other.left &&
            top == other.top &&
            right == other.right &&
            bottom == other.bottom);
  }

  bool contains(T x, T y) const
  {
    return (left <= x && x < right &&
            top <= y && y < bottom);
  }

  bool contains(const Rect_t& other) const
  {
    return (left <= other.left && other.right <= right &&
            top <= other.top && other.bottom <= bottom);
  }

  const T& get_left() const { return left; }
  const T& get_right() const { return right; }
  const T& get_top() const { return top; }
  const T& get_bottom() const { return bottom; }

  T get_width()  const { return right - left; }
  T get_height() const { return bottom - top; }
  Size get_size() const { return Size(right - left, bottom - top); }
  T get_area() const { return get_width() * get_height(); }

  bool empty() const
  {
    return (get_width() <= 0 ||
            get_height() <= 0);
  }

  bool valid() const
  {
    return left <= right && top <= bottom;
  }

  Rect_t normalized() const
  {
    return {std::min(left, right),
            std::min(top, bottom),
            std::max(left, right),
			std::max(top, bottom)};
  }

  Rect_t moved(T x, T y) const
  {
    return {left + x,
            top + y,
            right + x,
            bottom + y};
  }

  Rect_t grown(T border) const
  {
    return {left - border,
            top - border,
            right + border,
            bottom + border};
  }

  Rect_t<float> to_rectf() const;
  SDL_Rect to_sdl() const
  {
    return { left, top, get_width(), get_height() };
  }

  bool operator<(const Rect_t& other) const {
    return std::tie(left, top, right, bottom) < std::tie(other.left, other.top, other.right, other.bottom);
  }
  
private:
  Vectori<T> m_p1;
  T left;
  T top;
  T right;
  T bottom;
};

//std::ostream& operator<<(std::ostream& out, const Rect_t& rect);

using Rect = Rect_t<int>;
using Rectf = Rect_t<float>;

#endif

/* EOF */
