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

EditorMenubarWidget::EditorMenubarWidget(Editor& editor) :
	m_editor{editor},
	m_buttons{},
	// -20.0f is a hack for rounding
	m_rect(-g_config->menuroundness, -g_config->menuroundness, 500.0f, 40.0f)
{
}

void
EditorMenubarWidget::draw(DrawingContext& context)
{
  context.color().draw_filled_rect(m_rect, g_config->editorcolor, g_config->menuroundness,
                                   LAYER_GUI-5);

  // Would move to update function later, just a stub.
  std::list<char const*> things = {"File", "Edit", "Stuff", "AI", "Tools", "Help"};
  
  
  int i = 0;
  for (auto str: things)
	{
		
		context.color().draw_text(Resources::normal_font, str, Vector(20.0f+(i*80), 10.0f), ALIGN_LEFT, 999999, Color(1.0f, 1.0f, 1.0f, 1.0f));
		++i;
	}
}

void
EditorMenubarWidget::update(float dt_sec)
{
}

void
EditorMenubarWidget::setup()
{
	auto menubar_add = [&](const std::string& text) { m_buttons.push_back(EditorMenubarButtonWidget(*this, text)); };
	
	menubar_add("File");
	menubar_add("Edit");
	menubar_add("AI");
	menubar_add("Info");
	menubar_add("Help");
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
