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

#include "ColorButtonObject.h"

#include <QtDebug>
#include <QSettings>


ColorButtonObject::ColorButtonObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("ColorButton");
  _type = QString("widget");
  _widget = 0;
  
  _commandList << QString("color");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("set_color");
}

ColorButtonObject::~ColorButtonObject ()
{
  if (_widget)
  {
    if (! _widget->parent())
      delete _widget;
  }
}

int
ColorButtonObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = color(oc);
      break;
    case 1:
      rc = load(oc);
      break;
    case 2:
      rc = save(oc);
      break;
    case 3:
      rc = setColor(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
ColorButtonObject::color (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "ColorButtonObject::color: invalid widget";
    return 0;
  }
  
  oc->setValue(QString("color"), _widget->color());
  
  return 1;
}

int
ColorButtonObject::load (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "ColorButtonObject::load: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "ColorButtonObject::load: invalid" << key;
    return 0;
  }

  _widget->setColor(settings->value(_name, QColor(Qt::red)).value<QColor>());
  
  return 1;
}

int
ColorButtonObject::save (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "ColorButtonObject::save: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "ColorButtonObject::save: invalid" << key;
    return 0;
  }

  settings->setValue(_name, _widget->color());
  
  return 1;
}

QWidget *
ColorButtonObject::widget ()
{
  if (! _widget)
  {
    _widget = new ColorButtonWidget(0, QColor(Qt::red));
    connect(_widget, SIGNAL(valueChanged()), this, SLOT(colorChanged()));
  }
  
  return _widget;
}

void
ColorButtonObject::colorChanged ()
{
  if (! _widget)
    return;
  
  ObjectCommand toc(QString("color_changed"));
  toc.setValue(QString("color"), _widget->color());
  emit signalMessage(toc);
}

int
ColorButtonObject::setColor (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "ColorButtonObject::setColor: invalid widget";
    return 0;
  }
  
  QColor color = oc->getColor(QString("color"));
  if (! color.isValid())
  {
    qDebug() << "ColorButtonObject::setColor: invalid color";
    return 0;
  }
  
  _widget->setColor(color);
  
  return 1;
}
