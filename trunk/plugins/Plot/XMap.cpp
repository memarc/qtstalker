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

#include "XMap.h"

#include <QDebug>


XMap::XMap ()
{
  _spacing = 8;
  _index = -1;
  _interval = 1;
  _length = 1;
  
  _lengths << "1M";
  _lengths << "5M";
  _lengths << "10M";
  _lengths << "15M";
  _lengths << "30M";
  _lengths << "60M";
  _lengths << "D";
  _lengths << "W";
  _lengths << "M";

  _ticks = new PlotWidgetDateTick;
}

XMap::~XMap ()
{
  delete _ticks;
  qDeleteAll(_dates);
}

void
XMap::clear ()
{
  qDeleteAll(_dates);
  _dates.clear();
  _dateIndex.clear();
}

void
XMap::setDates (Bars *bars, QString dateKey, QString tlength)
{
  clear();
  
  int length = _lengths.indexOf(tlength);
  if (length == -1)
  {
    qDebug() << "XMap::setDates: invalid length" << tlength;
    return;
  }
  _length = length;
  
  QMapIterator<int, Bar *> it(bars->_bars);
  while (it.hasNext())
  {
    it.next();
    Bar *d = it.value();
    
    QDateTime ed, vd;
    QDateTime sd = d->date;
    if (! dateInterval(_length, 1, sd, ed, vd))
      continue;
    
    QString key = ed.toString("yyyyMMddHHmmss");
    
    XMapItem *item = _dates.value(key);
    if (! item)
    {
      item = new XMapItem;
      item->date = vd;
      item->index = -1;
      _dates.insert(key, item);
    }
  }
  
  QList<QString> keys = _dates.keys();
  qSort(keys);
  
  for (int pos = 0; pos < keys.size(); pos++)
  {
    XMapItem *i = _dates.value(keys.at(pos));
    i->index = pos;
    _dateIndex.insert(pos, i);
  }
  
  _ticks->setTicks(bars, dateKey);
}

int
XMap::size ()
{
  return _dates.size();
}

QDateTime
XMap::indexToDate (int index)
{
  if (index < 0 || index >= _dates.size())
    return QDateTime();

  XMapItem *i = _dateIndex.value(index);
  if (! i)
    return QDateTime();
  
  return i->date;
}

int
XMap::xToIndex (int x)
{
  int i = (x / _spacing) + _index;
  
  if (i >= _dates.size())
    i = _dates.size();
  
  if (i < _index)
    i = _index;
  
  return i;
}

void
XMap::setSpacing (int d)
{
  _spacing = d;
}

int
XMap::spacing ()
{
  return _spacing;
}

void
XMap::setIndex (int d)
{
  _index = d;
}

int
XMap::index ()
{
  return _index;
}

int
XMap::endPageIndex (int width)
{
  int i = _index + (width / _spacing) - 1;
  if (width % _spacing != 0)
    i++;
  return i;
}

int
XMap::indexToX (int index)
{
  return (index - _index) * _spacing;
}

PlotWidgetDateTick *
XMap::dateTicks ()
{
  return _ticks;
}

int
XMap::dateToIndex (QDateTime d)
{
  QDateTime dt = d;
  if (! dt.isValid())
    return -1;

  QDateTime ed, vd;
  if (! dateInterval(_length, _interval, dt, ed, vd))
    return -1;
  
  QString key = ed.toString("yyyyMMddHHmmss");
    
  XMapItem *item = _dates.value(key);
  if (! item)
    return -1;
  
  return item->index;
}

int
XMap::dateInterval (int length, int, QDateTime &sd, QDateTime &ed, QDateTime &vd)
{
  switch (length)
  {
    case 0: // 1M
      sd.setTime(QTime(sd.time().hour(), sd.time().minute(), 0, 0));
      ed = sd;
      ed = ed.addSecs(60);
      ed = ed.addSecs(-1);
      vd = ed;
      break;
    case 1: // 5M
    {
      int tint = sd.time().minute() / 5;
      sd.setTime(QTime(sd.time().hour(), tint * 5, 0, 0));
      ed = sd;
      ed = ed.addSecs(300);
      ed = ed.addSecs(-1);
      vd = ed;
      break;
    }
    case 2: // 10M
    {
      int tint = sd.time().minute() / 10;
      sd.setTime(QTime(sd.time().hour(), tint * 10, 0, 0));
      ed = sd;
      ed = ed.addSecs(600);
      ed = ed.addSecs(-1);
      vd = ed;
      break;
    }
    case 3: // 15M
    {
      int tint = sd.time().minute() / 15;
      sd.setTime(QTime(sd.time().hour(), tint * 15, 0, 0));
      ed = sd;
      ed = ed.addSecs(900);
      ed = ed.addSecs(-1);
      vd = ed;
      break;
    }
    case 4: // 30M
    {
      int tint = sd.time().minute() / 30;
      sd.setTime(QTime(sd.time().hour(), tint * 30, 0, 0));
      ed = sd;
      ed = ed.addSecs(1800);
      ed = ed.addSecs(-1);
      vd = ed;
      break;
    }
    case 5: // 60M
      sd.setTime(QTime(sd.time().hour(), 0, 0, 0));
      ed = sd;
      ed = ed.addSecs(3600);
      ed = ed.addSecs(-1);
      vd = ed;
      break;
    case 6: // D
      sd.setTime(QTime(0, 0, 0, 0));
      ed = sd;
      ed = ed.addDays(1);
      ed = ed.addSecs(-1);
      vd = ed;
      break;
    case 7: // W
      sd.setTime(QTime(0, 0, 0, 0));
      sd = sd.addDays(- (sd.date().dayOfWeek() - 1));
      ed = sd;
      ed = ed.addDays(7);
      ed = ed.addSecs(-1);
      vd = ed;
      break;
    case 8: // M
      sd.setTime(QTime(0, 0, 0, 0));
      sd = sd.addDays(- (sd.date().day() - 1));
      ed = sd;
      ed = ed.addDays(ed.date().daysInMonth());
      ed = ed.addSecs(-1);
      vd = ed;
      break;
    default:
      return 0;
      break;
  }
  
  return 1;
}
