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

#include "PlotWidgetDateTick.h"

#include <QtDebug>

PlotWidgetDateTick::PlotWidgetDateTick ()
{
}

PlotWidgetDateTick::~PlotWidgetDateTick ()
{
  clear();
}

void
PlotWidgetDateTick::clear ()
{
  qDeleteAll(_ticks);
  _ticks.clear();
}

int
PlotWidgetDateTick::tick (int index, int &type, QString &text)
{
  DateTickItem *t = _ticks.value(index);
  if (! t)
    return 0;
  
  type = t->type;
  text = t->text;
  
  return 1;
}

int
PlotWidgetDateTick::size ()
{
  return _ticks.size();
}

void
//PlotWidgetDateTick::setTicks (QList<QDateTime> &d, OFCTypeBarLength::Length l)
PlotWidgetDateTick::setTicks (Bars *bars, QString dateKey)
{
  clear();

  if (! bars->_bars.size())
    return;

  QDateTime pdate;
  QMapIterator<int, Bar *> it(bars->_bars);
  while (it.hasNext())
  {
    it.next();
    Bar *d = it.value();
    
    QDateTime date = d->date;
    if (! date.isValid())
      continue;
    
    if (! pdate.isValid())
    {
      pdate = date;
      continue;
    }

    // monthly, weekly, daily ticks
    int days = pdate.daysTo(date);
    //qDebug() << "PlotDateTick::setTicks: days=" << days;

    if (days)
    {
      if (days >= 28)
      {
        //qDebug() << "PlotDateTick::setTicks: monthly";
        if (date.date().year() != pdate.date().year())
        {
          DateTickItem *tick = new DateTickItem;
          tick->type = 1;
          tick->text = date.toString("yyyy");
          _ticks.insert(it.key(), tick);
        }
        pdate = date;
        continue;
      }
    
      if (days >= 7)
      {
        //qDebug() << "PlotDateTick::setTicks: weekly";
        if (date.date().month() != pdate.date().month())
        {
          // short tick
          DateTickItem *tick = new DateTickItem;
          tick->type = 0;
          tick->text = date.toString("M");

          if (date.date().month() == 1)
          {
            // long tick
            tick->type = 1;
            tick->text = date.toString("yy");
          }
      
          _ticks.insert(it.key(), tick);
        }
        pdate = date;
        continue;
      }
    
      //qDebug() << "PlotDateTick::setTicks: daily";
      if (date.date().month() != pdate.date().month())
      {
        // long tick
        DateTickItem *tick = new DateTickItem;
        tick->type = 1;
        tick->text = date.toString("MMM yy");
        _ticks.insert(it.key(), tick);
      }
      else
      {
        // if start of new week make a tick
        if (date.date().dayOfWeek() <  pdate.date().dayOfWeek())
        {
          DateTickItem *tick = new DateTickItem;
          tick->type = 0;
          tick->text = date.toString("d");
          _ticks.insert(it.key(), tick);
        }
      }
      pdate = date;
      continue;
    }
  
    int min = pdate.secsTo(date) / 60;
    //qDebug() << "PlotDateTick::setTicks: min=" << min;

    if (min)
    {
      // if next day
      if (date.date().day() != pdate.date().day())
      {
        // long tick
        DateTickItem *tick = new DateTickItem;
        tick->type = 1; // long
        tick->text = date.toString("MMM d");
        _ticks.insert(it.key(), tick);
	pdate = date;
        continue;
      }
      
      if (min > 60)
      {
	// hourly tick
        // short tick
        DateTickItem *tick = new DateTickItem;
        tick->type = 0; // short
        tick->text = QString::number(date.time().hour()) + ":00";
        _ticks.insert(it.key(), tick);
      }
    }
    
    pdate = date;
  }
  
//qDebug() << "PlotDateTick::setTicks" << d.size() << l << _ticks.size();   
  
/*  
  clear();
  
  switch (l)
  {
    case OFCTypeBarLength::_MIN1:
    case OFCTypeBarLength::_MIN5:
    case OFCTypeBarLength::_MIN10:
    case OFCTypeBarLength::_MIN15:
    case OFCTypeBarLength::_MIN30:
    case OFCTypeBarLength::_MIN60:
      minuteTicks(d, l);
      break;
    case OFCTypeBarLength::_WEEKLY:
      weeklyTicks(d);
      break;
    case OFCTypeBarLength::_MONTHLY:
      monthlyTicks(d);
      break;
    default:
      dailyTicks(d);
      break;
  }
  
//qDebug() << "PlotWidgetDateTick::setTicks" << d.size() << l << _ticks.size();   
*/
}

/*
void
PlotWidgetDateTick::minuteTicks (QList<QDateTime> &dates, OFCTypeBarLength::Length length)
{
  int loop = 0;
  QDateTime nextHour = dates.at(loop);
  QDateTime oldDay = nextHour;
  nextHour.setTime(QTime(nextHour.time().hour(), 0, 0, 0));
  
  if (length != OFCTypeBarLength::_MIN1)
    nextHour = nextHour.addSecs(7200);
  else
    nextHour = nextHour.addSecs(3600);

  for (; loop < dates.size(); loop++)
  {
    QDateTime date = dates.at(loop);
    
    if (date.date().day() != oldDay.date().day())
    {
      // long tick
      DateTickItem *tick = new DateTickItem;
      tick->type = 1; // long
      tick->text = date.date().toString("MMM d");
      _ticks.insert(loop, tick);
      oldDay = date;
    }
    else
    {
      if (date >= nextHour)
      {
        if (length < OFCTypeBarLength::_MIN30)
        {
          // short tick
          DateTickItem *tick = new DateTickItem;
          tick->type = 0; // short
          tick->text = QString::number(date.time().hour()) + ":00";
          _ticks.insert(loop, tick);
        }
      }
    }
    
    if (date >= nextHour)
    {
      nextHour = date;
      nextHour.setTime(QTime(date.time().hour(), 0, 0, 0));
      
      if (length != OFCTypeBarLength::_MIN1)
        nextHour = nextHour.addSecs(7200);
      else
        nextHour = nextHour.addSecs(3600);
    }
  }
}
*/

/*
void
PlotWidgetDateTick::dailyTicks (QList<QDateTime> &dates)
{
  int loop = 0;
  QDateTime dt = dates.at(loop);
  QDate oldDate = dt.date();
  QDate oldWeek = oldDate;
  oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

  for (; loop < dates.size(); loop++)
  {
    dt = dates.at(loop);
    QDate date = dt.date();

    if (date.month() != oldDate.month())
    {
      // long tick
      DateTickItem *tick = new DateTickItem;
      tick->type = 1;
      tick->text = date.toString("MMM yy");
      _ticks.insert(loop, tick);
      
      oldDate = date;
      oldWeek = date;
      oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
    }
    else
    {
      // if start of new week make a tick
      if (date > oldWeek)
      {
        DateTickItem *tick = new DateTickItem;
        tick->type = 0;
        tick->text = date.toString("d");
        _ticks.insert(loop, tick);
        
        oldWeek = date;
        oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
      }
    }
  }
}
*/

/*
void
PlotWidgetDateTick::weeklyTicks (QList<QDateTime> &dates)
{
  int loop = 0;
  QDateTime dt = dates.at(loop);
  QDate oldMonth = dt.date();

  for (; loop < dates.size(); loop++)
  {
    dt = dates.at(loop);
    QDate date = dt.date();

    if (date.month() != oldMonth.month())
    {
      // short tick
      DateTickItem *tick = new DateTickItem;
      tick->type = 0;
      tick->text = date.toString("M");

      oldMonth = date;
      
      if (date.month() == 1)
      {
        // long tick
        tick->type = 1;
        tick->text = date.toString("yy");
      }
      
      _ticks.insert(loop, tick);
    }
  }
}
*/

/*
void
PlotWidgetDateTick::monthlyTicks (QList<QDateTime> &dates)
{
  int loop = 0;
  QDateTime dt = dates.at(loop);
  QDate oldYear = dt.date();

  for (; loop < dates.size(); loop++)
  {
    dt = dates.at(loop);
    QDate date = dt.date();

    if (date.year() != oldYear.year())
    {
      DateTickItem *tick = new DateTickItem;
      tick->type = 1;
      tick->text = date.toString("yyyy");
      _ticks.insert(loop, tick);
      oldYear = date;
    }
  }
}
*/
