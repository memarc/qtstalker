/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "PlotWidgetGrid.h"
#include "PlotWidgetDateTick.h"

#include <QPen>
#include <QMap>
#include <QDebug>


PlotWidgetGrid::PlotWidgetGrid (QObject *p, XMap *xmap, YMap *ymap) : QObject(p)
{
  _xmap = xmap;
  _ymap = ymap;
  _color = QColor("#626262");
  _xGrid = TRUE;
  _yGrid = TRUE;
}

PlotWidgetGrid::~PlotWidgetGrid ()
{
  clear();
}

void
PlotWidgetGrid::clear ()
{
}

void
PlotWidgetGrid::draw (QPainter *p)
{
  if (_xGrid)
    drawXGrid(p);
  
  if (_yGrid)
    drawYGrid(p);
}

void
PlotWidgetGrid::drawXGrid (QPainter *p)
{
  PlotWidgetDateTick *ticks = _xmap->dateTicks();
  
  p->setPen(QPen(_color, 1, Qt::DotLine));
  
  QRect rect = p->viewport();
  
  int x = 0;
  int x2 = rect.width();
  int pos = _xmap->index();
  
  while(x <= x2)
  {
    int type = 0;
    QString ts;
    if (ticks->tick(pos, type, ts))
    {
      if (type == 1)
        p->drawLine (x, 0, x, rect.height());
    }
              
    x += _xmap->spacing();
    pos++;
  }
}

void
PlotWidgetGrid::drawYGrid (QPainter *p)
{
  QRect rect = p->viewport();
  
  p->setPen(QPen(_color, 1, Qt::DotLine));
  
  QList<double> scaleArray = _ymap->scaleArray();

  for (int loop = 0; loop < scaleArray.size(); loop++)
  {
    int y = _ymap->convertToY(scaleArray.at(loop));
    p->drawLine (0, y, rect.width(), y);
  }
}

void
PlotWidgetGrid::enableXGrid (bool d)
{
  _xGrid = d;
}

bool
PlotWidgetGrid::xGrid ()
{
  return _xGrid;
}

void
PlotWidgetGrid::enableYGrid (bool d)
{
  _yGrid = d;
}

bool
PlotWidgetGrid::yGrid ()
{
  return _yGrid;
}

void
PlotWidgetGrid::setColor (QColor d)
{
  _color = d;
}

QColor
PlotWidgetGrid::color ()
{
  return _color;
}
