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

#include "YMap.h"
#include "Object.h"

#include <math.h>

YMap::YMap ()
{
  clear();
  
  _scaleList << ".00001";
  _scaleList << ".00002";
  _scaleList << ".00005";
  _scaleList << ".0001";
  _scaleList << ".0002";
  _scaleList << ".0005";
  _scaleList << ".001";
  _scaleList << ".002";
  _scaleList << ".005";
  _scaleList << ".01";
  _scaleList << ".02";
  _scaleList << ".05";
  _scaleList << ".1";
  _scaleList << ".2";
  _scaleList << ".5";
  _scaleList << "1";
  _scaleList << "2";
  _scaleList << "5";
  _scaleList << "10";
  _scaleList << "25";
  _scaleList << "50";
  _scaleList << "100";
  _scaleList << "250";
  _scaleList << "500";
  _scaleList << "1000";
  _scaleList << "2500";
  _scaleList << "5000";
  _scaleList << "10000";
  _scaleList << "25000";
  _scaleList << "50000";
  _scaleList << "100000";
  _scaleList << "250000";
  _scaleList << "500000";
  _scaleList << "1000000";
  _scaleList << "2500000";
  _scaleList << "5000000";
  _scaleList << "10000000";
  _scaleList << "25000000";
  _scaleList << "50000000";
  _scaleList << "100000000";
  _scaleList << "250000000";
  _scaleList << "500000000";
  _scaleList << "1000000000";
  _scaleList << "2500000000";
  _scaleList << "5000000000";
  _scaleList << "10000000000";
  _scaleList << "25000000000";
  _scaleList << "50000000000";
  _scaleList << "100000000000";
  _scaleList << "250000000000";
  _scaleList << "500000000000";
}

YMap::~YMap ()
{
  clear();
}

void
YMap::clear ()
{
  _height = 0;
  _logScale = 0;
  _scaleHigh = 0;
  _scaleLow = 0;
  _logScaleHigh = 0;
  _logRange = 0;
  _range = 0;
  _scaler = 0;

  qDeleteAll(_data);
  _data.clear();
}

int
YMap::convertToY (double val)
{
  if (_logScale)
  {
    if (val <= 0.0)
      return _height;
    else
      return (int) (_height * (_logScaleHigh - log(val)) / _logRange);
  }

  double t = val - _scaleLow;
  int y = (int) (t * _scaler);
  y = _height - y;
  if (y > _height)
    y = _height;
  
  return y;
}

double
YMap::convertToVal (int y)
{
  if (_logScale)
  {
    if (y >= _height)
      return _scaleLow;
    else
      return exp(_logScaleHigh - ((y * _logRange) / _height));
  }

  if (_height == 0)
    return 0;
    
  int p = _height - y;
  double val = _scaleLow + (p / _scaler) ;
  return val;
}

QList<double>
YMap::scaleArray ()
{
  int ticks = 2;
  for (; (ticks * 15) < _height; ticks++)
    ;
  ticks--;
  if (ticks > 10)
    ticks = 10;
    
  double interval = 0;
  for (int loop = 0; loop < _scaleList.size(); loop++)
  {
    interval = _scaleList.at(loop).toDouble();
    if ((_range / interval) < ticks)
      break;
  }

  QList<double> sa;
  double t = 0 - (ticks * interval);
  if (interval > 0)
  {
    while (t <= _scaleHigh)
    {
      t = t + interval;

      if (t >= _scaleLow)
        sa << t;
    }
  }

  return sa;
}

double
YMap::logScaleHigh ()
{
  return _logScaleHigh;
}

double
YMap::logRange ()
{
  return _logRange;
}

void
YMap::setHeight (int d)
{
  _height = d;
  
  // rescale
  _range = _scaleHigh - _scaleLow;
  _scaler = _height / _range;
}

int
YMap::height ()
{
  return _height;
}

bool
YMap::logFlag ()
{
  return _logScale;
}

double
YMap::low ()
{
  return _scaleLow;
}

void
YMap::setLog (bool d)
{
  _logScale = d;
}

void
YMap::setHighLow (int index, double high, double low)
{
  HighLowRangeItem *item = _data.value(index);
  if (! item)
  {
    item = new HighLowRangeItem;
    _data.insert(index, item);
    item->high = high;
    item->low = low;
    return;
  }
    
  if (high > item->high)
    item->high = high;
    
  if (low < item->low)
    item->low = low;
}

void
YMap::update (XMap *xmap, QRect rect)
{
  if (! _data.size())
    return;
  
  _scaleHigh = 0;
  _scaleLow = 0;
  int flag = 0;

  int start = xmap->index();
  int end = start + (rect.width() / xmap->spacing());

  for (int index = start; index <= end; index++)
  {
    HighLowRangeItem *item = _data.value(index);
    if (! item)
      continue;
    
    if (! flag)
    {
      _scaleHigh = item->high;
      _scaleLow = item->low;
      flag++;
      continue;
    }
    
    if (item->high > _scaleHigh)
      _scaleHigh = item->high;
    
    if (item->low < _scaleLow)
      _scaleLow = item->low;
  }
  
  // add a little space for the upper and lower scaling
  _scaleHigh = _scaleHigh * 1.01;

  _logScaleHigh = 1;
  _logRange = 0;
  if (_logScale)
  {
    _logScaleHigh = _scaleHigh > 0.0 ? log(_scaleHigh) : 1;
    double logScaleLow = _scaleLow > 0.0 ? log(_scaleLow) : 0;
    _logRange = _logScaleHigh - logScaleLow;
  }

  setHeight(rect.height());
  
//qDebug() << "OFCPlotHighLowRange::updateScaler" << start << end << scaleHigh << scaleLow;
}
