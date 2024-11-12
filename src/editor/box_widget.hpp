//  SuperTux
//  Copyright (C) 2024 Hyland B. <me@ow.swag.toys>
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

// This is a generic box widget that can handle events and such for
//  you. Please use this instead of testing for events yourself. It can
//  also draw a box of your choice, but it's not the only usage.

#ifndef HEADER_SUPERTUX_EDITOR_BOX_WIDGET_HPP
#define HEADER_SUPERTUX_EDITOR_BOX_WIDGET_HPP

#include "editor/widget.hpp"

#include <functional>
#include <string>

#include "math/rectf.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"

// All class, no style. This may need to be improved in the
// future. Caching may be redundant especially when considering that
// it's not being done 1000 times a second. Please decide when moving
// widgets into the entire codebase if this is okay.
class BoxWidget : public Widget
{
public:
  BoxWidget(const Rectf& box_props, float padding = 0.0f);
  BoxWidget(const Rectf& box_props, float padding_w, float padding_h);
  BoxWidget(BoxWidget&&) = default;
  
  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;

  virtual void setup() override;
  virtual void on_window_resize() override;

  virtual bool on_mouse_button_up(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_button_down(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_motion(const SDL_MouseMotionEvent& motion) override;
  
  void set_size(float w, float h)
  {
    m_box.set_size(w, h);
    set_cached_padding();
  }
  
  // This naming scheme sucks and so does this design really but I couldn't think of anything better :(
  // Maybe we could just not cache anything?
  /** @return Return "cached" box, meaning the paddings are calculated. Use m_box  */
  inline const Rectf& box() { return m_box_cached; }
  inline bool hovered() { return m_is_hovered; }
  
protected:
  Rectf m_box;
  
private:
  float m_padding_w, m_padding_h;
  // Cached with padding, uses very little memory to cache a very little cpu usage calculation
  Rectf m_box_cached;
  bool m_is_hovered;
  
  void set_cached_padding();
  
  //EditorMenubarButtonWidget(const EditorMenubarButtonWidget&) = delete;
  BoxWidget& operator=(const BoxWidget&) = delete;
};

#endif

/* EOF */
