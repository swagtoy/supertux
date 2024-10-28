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

#ifndef HEADER_SUPERTUX_MATH_SIZE_HPP
#define HEADER_SUPERTUX_MATH_SIZE_HPP

#include <iosfwd>

template<typename T>
class Size_t final
{
public:
  Size_t() :
    width(static_cast<T>(0)),
    height(static_cast<T>(0))
  {}

  Size_t(T width_, T height_) :
    width(width_),
    height(height_)
  {}

  Size_t(const Size_t& rhs) = default;
  Size_t& operator=(const Size_t& rhs) = default;

  explicit Size_t<int>(const Size_t<float>& rhs);

  Size_t& operator*=(T factor)
  {
    width  *= factor;
    height *= factor;
    return *this;
  }

  Size_t& operator/=(T divisor)
  {
    width  /= divisor;
    height /= divisor;
    return *this;
  }

  Size_t& operator+=(const Size_t& rhs)
  {
    width  += rhs.width;
    height += rhs.height;
    return *this;
  }

  Size_t& operator-=(const Size_t& rhs)
  {
    width  -= rhs.width;
    height -= rhs.height;
    return *this;
  }

  bool is_valid() const 
  {
    return width > 0 && height > 0;
  }

public:
  T width;
  T height;
};

template<typename T>
inline Size_t<T> operator*(const Size_t<T>& lhs, T factor)
{
  return Size_t(lhs.width  * factor,
              lhs.height * factor);
}

template<typename T>
inline Size_t<T> operator*(T factor, const Size_t<T>& rhs)
{
  return Size_t(rhs.width  * factor,
              rhs.height * factor);
}

template<typename T>
inline Size_t<T> operator/(const Size_t<T>& lhs, T divisor)
{
  return Size_t(lhs.width  / divisor,
              lhs.height / divisor);
}

template<typename T>
inline Size_t<T> operator+(const Size_t<T>& lhs, const Size_t<T>& rhs)
{
  return Size_t(lhs.width  + rhs.width,
              lhs.height + rhs.height);
}

template<typename T>
inline Size_t<T> operator-(const Size_t<T>& lhs, const Size_t<T>& rhs)
{
  return Size_t(lhs.width  - rhs.width,
              lhs.height - rhs.height);
}

template<typename T>
inline bool operator==(const Size_t<T>& lhs, const Size_t<T>& rhs)
{
  return (lhs.width == rhs.width) && (lhs.height == rhs.height);
}

template<typename T>
inline bool operator!=(const Size_t<T>& lhs, const Size_t<T>& rhs)
{
  return (lhs.width != rhs.width) || (lhs.height != rhs.height);
}

template<typename T>
std::ostream& operator<<(std::ostream& s, const Size_t<T>& size);

using Size = Size_t<int>;
using Sizef = Size_t<float>;

#endif

/* EOF */
