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

#ifndef PLUGIN_PLOT_XMAP_HPP
#define PLUGIN_PLOT_XMAP_HPP

#include <QList>
#include <QDateTime>
#include <QHash>
#include <QStringList>

#include "PlotWidgetDateTick.h"

typedef struct
{
  QDateTime date;
  int index;
  
} XMapItem;


class XMap
{
  public:
    XMap ();
    ~XMap ();
    void clear ();
    void setDates (QMap<int, Data *> &, QString dateKey, QString length);
    QDateTime indexToDate (int);
    int size ();
    int xToIndex (int x);
    void setSpacing (int);
    int spacing ();
    void setIndex (int);
    int index ();
    int endPageIndex (int width);
    int indexToX (int index);
    PlotWidgetDateTick * dateTicks ();
    int dateInterval (int length, int interval, QDateTime &sd, QDateTime &ed, QDateTime &vd);
    int dateToIndex (QDateTime);

  private:
    QHash<QString, XMapItem *> _dates;
    QHash<int, XMapItem *> _dateIndex;
    QStringList _lengths;
    int _spacing;
    int _index;
    int _interval;
    int _length;
    PlotWidgetDateTick *_ticks;
};

#endif


