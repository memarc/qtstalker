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

#ifndef PLUGIN_PLOT_WIDGET_GRID_HPP
#define PLUGIN_PLOT_WIDGET_GRID_HPP

#include <QObject>
#include <QColor>
#include <QPainter>

#include "XMap.h"
#include "YMap.h"

class PlotWidgetGrid : public QObject
{
  Q_OBJECT
  
  public:
    PlotWidgetGrid (QObject *, XMap *xmap, YMap *ymap);
    ~PlotWidgetGrid ();
    void clear ();
    void draw (QPainter *);
    void drawXGrid (QPainter *);
    void drawYGrid (QPainter *);
    void enableXGrid (bool);
    bool xGrid ();
    void enableYGrid (bool);
    bool yGrid ();
    void setColor (QColor);
    QColor color ();
    
  private:
    XMap *_xmap;
    YMap *_ymap;
    QColor _color;
    bool _xGrid;
    bool _yGrid;
};

#endif


