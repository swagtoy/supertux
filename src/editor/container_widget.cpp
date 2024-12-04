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

#include "container_widget.hpp"

ContainerWidget::ContainerWidget(const Rectf& props) :
  BoxWidget{props},
  m_widgets(),
  m_is_horizontal(false)
{ }

void
ContainerWidget::draw(DrawingContext& ctx)
{
}

void
ContainerWidget::update(float dt_sec)
{
}

void
ContainerWidget::add(const BoxWidget& box)
{
}

void
ContainerWidget::setup()
{
}

