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

#include "PlotWidgetDate.h"

#include <QDebug>


PlotWidgetDate::PlotWidgetDate (QWidget *p, XMap *xmap) : QWidget (p)
{
  _xmap = xmap;
  _color = QColor(Qt::white);
  _backgroundColor = QColor(Qt::black);

  QFont font;
  font.setFamily("Helvetica");
  font.setPointSize(10);
  font.setWeight(50);
  setFont(font);
  
  _ticks = new PlotWidgetDateTick;
  
  setAttribute(Qt::WA_NoSystemBackground, TRUE); // disable background fill
  
  setMaximumHeight(30);
  setMinimumHeight(30);
}

PlotWidgetDate::~PlotWidgetDate ()
{
  clear();
  delete _ticks;
}

void
PlotWidgetDate::clear ()
{
  _ticks->clear();
}

void
PlotWidgetDate::paintEvent (QPaintEvent *event)
{
  QPainter p(this);
  QRect rect = event->rect();
  p.drawPixmap(rect, _pic, rect);
}

void
PlotWidgetDate::resizeEvent (QResizeEvent *event)
{
  _pic = QPixmap(event->size());
  draw();
  QWidget::resizeEvent(event);
}

void
PlotWidgetDate::draw ()
{
  _pic.fill(_backgroundColor);
  
  if (isVisible())
    drawDates();

  update();
}

void
PlotWidgetDate::drawDates ()
{
  if (! _ticks->size() || ! isVisible())
    return;

  QPainter p(&_pic);
  p.setPen(_color);
  p.setFont(font());
  p.setRenderHint(QPainter::TextAntialiasing, TRUE);
    
  QFontMetrics fm(font());
  
  int x = 0;
  int y = 0;
  int x2 = _pic.width();
  int y2 = height();
  
  // clear date area
  p.fillRect(x, y, x2, y2, _backgroundColor);
  
  // draw the seperator line
  p.drawLine (x, y, x2, y);

  // get index from XMap
  int pos = _xmap->index();
  
  // get spacing from DateMap
  int spacing = _xmap->spacing();
  
  while(x <= x2)
  {
    int type = 0;
    QString text;
    
    if (_ticks->tick(pos, type, text))
    {
      switch (type)
      {
        case 1: // long tick
          p.drawLine (x, y + 1, x, y2 - fm.height() - 2);
          p.drawText (x - (fm.width(text, -1) / 2),
                      y2 - 2,
                      text);
          break;
        default: // short tick
          p.drawLine (x, y + 1, x, y + 4);
          p.drawText (x - (fm.width(text, -1) / 2),
                      y2 - fm.height() + 2,
                      text);
          break;
      }
    }
              
    x += spacing;
    pos++;
  }
}

void
PlotWidgetDate::setDates (QMap<int, Data *> &bars, QString k)
{
  clear();
  _ticks->setTicks(bars, k);
}

void
PlotWidgetDate::setBackgroundColor (QColor d)
{
  _backgroundColor = d;
}

QColor
PlotWidgetDate::backgroundColor ()
{
  return _backgroundColor;
}

void
PlotWidgetDate::setColor (QColor d)
{
  _color = d;
}

QColor
PlotWidgetDate::color ()
{
  return _color;
}
