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

#include <QtDebug>

#include "ObjectCommand.h"

ObjectCommand::ObjectCommand ()
{
}

ObjectCommand::ObjectCommand (QString d)
{
  _command = d;
}

ObjectCommand::~ObjectCommand ()
{
}

void
ObjectCommand::clear ()
{
  _command.clear();
  _msg.clear();
  clearMap();
  clearValues();
}

void
ObjectCommand::setCommand (QString d)
{
  _command = d;
}

QString
ObjectCommand::command ()
{
  return _command;
}

void
ObjectCommand::setMsg (QString d)
{
  _msg = d;
}

QString
ObjectCommand::msg ()
{
  return _msg;
}

// ********************************
//          map
// *********************************

void
ObjectCommand::clearMap ()
{
  _map.clear();
}

void
ObjectCommand::setMap (QMap<int, Data *> &d)
{
  _map = d;
}

QMap<int, Data *>
ObjectCommand::map ()
{
  return _map;
}

// **************************************
//              values
// **************************************

void
ObjectCommand::clearValues ()
{
  _ints.clear();
  _doubles.clear();
  _strings.clear();
  _lists.clear();
  _objects.clear();
  _colors.clear();
  _datas.clear();
  _dates.clear();
  _bools.clear();
  _icons.clear();
  _fonts.clear();
}

void
ObjectCommand::setValue (QString k, int d)
{
  _ints.insert(k, d);
}

void
ObjectCommand::setValue (QString k, double d)
{
  _doubles.insert(k, d);
}

void
ObjectCommand::setValue (QString k, QString d)
{
  _strings.insert(k, d);
}

void
ObjectCommand::setValue (QString k, QStringList d)
{
  _lists.insert(k, d);
}

void
ObjectCommand::setValue (QString k, void *d)
{
  _objects.insert(k, d);
}

void
ObjectCommand::setValue (QString k, QColor d)
{
  _colors.insert(k, d);
}

void
ObjectCommand::setValue (QString k, Data d)
{
  _datas.insert(k, d);
}

void
ObjectCommand::setValue (QString k, QDateTime d)
{
  _dates.insert(k, d);
}

void
ObjectCommand::setValue (QString k, bool d)
{
  _bools.insert(k, d);
}

void
ObjectCommand::setValue (QString k, QIcon d)
{
  _icons.insert(k, d);
}

void
ObjectCommand::setValue (QString k, QFont d)
{
  _fonts.insert(k, d);
}

int
ObjectCommand::getInt (QString k)
{
  return _ints.value(k);
}

double
ObjectCommand::getDouble (QString k)
{
  return _doubles.value(k);
}

QString
ObjectCommand::getString (QString k)
{
  return _strings.value(k);
}

QStringList
ObjectCommand::getList (QString k)
{
  return _lists.value(k);
}

void *
ObjectCommand::getObject (QString k)
{
  return _objects.value(k);
}

QColor
ObjectCommand::getColor (QString k)
{
  return _colors.value(k);
}

Data
ObjectCommand::getData (QString k)
{
  return _datas.value(k);
}

QDateTime
ObjectCommand::getDate (QString k)
{
  return _dates.value(k);
}

bool
ObjectCommand::getBool (QString k)
{
  return _bools.value(k);
}

QIcon
ObjectCommand::getIcon (QString k)
{
  return _icons.value(k);
}

QFont
ObjectCommand::getFont (QString k)
{
  return _fonts.value(k);
}

void
ObjectCommand::setObjects (QHash<QString, void *> d)
{
  _objects = d;
}

QHash<QString, void *>
ObjectCommand::getObjects ()
{
  return _objects;
}

QHash<QString, Data>
ObjectCommand::getDatas ()
{
  return _datas;
}

void
ObjectCommand::setDatas (QHash<QString, Data> d)
{
  _datas = d;
}

