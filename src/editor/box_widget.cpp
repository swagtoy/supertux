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

#include "editor/box_widget.hpp"

#include <fmt/format.h>
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"
#include "math/util.hpp"
#include "video/viewport.hpp"
#include "video/video_system.hpp"
#include "supertux/resources.hpp"

BoxWidget::BoxWidget(const Rectf& props, float padding_w, float padding_h) :
  m_box{props},
  m_padding_w{padding_w},
  m_padding_h{padding_h},
  m_is_hovered{false}
{
}

BoxWidget::BoxWidget(const Rectf& props, float padding) :
  BoxWidget(std::move(props), std::move(padding), std::move(padding))
{ }

void
BoxWidget::draw(DrawingContext& context)
{
}

void
BoxWidget::update(float dt_sec)
{
}

void
BoxWidget::setup()
{
}

void
BoxWidget::on_window_resize()
{
}

const Rectf BoxWidget::box()
{
  return m_box.grown(m_padding_w, m_padding_h);
}


bool
BoxWidget::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
  
  return false;
}

bool
BoxWidget::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);

  return false;
}

bool
BoxWidget::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);

  m_is_hovered = box().contains(mouse_pos);
  return m_is_hovered;
}

/* EOF */
