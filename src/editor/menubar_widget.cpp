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

#include "editor/menubar_widget.hpp"

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/viewport.hpp"
#include "video/video_system.hpp"

constexpr int CORNER_RADIUS = 15.0f;

EditorMenubarWidget::EditorMenubarWidget(Editor& editor) :
	m_editor{editor},
	// -20.0f is a hack for rounding
	m_rect(-CORNER_RADIUS, -CORNER_RADIUS, 500.0f, 40.0f)
{
}

void
EditorMenubarWidget::draw(DrawingContext& context)
{
  context.color().draw_filled_rect(m_rect, Color(114.0f, 135.0f, 176.0f, 0.75f), CORNER_RADIUS,
                                   LAYER_GUI-5);
}

void
EditorMenubarWidget::update(float dt_sec)
{
}

void
EditorMenubarWidget::setup()
{
}

void
EditorMenubarWidget::on_window_resize()
{
}

bool
EditorMenubarWidget::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  if (button.button != SDL_BUTTON_LEFT) return false;

  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
  
  return false;
}

bool
EditorMenubarWidget::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  if (button.button != SDL_BUTTON_LEFT) return false;

  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);

  return false;
}

bool
EditorMenubarWidget::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);

  return false;
}

/* EOF */
