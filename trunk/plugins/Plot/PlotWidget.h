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

#ifndef PLUGIN_PLOT_WIDGET_HPP
#define PLUGIN_PLOT_WIDGET_HPP

#include <QtGui>

#include "PlotWidgetDate.h"
#include "PlotWidgetScale.h"
#include "PlotWidgetArea.h"
#include "Object.h"
#include "XMap.h"
#include "YMap.h"
#include "Bars.h"


class PlotWidget : public QWidget
{
  Q_OBJECT
  
  signals:
    void signalSettingsChanged ();

  public:
    PlotWidget (QWidget *, Object *);
    ~PlotWidget ();
    void setDates (Bars *, QString key, QString length);
    int index ();
    int spacing ();
    PlotWidgetArea * plotArea ();
    PlotWidgetScale * plotScale ();
    PlotWidgetDate * plotDate ();
    YMap *getYMap ();
    XMap *getXMap ();
    QString name ();

  public slots:
    void draw ();
    void clear ();
    void setIndex (int);
    void setSpacing (int);
    void setName (QString);
    void dateToggled (bool);
    
  protected:
    PlotWidgetDate *_plotDate;
    PlotWidgetScale *_plotScale;
    PlotWidgetArea *_plotArea;
    YMap *_ymap;
    XMap *_xmap;
};

#endif
