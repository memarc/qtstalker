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

#include "IndicatorInputObject.h"

#include <QtDebug>
#include <QSettings>


IndicatorInputObject::IndicatorInputObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("IndicatorInput");
  _type = QString("widget");
  _widget = 0;
  
  _commandList << QString("set_objects");
  _commandList << QString("set_input");
  _commandList << QString("input");
  _commandList << QString("set_key");
  _commandList << QString("key");
  _commandList << QString("set_offset");
  _commandList << QString("offset");
  _commandList << QString("set_enabled");
}

IndicatorInputObject::~IndicatorInputObject ()
{
  if (_widget)
  {
    if (! _widget->parent())
      delete _widget;
  }
}

int
IndicatorInputObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = setObjects(oc);
      break;
    case 1:
      rc = setInput(oc);
      break;
    case 2:
      rc = input(oc);
      break;
    case 3:
      rc = setKey(oc);
      break;
    case 4:
      rc = key(oc);
      break;
    case 5:
      rc = setOffset(oc);
      break;
    case 6:
      rc = offset(oc);
      break;
    case 7:
      rc = setEnabled(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
IndicatorInputObject::setObjects (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "IndicatorInputObject::setObjects: invalid widget";
    return 0;
  }
  
  _widget->setObjects(oc->getObjects());
  
  return 1;
}

int
IndicatorInputObject::setInput (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "IndicatorInputObject::setInput: invalid widget";
    return 0;
  }
  
  QString key("input");
  QString input = oc->getString(key);
  if (input.isEmpty())
  {
    qDebug() << "IndicatorInputObject::setInput: invalid" << key;
    return 0;
  }
  
  _widget->setInput(input);
  
  return 1;
}

int
IndicatorInputObject::input (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "IndicatorInputObject::input: invalid widget";
    return 0;
  }

  oc->setValue(QString("input"), _widget->input());
  
  return 1;
}

int
IndicatorInputObject::setKey (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "IndicatorInputObject::setKey: invalid widget";
    return 0;
  }
  
  QString key("key");
  QString k = oc->getString(key);
  if (k.isEmpty())
  {
    qDebug() << "IndicatorInputObject::setKey: invalid" << key;
    return 0;
  }
  
  key = QString("data");
  QString d = oc->getString(key);
  if (d.isEmpty())
  {
    qDebug() << "IndicatorInputObject::setKey: invalid" << key;
    return 0;
  }
  
  _widget->setKey(k, d);
  
  return 1;
}

int
IndicatorInputObject::key (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "IndicatorInputObject::key: invalid widget";
    return 0;
  }
  
  QString key("key");
  QString k = oc->getString(key);
  if (k.isEmpty())
  {
    qDebug() << "IndicatorInputObject::setKey: invalid" << key;
    return 0;
  }

  oc->setValue(QString("data"), _widget->key(k));
  
  return 1;
}

int
IndicatorInputObject::setOffset (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "IndicatorInputObject::setOffset: invalid widget";
    return 0;
  }
  
  QString key("offset");
  int offset = oc->getInt(key);
  
  _widget->setOffset(offset);
  
  return 1;
}

int
IndicatorInputObject::offset (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "IndicatorInputObject::offset: invalid widget";
    return 0;
  }
  
  oc->setValue(QString("offset"), _widget->offset());
  
  return 1;
}

QWidget *
IndicatorInputObject::widget ()
{
  if (! _widget)
  {
    _widget = new IndicatorInputWidget;
    connect(_widget, SIGNAL(valueChanged()), this, SLOT(inputChanged()));
  }
  
  return _widget;
}

void
IndicatorInputObject::inputChanged ()
{
  if (! _widget)
    return;
  
  ObjectCommand toc(QString("input_changed"));
  emit signalMessage(toc);
}

int
IndicatorInputObject::setEnabled (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "IndicatorInputObject::setEnabled: invalid widget";
    return 0;
  }
  
  _widget->setEnabled(oc->getBool(QString("status")));
  
  return 1;
}
