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
#include "supertux/resources.hpp"
#include "util/log.hpp"
#include "math/util.hpp"

EditorMenubarWidget::EditorMenubarWidget() :
	m_buttons{},
	// -20.0f is a hack for rounding
	m_rect(-g_config->menuroundness, -g_config->menuroundness, 0.0f, 40.0f)
{
}

void
EditorMenubarWidget::draw(DrawingContext& context)
{
	context.color().draw_filled_rect(m_rect, g_config->editorcolor, math::clamp(g_config->menuroundness, 0.0f, 16.0f),
                                   LAYER_GUI-5);

	for (auto& button: m_buttons)
	{
		button.draw(context);
	}
}

void
EditorMenubarWidget::update(float dt_sec)
{
}

void
EditorMenubarWidget::setup()
{
	float x_pos = 20;
	auto menubar_add = [&](const std::string& text) {
		m_buttons.emplace_back(text, x_pos);
	};
	
	menubar_add("File");
	menubar_add("Edit");
	menubar_add("AI");
	menubar_add("Layers");
	menubar_add("Info");
	menubar_add("Help");
	
	m_rect.set_width(x_pos + 3.0f);
}

void
EditorMenubarWidget::on_window_resize()
{
}

bool
EditorMenubarWidget::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  for (auto& btn : m_buttons) btn.on_mouse_button_up(button);
  return false;
}

bool
EditorMenubarWidget::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  for (auto& btn : m_buttons) btn.on_mouse_button_down(button);
  return false;
}

bool
EditorMenubarWidget::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  for (auto& btn : m_buttons) btn.on_mouse_motion(motion);
  
  return false;
}

/* EOF */
