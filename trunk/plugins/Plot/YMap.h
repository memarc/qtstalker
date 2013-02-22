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

#ifndef PLUGIN_PLOT_YMAP_HPP
#define PLUGIN_PLOT_YMAP_HPP

#include <QStringList>
#include <QList>
#include <QMap>
#include <QRect>

#include "XMap.h"


typedef struct
{
  double high;
  double low;
  
} HighLowRangeItem;


class YMap
{
  public:
    YMap ();
    ~YMap ();
    void clear ();
    int convertToY (double);
    double convertToVal (int);
    QList<double> scaleArray ();
    double logScaleHigh ();
    double logRange();
    void setHeight (int);
    int height ();
    bool logFlag ();
    double low ();
    void setLog (bool);
    void setHighLow (int index, double high, double low);
    void update (XMap *, QRect);

  protected:
    int _height;
    bool _logScale;
    double _scaleHigh;
    double _scaleLow;
    double _logScaleHigh;
    double _logRange;
    double _range;
    double _scaler;
    QStringList _scaleList;
    QMap<int, HighLowRangeItem *> _data;
};

#endif
