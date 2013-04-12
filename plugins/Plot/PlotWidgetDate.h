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

#ifndef PLUGIN_PLOT_WIDGET_DATE_HPP
#define PLUGIN_PLOT_WIDGET_DATE_HPP

#include <QtGui>

#include "PlotWidgetDateTick.h"
#include "XMap.h"
#include "Bars.h"

class PlotWidgetDate : public QWidget
{
  Q_OBJECT
  
  public:
    PlotWidgetDate (QWidget *, XMap *);
    ~PlotWidgetDate ();
    void clear ();
    void setDates (Bars *, QString dateKey);
    void setBackgroundColor (QColor);
    QColor backgroundColor ();
    void setColor (QColor);
    QColor color ();

  public slots:
    void draw ();
    
  protected:
    virtual void resizeEvent (QResizeEvent *);
    virtual void paintEvent (QPaintEvent *);
    void drawDates ();

    
    XMap *_xmap;
    QColor _backgroundColor;
    QColor _color;
    QPixmap _pic;
    PlotWidgetDateTick *_ticks;
};

#endif


