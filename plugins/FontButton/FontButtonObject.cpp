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

#include "FontButtonObject.h"

#include <QtDebug>
#include <QSettings>


FontButtonObject::FontButtonObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("FontButton");
  _type = QString("widget");
  _widget = 0;
  
  _commandList << QString("font");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("set_font");
}

FontButtonObject::~FontButtonObject ()
{
  if (_widget)
  {
    if (! _widget->parent())
      delete _widget;
  }
}

int
FontButtonObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = font(oc);
      break;
    case 1:
      rc = load(oc);
      break;
    case 2:
      rc = save(oc);
      break;
    case 3:
      rc = setFont(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
FontButtonObject::font (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "FontButtonObject::font: invalid widget";
    return 0;
  }
  
  oc->setValue(QString("font"), _widget->font());
  
  return 1;
}

int
FontButtonObject::load (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "FontButtonObject::load: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "FontButtonObject::load: invalid" << key;
    return 0;
  }

  _widget->setFont(settings->value(_name, QFont()).value<QFont>());
  
  return 1;
}

int
FontButtonObject::save (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "FontButtonObject::save: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "FontButtonObject::save: invalid" << key;
    return 0;
  }

  settings->setValue(_name, _widget->font());
  
  return 1;
}

QWidget *
FontButtonObject::widget ()
{
  if (! _widget)
  {
    _widget = new FontButtonWidget(0, QFont());
    connect(_widget, SIGNAL(valueChanged()), this, SLOT(fontChanged()));
  }
  
  return _widget;
}

void
FontButtonObject::fontChanged ()
{
  if (! _widget)
    return;
  
  ObjectCommand toc(QString("font_changed"));
  toc.setValue(QString("font"), _widget->font());
  emit signalMessage(toc);
}

int
FontButtonObject::setFont (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "FontButtonObject::setFont: invalid widget";
    return 0;
  }
  
  _widget->setFont(oc->getFont(QString("font")));
  
  return 1;
}
