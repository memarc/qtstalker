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

#include "GroupObject.h"
#include "Util.h"

#include <QtDebug>


GroupObject::GroupObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("Group");
  _type = QString("system");
  
  _commandList << QString("groups");
  _commandList << QString("dialog");
  _commandList << QString("remove");
  _commandList << QString("symbols");
  _commandList << QString("add");
  
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Group") << QString("db") << QString("groups");
  _file = tl.join("/");

  Util util;
  _dialog = util.object(QString("SymbolSelect"), QString(), QString());
  if (_dialog)
    connect(_dialog, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(dialogMessage(ObjectCommand)));
  
  load();
}

GroupObject::~GroupObject ()
{
  if (_dialog)
    delete _dialog;
}

int
GroupObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = groups(oc);
      break;
    case 1:
      rc = dialog(oc);
      break;
    case 2:
      rc = remove(oc);
      break;
    case 3:
      rc = symbols(oc);
      break;
    case 4:
      rc = add(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
GroupObject::load ()
{
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_file, QSettings::NativeFormat);
  _symbols = settings.value(_name).toStringList();
  
  return 1;
}

int
GroupObject::save ()
{
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_file, QSettings::NativeFormat);
  settings.setValue(_name, _symbols);
  settings.sync();

  return 1;
}

int
GroupObject::groups (ObjectCommand *oc)
{
  QSettings settings(_file, QSettings::NativeFormat);
  oc->setValue(QString("groups"), settings.childKeys());
  return 1;
}

int
GroupObject::remove (ObjectCommand *)
{
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_file, QSettings::NativeFormat);
  settings.remove(_name);
  settings.sync();

  _name.clear();
  
  return 1;
}

int
GroupObject::dialog (ObjectCommand *)
{
  if (_name.isEmpty())
    return 0;

  if (! _dialog)
    return 0;
  
  ObjectCommand toc(QString("settings"));
  _dialog->message(&toc);
  
  toc.setValue(QString("symbols"), _symbols);
  
  toc.setCommand(QString("set_settings"));
  _dialog->message(&toc);
  
  toc.setCommand(QString("dialog"));
  _dialog->message(&toc);

  return 1;
}

void
GroupObject::dialogMessage (ObjectCommand oc)
{
  if (oc.command() != QString("symbols_changed"))
    return;
  
  ObjectCommand toc(QString("settings"));
  _dialog->message(&toc);
  
  _symbols = toc.getList(QString("symbols"));
  
  save();
  
  ObjectCommand moc(QString("modified"));
  moc.setValue(QString("group"), _name);
  moc.setValue(QString("symbols"), _symbols);
  emit signalMessage(moc);
}

int
GroupObject::symbols (ObjectCommand *oc)
{
  oc->setValue(QString("symbols"), _symbols);
  return 1;
}

int
GroupObject::add (ObjectCommand *oc)
{
  QString key("group");
  QString group = oc->getString(key);
  if (group.isEmpty())
  {
    qDebug() << "GroupObject::add: invalid" << key;
    return 0;
  }
  _name = group;
  
  load();
  
  key = QString("list");
  _symbols << oc->getList(key);
  _symbols.removeDuplicates();

  save();
  
  return 1;
}
