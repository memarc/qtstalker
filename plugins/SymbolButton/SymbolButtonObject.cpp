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

#include "SymbolButtonObject.h"

#include <QtDebug>
#include <QSettings>


SymbolButtonObject::SymbolButtonObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("SymbolButton");
  _type = QString("widget");
  _widget = 0;
  
  _commandList << QString("symbols");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("set_symbols");
}

SymbolButtonObject::~SymbolButtonObject ()
{
  if (_widget)
  {
    if (! _widget->parent())
      delete _widget;
  }
}

int
SymbolButtonObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = symbols(oc);
      break;
    case 1:
      rc = load(oc);
      break;
    case 2:
      rc = save(oc);
      break;
    case 3:
      rc = setSymbols(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
SymbolButtonObject::symbols (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "SymbolButtonObject::symbols: invalid widget";
    return 0;
  }
  
  oc->setValue(QString("symbols"), _widget->symbols());
  
  return 1;
}

int
SymbolButtonObject::load (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "SymbolButtonObject::load: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "SymbolButtonObject::load: invalid" << key;
    return 0;
  }

  _widget->setSymbols(settings->value(_name).toStringList());
  
  return 1;
}

int
SymbolButtonObject::save (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "SymbolButtonObject::save: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "SymbolButtonObject::save: invalid" << key;
    return 0;
  }

  settings->setValue(_name, _widget->symbols());
  
  return 1;
}

QWidget *
SymbolButtonObject::widget ()
{
  if (! _widget)
  {
    _widget = new SymbolButtonWidget;
    connect(_widget, SIGNAL(signalSymbolsChanged()), this, SLOT(symbolsChanged()));
  }
  
  return _widget;
}

void
SymbolButtonObject::symbolsChanged ()
{
  if (! _widget)
    return;
  
  ObjectCommand toc(QString("symbols_changed"));
  emit signalMessage(toc);
}

int
SymbolButtonObject::setSymbols (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "SymbolButtonObject::setSymbols: invalid widget";
    return 0;
  }
  
  _widget->setSymbols(oc->getList(QString("symbols")));
  
  return 1;
}
