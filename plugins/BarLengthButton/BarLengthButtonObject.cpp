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

#include "BarLengthButtonObject.h"

#include <QtDebug>
#include <QSettings>


BarLengthButtonObject::BarLengthButtonObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("BarLengthButton");
  _type = QString("widget");
  _widget = 0;
  
  _commandList << QString("length");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("list");
  _commandList << QString("interval");
  
  _lengthList << "1M";
  _lengthList << "5M";
  _lengthList << "10M";
  _lengthList << "15M";
  _lengthList << "30M";
  _lengthList << "60M";
  _lengthList << "D";
  _lengthList << "W";
  _lengthList << "M";
}

BarLengthButtonObject::~BarLengthButtonObject ()
{
  if (_widget)
  {
    if (! _widget->parent())
      delete _widget;
  }
}

int
BarLengthButtonObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = length(oc);
      break;
    case 1:
      rc = load(oc);
      break;
    case 2:
      rc = save(oc);
      break;
    case 3:
      rc = list(oc);
      break;
    case 4:
      rc = interval(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
BarLengthButtonObject::length (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "BarLengthButtonObject::length: invalid widget";
    return 0;
  }
  
  oc->setValue(QString("length"), _widget->length());
  
  return 1;
}

int
BarLengthButtonObject::interval (ObjectCommand *oc)
{
  QString key(QString("length"));
  QString length = oc->getString(key);
  if (length.isEmpty())
  {
    qDebug() << "BarLengthButtonObject::interval: invalid" << key;
    return 0;
  }
  
  key = QString("date");
  QDateTime sd = oc->getDate(key);
  if (! sd.isValid())
  {
    qDebug() << "BarLengthButtonObject::interval: invalid" << key;
    return 0;
  }

  QDateTime ed;
  switch (_lengthList.indexOf(length))
  {
    case 0: // 1M
      sd.setTime(QTime(sd.time().hour(), sd.time().minute(), 0, 0));
      ed = sd;
      ed = ed.addSecs(60);
//      ed = ed.addSecs(-1);
      break;
    case 1: // 5M
    {
      int tint = sd.time().minute() / 5;
      sd.setTime(QTime(sd.time().hour(), tint * 5, 0, 0));
      ed = sd;
      ed = ed.addSecs(300);
 //     ed = ed.addSecs(-1);
      break;
    }
    case 2: // 10M
    {
      int tint = sd.time().minute() / 10;
      sd.setTime(QTime(sd.time().hour(), tint * 10, 0, 0));
      ed = sd;
      ed = ed.addSecs(600);
//      ed = ed.addSecs(-1);
      break;
    }
    case 3: // 15M
    {
      int tint = sd.time().minute() / 15;
      sd.setTime(QTime(sd.time().hour(), tint * 15, 0, 0));
      ed = sd;
      ed = ed.addSecs(900);
//      ed = ed.addSecs(-1);
      break;
    }
    case 4: // 30M
    {
      int tint = sd.time().minute() / 30;
      sd.setTime(QTime(sd.time().hour(), tint * 30, 0, 0));
      ed = sd;
      ed = ed.addSecs(1800);
//      ed = ed.addSecs(-1);
      break;
    }
    case 5: // 60M
      sd.setTime(QTime(sd.time().hour(), 0, 0, 0));
      ed = sd;
      ed = ed.addSecs(3600);
//      ed = ed.addSecs(-1);
      break;
    case 6: // D
      sd.setTime(QTime(0, 0, 0, 0));
      ed = sd;
      ed = ed.addDays(1);
//      ed = ed.addSecs(-1);
      break;
    case 7: // W
      sd.setTime(QTime(0, 0, 0, 0));
      sd = sd.addDays(- (sd.date().dayOfWeek() - 1));
      ed = sd;
      ed = ed.addDays(7);
//      ed = ed.addSecs(-1);
      break;
    case 8: // M
      sd.setTime(QTime(0, 0, 0, 0));
      sd = sd.addDays(- (sd.date().day() - 1));
      ed = sd;
//      ed = ed.addSecs(-1);
      break;
    default:
      qDebug() << "BarLengthButtonObject::interval: invalid length" << length;
      return 0;
      break;
  }

  oc->setValue(QString("start_date"), sd);
  oc->setValue(QString("end_date"), ed);

  return 1;
}

int
BarLengthButtonObject::load (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "BarLengthButtonObject::load: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "BarLengthButtonObject::load: invalid" << key;
    return 0;
  }

  _widget->setSettings(_lengthList, settings->value(QString("length"), QString("D")).toString());
  
  return 1;
}

int
BarLengthButtonObject::save (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "BarLengthButtonObject::save: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "BarLengthButtonObject::save: invalid" << key;
    return 0;
  }

  settings->setValue(QString("length"), _widget->length());
  
  return 1;
}

QWidget *
BarLengthButtonObject::widget ()
{
  if (! _widget)
  {
    _widget = new BarLengthButtonWidget(_profile);
    connect(_widget, SIGNAL(signalLength()), this, SLOT(lengthChanged()));
  }
  
  return _widget;
}

void
BarLengthButtonObject::lengthChanged ()
{
  if (! _widget)
    return;
  
  QString key("length_changed");
  ObjectCommand toc(key);
  toc.setValue(key, _widget->length());
  emit signalMessage(toc);
}

int
BarLengthButtonObject::list (ObjectCommand *oc)
{
  oc->setValue(QString("list"), _lengthList);
  return 1;
}
