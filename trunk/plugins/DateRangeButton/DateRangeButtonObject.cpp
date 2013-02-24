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

#include "DateRangeButtonObject.h"

#include <QtDebug>
#include <QSettings>


DateRangeButtonObject::DateRangeButtonObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("DateRangeButton");
  _type = QString("widget");
  _widget = 0;
  
  _commandList << QString("dates");
  _commandList << QString("load");
  _commandList << QString("save");
  
  _rangeList << tr("*");
  _rangeList << tr("1D");
  _rangeList << tr("1W");
  _rangeList << tr("1M");
  _rangeList << tr("3M");
  _rangeList << tr("6M");
  _rangeList << tr("1Y");
  _rangeList << tr("2Y");
  _rangeList << tr("5Y");
  _rangeList << tr("10Y");
  _rangeList << tr("25Y");
  _rangeList << tr("50Y");
}

DateRangeButtonObject::~DateRangeButtonObject ()
{
  if (_widget)
  {
    if (! _widget->parent())
      delete _widget;
  }
}

int
DateRangeButtonObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = dates(oc);
      break;
    case 1:
      rc = load(oc);
      break;
    case 2:
      rc = save(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
DateRangeButtonObject::dates (ObjectCommand *oc)
{
  QDateTime sd, ed;
  if (_widget)
  {
    if (_widget->custom())
    {
      sd = _widget->startDate();
      ed = _widget->endDate();
    }
    else
    {
      if (! datesLocal(_widget->range(), sd, ed))
      {
        qDebug() << "DateRangeButtonObject::dates: invalid range";
        return 0;
      }
    }
  }
  else
  {
    QString key("range");
    QString range = oc->getString(key);
    if (range.isEmpty())
    {
      qDebug() << "DateRangeButtonObject::dates: invalid" << key;
      return 0;
    }
    
    if (! datesLocal(range, sd, ed))
    {
      qDebug() << "DateRangeButtonObject::dates: invalid range" << range;
      return 0;
    }
  }
  
  oc->setValue(QString("start_date"), sd);
  oc->setValue(QString("end_date"), ed);
  
  return 1;
}

int
DateRangeButtonObject::datesLocal (QString range, QDateTime &sd, QDateTime &ed)
{
  switch (_rangeList.indexOf(range))
  {
    case 0: // all
      sd = QDateTime::fromString("1900-01-01 00:00:00", Qt::ISODate);
      break;
    case 1: // 1D
      sd = QDateTime::currentDateTime().addDays(-1);
      break;
    case 2: // 1W
      sd = QDateTime::currentDateTime().addDays(-7);
      break;
    case 3: // 1M
      sd = QDateTime::currentDateTime().addMonths(-1);
      break;
    case 4: // 3M
      sd = QDateTime::currentDateTime().addMonths(-3);
      break;
    case 5: // 6M
      sd = QDateTime::currentDateTime().addMonths(-6);
      break;
    case 6: // 1Y
      sd = QDateTime::currentDateTime().addYears(-1);
      break;
    case 7: // 2Y
      sd = QDateTime::currentDateTime().addYears(-2);
      break;
    case 8: // 5Y
      sd = QDateTime::currentDateTime().addYears(-5);
      break;
    case 9: // 10Y
      sd = QDateTime::currentDateTime().addYears(-10);
      break;
    case 10: // 25Y
      sd = QDateTime::currentDateTime().addYears(-25);
      break;
    case 11: // 50Y
      sd = QDateTime::currentDateTime().addYears(-50);
      break;
    default:
      return 0;
      break;
  }

  ed = QDateTime::currentDateTime();
  
  return 1;
}

int
DateRangeButtonObject::load (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "DateRangeButtonObject::load: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "DateRangeButtonObject::load: invalid" << key;
    return 0;
  }

  _widget->setSettings(_rangeList,
                       settings->value(QString("range"), QString("1Y")).toString(),
                       settings->value(QString("start_date"), QDateTime::currentDateTime()).toDateTime(),
                       settings->value(QString("end_date"), QDateTime::currentDateTime()).toDateTime(),
                       settings->value(QString("date"), FALSE).toBool());
  
  return 1;
}

int
DateRangeButtonObject::save (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "DateRangeButtonObject::save: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "DateRangeButtonObject::save: invalid" << key;
    return 0;
  }

  settings->setValue(QString("range"), _widget->range());
  settings->setValue(QString("start_date"), _widget->startDate());
  settings->setValue(QString("end_date"), _widget->endDate());
  settings->setValue(QString("date"), _widget->custom());
  
  return 1;
}

QWidget *
DateRangeButtonObject::widget ()
{
  if (! _widget)
  {
    _widget = new DateRangeButtonWidget(_profile);
    connect(_widget, SIGNAL(signalRange()), this, SLOT(rangeChanged()));
  }
  
  return _widget;
}

void
DateRangeButtonObject::rangeChanged ()
{
  if (! _widget)
    return;
  
  QString key("range_changed");
  ObjectCommand toc(key);
  toc.setValue(key, _widget->range());
  
  if (_widget->custom())
  {
    toc.setValue(QString("start_date"), _widget->startDate());
    toc.setValue(QString("end_date"), _widget->endDate());
  }
  else
  {
    QDateTime sd, ed;
    datesLocal(_widget->range(), sd, ed);
    toc.setValue(QString("start_date"), sd);
    toc.setValue(QString("end_date"), ed);
  }  

  emit signalMessage(toc);
}
