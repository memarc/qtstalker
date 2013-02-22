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
#include "GroupDialog.h"
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
  
  load();
}

GroupObject::~GroupObject ()
{
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
  
  QStringList symbols = settings.value(_name).toStringList();
  if (! symbols.size())
    return 1;
  
  Util util;
  Object *db = util.object(QString("Symbol"), QString(), QString());
  if (! db)
  {
    qDebug() << "GroupObject::load: invalid Symbol";
    return 0;
  }
  
  ObjectCommand toc(QString("info"));
  toc.setValue(QString("names"), symbols);
  if (! db->message(&toc))
  {
    qDebug() << "GroupObject::load: message error" << db->plugin() << toc.command();
    delete db;
    return 0;
  }
  else
    delete db;

  _symbols = toc.getDatas();

  return 1;
}

int
GroupObject::save ()
{
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_file, QSettings::NativeFormat);
  
  QStringList tl;
  QHashIterator<QString, Data> it(_symbols);
  while (it.hasNext())
  {
    it.next();
    tl << it.key();
  }
  
  settings.setValue(_name, tl);
  
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
  
  GroupDialog *d = new GroupDialog(_name);
  d->setSettings(_symbols);
  connect(d, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  d->setModified(FALSE);
  d->show();
  return 1;
}

void
GroupObject::dialogDone (void *dialog)
{
  GroupDialog *d = (GroupDialog *) dialog;
  d->settings(_symbols);
  save();
  
  ObjectCommand toc(QString("modified"));
  toc.setValue(QString("group"), _name);
  toc.setDatas(_symbols);
  emit signalMessage(toc);
}

int
GroupObject::symbols (ObjectCommand *oc)
{
  oc->setDatas(_symbols);
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
  QStringList tl = oc->getList(key);

  Util util;
  Object *db = util.object(QString("Symbol"), QString(), QString());
  if (! db)
  {
    qDebug() << "GroupObject::add: invalid Symbol object";
    return 0;
  }
  
  ObjectCommand toc(QString("info"));
  toc.setValue(QString("names"), tl);
  if (! db->message(&toc))
  {
    qDebug() << "GroupObject::load: message error" << db->plugin() << toc.command();
    delete db;
    return 0;
  }
  else
    delete db;

  QHashIterator<QString, Data> it(toc.getDatas());
  while (it.hasNext())
  {
    it.next();
    _symbols.insert(it.key(), it.value());
  }
  
  save();
  
  return 1;
}
