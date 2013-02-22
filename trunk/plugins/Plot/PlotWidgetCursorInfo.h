/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#ifndef PLUGIN_PLOT_WIDGET_CURSOR_INFO_HPP
#define PLUGIN_PLOT_WIDGET_CURSOR_INFO_HPP

#include <QObject>
#include <QHash>
#include <QStringList>
#include <QMouseEvent>
#include <QPainter>
#include <QColor>
#include <QFont>

#include "XMap.h"
#include "Data.h"
#include "Object.h"


class PlotWidgetCursorInfo : public QObject
{
  Q_OBJECT
  
  signals:
    void signalInfo (Data);
    void signalDraw ();
    
  public:
    PlotWidgetCursorInfo (QObject *, XMap *);
    ~PlotWidgetCursorInfo ();
    void clear ();
    void draw (QPainter *);
    void setInfo (QMouseEvent *, QHash<QString, Object *> &, QString &name);
    void setEnabled (bool);
    bool enabled ();
    void setColor (QColor);
    QColor color ();
    void setFont (QFont);
    QFont font ();
    
  protected:
    bool _enabled;
    Data _info;
    QColor _color;
    QFont _font;
    XMap *_xmap;
    QStringList _fields;
};

#endif
