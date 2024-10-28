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

#include "editor/menubar_button_widget.hpp"

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"
#include "video/viewport.hpp"
#include "video/video_system.hpp"
#include "supertux/resources.hpp"

EditorMenubarButtonWidget::EditorMenubarButtonWidget(const std::string& text, float& x_pos_inc) :
	m_text{text},
	x_pos{x_pos_inc}
{
	x_pos_inc += Resources::normal_font->get_text_width(text);
}

void
EditorMenubarButtonWidget::draw(DrawingContext& context)
{
	context.color().draw_text(
		Resources::normal_font, m_text, {x_pos, 8.0f}, ALIGN_LEFT, 999999, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void
EditorMenubarButtonWidget::update(float dt_sec)
{
}

void
EditorMenubarButtonWidget::setup()
{
}

void
EditorMenubarButtonWidget::on_window_resize()
{
}

bool
EditorMenubarButtonWidget::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  if (button.button != SDL_BUTTON_LEFT) return false;

  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
  
  return false;
}

bool
EditorMenubarButtonWidget::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  if (button.button != SDL_BUTTON_LEFT) return false;

  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);

  return false;
}

bool
EditorMenubarButtonWidget::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);

  return false;
}

/* EOF */
