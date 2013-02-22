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

#include "CommandParse.h"

#include <QtDebug>

CommandParse::CommandParse ()
{
  _verb = -1;
}

int
CommandParse::parse (QString d)
{
  // object.new(name); - create object
  // object.run(); - execute object
  // object.prop.prop.prop(); - get value
  // object.prop.prop.prop(value); - set value
  
  _command = d;
  
  // remove outside brackets
  int ti = _command.indexOf(QString("("), 0, Qt::CaseSensitive);
  if (ti == -1)
  {
//    qDebug() << "CommandParse::parse: invalid ( bracketing" << d;
    return 0;
  }
  QString tprops = d.left(ti);
  tprops.trimmed();
//qDebug() << "CommandParse::parse: props" << props;

  int ti2 = _command.lastIndexOf(QString(")"), -1, Qt::CaseSensitive);
  if (ti2 == -1)
  {
//    qDebug() << "CommandParse::parse: invalid ) bracketing" << d;
    return 0;
  }
  _value = d.mid(ti + 1, ti2 - ti - 1);
  _value.trimmed();
//qDebug() << "CommandParse::parse: values" << values;
  
  // props
  QStringList tl = tprops.split(".", QString::SkipEmptyParts);
  if (tl.size() < 2)
  {
//    qDebug() << "CommandParse::parse: invalid format" << d;
    return 0;
  }
  
  // store object name
  _object = tl.at(0);
  
  int loop = 1;
  for (; loop < tl.size(); loop++)
    _props.insert(loop - 1, tl.at(loop));

  // figure out verb type
  if (prop(0) == "new")
  {
    if (_value.isEmpty())
      return 0;
    
    _verb = _CREATE;
  }
  else
  {
    if (prop(0) == "run")
      _verb = _RUN;
    else
    {
      if (_value.isEmpty())
        _verb = _GET;
      else
      {
        if (_value.isEmpty())
          return 0;
        
        _verb = _SET;
      }
    }
  }
  
  return 1;
}

QString
CommandParse::value ()
{
  return _value;
}

int
CommandParse::props ()
{
  return _props.count();
}

QString
CommandParse::prop (int d)
{
  return _props.value(d);
}

QString
CommandParse::object ()
{
  return _object;
}

QString
CommandParse::command ()
{
  return _command;
}

int
CommandParse::verb ()
{
  return _verb;
}
