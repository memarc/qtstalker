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

#include "PlotWidgetScale.h"
#include "Util.h"

#include <QPen>
#include <QPoint>
#include <math.h>
#include <QFontMetrics>
#include <QString>
#include <QDebug>


PlotWidgetScale::PlotWidgetScale (QWidget *p, YMap *map) : QWidget (p)
{
  _ymap = map;
  _color = QColor(Qt::white);
  _backgroundColor = QColor(Qt::black);
  
  QFont font;
  font.setFamily("Helvetica");
  font.setPointSize(10);
  font.setWeight(50);
  setFont(font);
  
  setAttribute(Qt::WA_NoSystemBackground, TRUE); // disable background fill
  
  setMinimumHeight(1);
  setMaximumWidth(60);
  setMinimumWidth(60);
}

PlotWidgetScale::~PlotWidgetScale ()
{
}

void
PlotWidgetScale::clear ()
{
  clearPoints();
}

void
PlotWidgetScale::paintEvent (QPaintEvent *event)
{
  QPainter p(this);
  QRect rect = event->rect();
  p.drawPixmap(rect, _pic, rect);
}

void
PlotWidgetScale::resizeEvent (QResizeEvent *event)
{
  _pic = QPixmap(event->size());
  draw();
  QWidget::resizeEvent(event);
}

void
PlotWidgetScale::draw ()
{
  // fill background with background color
  _pic.fill(_backgroundColor);
  
  if (isVisible())
    drawScale();

  update();
}

void
PlotWidgetScale::drawScale ()
{
  QPainter p(&_pic);
  p.setFont(font());
  p.setPen(QPen(_color, 1, Qt::SolidLine));
  p.setRenderHint(QPainter::TextAntialiasing, TRUE);

  QList<double> scaleArray = _ymap->scaleArray();
  
  QFontMetrics fm(font());

  Util strip;
  for (int pos = 0; pos < scaleArray.size(); pos++)
  {
    int y = _ymap->convertToY(scaleArray.at(pos));
    p.drawLine(0, y, 4, y);

    // draw the text
    QString s;
    strip.strip(scaleArray.at(pos), 4, s);
    
    // abbreviate too many (>=3) trailing zeroes in large numbers on y-axes
    bool flag = FALSE;
    if (s.toDouble() < 0)
    {
      flag = TRUE;
      s.remove(0, 1);  
    }
      
    if (s.toDouble() >= 1000000000)
    {
      strip.strip(s.toDouble() / 1000000000, 4, s);
      s.append("b");
    }
    else
    {
      if (s.toDouble() >= 1000000)
      {
        strip.strip(s.toDouble() / 1000000, 4, s);
	s.append("m");
      }
    }
      
    if (flag)
      s.prepend("-");
    
    p.drawText(7, y + (fm.height() / 2), s);
  }
  
  // draw vertical border line
  p.drawLine (0, 0, 0, _pic.height());

  // draw the value points for all the curves
  drawPoints(p);
}

void
PlotWidgetScale::drawPoints (QPainter &p)
{
  int x = 0;
  int offset = 8;
  QFontMetrics fm(font());
  Util strip;

  for (int pos = 0; pos < _pointValues.size(); pos++)
  {
    QString s;
    strip.strip(_pointValues.at(pos), 4, s);

    int y = _ymap->convertToY(_pointValues.at(pos));

    QRect rc = p.boundingRect(x + offset,
                              y - (fm.height() / 2),
                              0,
                              0,
                              0,
                              s);

    // draw the arrow banner
    QPolygon arrow;
    arrow << QPoint(x, y); // point
    arrow << rc.topLeft();
    arrow << rc.topRight();
    arrow << rc.bottomRight();
    arrow << rc.bottomLeft();
    arrow << QPoint(x, y); // close point

    p.setBrush(_pointColors.at(pos));
    p.drawPolygon(arrow, Qt::OddEvenFill);

    p.setPen(_backgroundColor);
    p.drawText(rc, s);
  }
}

void
PlotWidgetScale::clearPoints ()
{
  _pointColors.clear();
  _pointValues.clear();
}

void
PlotWidgetScale::addPoint (QColor c, double v)
{
  _pointColors << c;
  _pointValues << v;
}

void
PlotWidgetScale::setBackgroundColor (QColor d)
{
  _backgroundColor = d;
}

QColor
PlotWidgetScale::backgroundColor ()
{
  return _backgroundColor;
}

void
PlotWidgetScale::setColor (QColor d)
{
  _color = d;
}

QColor
PlotWidgetScale::color ()
{
  return _color;
}
