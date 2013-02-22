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

#include "DataBaseObject.h"
#include "Util.h"

DataBaseObject::DataBaseObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("DataBase");
  _type = QString("system");

  _commandList << QString("transaction");
  _commandList << QString("commit");
  _commandList << QString("get");
  _commandList << QString("set");
  _commandList << QString("names");
  _commandList << QString("remove");
  _commandList << QString("search");
  _commandList << QString("get_range");
  _commandList << QString("drop");
  _commandList << QString("open");
}

int
DataBaseObject::message (ObjectCommand *command)
{
  int rc = 0;

  switch (_commandList.indexOf(command->command()))
  {
    case 0:
      rc = transaction();
      break;
    case 1:
      rc = commit();
      break;
    case 2:
      rc = get(command);
      break;
    case 3:
      rc = set(command);
      break;
    case 4:
      rc = names(command);
      break;
    case 5:
      rc = remove(command);
      break;
    case 6:
      rc = search(command);
      break;
    case 7:
      rc = getRange(command);
      break;
    case 8:
      rc = drop(command);
      break;
    case 9:
      rc = open(command);
      break;
    default:
      break;
  }
  
  return rc;
}

int
DataBaseObject::open (ObjectCommand *oc)
{
  QString key("path");
  QString path = oc->getString(key);
  if (path.isEmpty())
  {
    qDebug() << "DataBaseObject::open: invalid" << key;
    return 0;
  }
  
  key = QString("file");
  QString file = oc->getString(key);
  if (file.isEmpty())
  {
    qDebug() << "DataBaseObject::open: invalid" << key;
    return 0;
  }
  
  QDir dir;
  if (! dir.mkpath(path))
  {
    qDebug() << "DataBaseObject::open: path create error" << path;
    return 0;
  }

  QString dbfile = path + "/" + file;
  
  _db = QSqlDatabase::database(dbfile);
  if (! _db.isOpen())
  {
    _db = QSqlDatabase::addDatabase("QSQLITE", dbfile);
    _db.setHostName("OTA");
    _db.setDatabaseName(dbfile);
    _db.setUserName("OTA");
    _db.setPassword("OTA");
    if (! _db.open())
    {
      qDebug() << "DataBaseObject::open:" << _db.lastError().text();
      return 0;
    }
  }
  
  return 1;
}

int
DataBaseObject::createTable (QString table)
{
  if (table.isEmpty())
  {
    qDebug() << "DataBaseObject::createTable: invalid table";
    return 0;
  }

  if (! _db.isOpen())
  {
    qDebug() << "DataBaseObject::createTable: db not open";
    return 0;
  }
  
  // EAV structure for data
  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS " + table + " (";
  s.append("a INTEGER PRIMARY KEY"); // auto increment index
  s.append(", name TEXT"); // entity name
  s.append(", key TEXT"); // attribute
  s.append(", value TEXT"); // attribute value
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBaseObject::createTable: " << q.lastError().text();
    return 0;
  }
  
  return 1;
}

int
DataBaseObject::transaction ()
{
  if (! _db.isOpen())
  {
    qDebug() << "DataBaseObject::transaction: db not open";
    return 0;
  }
  
  _db.transaction();
  
  return 1;
}

int
DataBaseObject::commit ()
{
  if (! _db.isOpen())
  {
    qDebug() << "DataBaseObject::commit: db not open";
    return 0;
  }
  
  _db.commit();
  
  return 1;
}

int
DataBaseObject::get (ObjectCommand *oc)
{
  if (! _db.isOpen())
  {
    qDebug() << "DataBaseObject::get: db not open";
    return 0;
  }

  QString key("table");
  QString table = oc->getString(key);
  if (table.isEmpty())
  {
    qDebug() << "DataBaseObject::get: invalid" << key;
    return 0;
  }

  key = QString("names");
  QStringList names = oc->getList(key);
  
  QSqlQuery q(_db);

  for (int pos = 0; pos < names.size(); pos++)
  {
    QString s = "SELECT key,value FROM " + table + " WHERE name='" + names.at(pos) + "'";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBaseObject::get: " << q.lastError().text();
      continue;
    }

    Data data;
    while (q.next())
      data.insert(q.value(0).toString(), q.value(1));
  
    if (data.size())
      oc->setValue(names.at(pos), data);
  }

  return 1;
}

int
DataBaseObject::set (ObjectCommand *oc)
{
  if (! _db.isOpen())
  {
    qDebug() << "DataBaseObject::set: db not open";
    return 0;
  }

  QString key("table");
  QString table = oc->getString(key);
  if (table.isEmpty())
  {
    qDebug() << "DataBaseObject::set: invalid" << key;
    return 0;
  }

  if (! createTable(table))
    return 0;
  
  QSqlQuery q(_db);
  
  QHashIterator<QString, Data> it(oc->getDatas());
  while (it.hasNext())
  {
    it.next();
    Data input = it.value();
    
    // remove old records if any
    if (! remove2(table, it.key()))
    {
      qDebug() << "DataBaseObject::set: error during remove";
      return 0;
    }
    
    // save all the settings
    QHashIterator<QString, QVariant> it2(input);
    while (it2.hasNext())
    {
      it2.next();
      
      QString s = "INSERT INTO " + table + " VALUES (";
      s.append("NULL"); // auto increment
      s.append(",'" + it.key() + "'");
      s.append(",'" + it2.key() + "'");
      s.append(",'" + it2.value().toString() + "'");
      s.append(")");
      q.exec(s);
      if (q.lastError().isValid())
      {
        qDebug() << "DataBaseObject::set: " << q.lastError().text();
        qDebug() << s;
        return 0;
      }
    }
  }

  return 1;
}

int
DataBaseObject::remove (ObjectCommand *oc)
{
  if (! _db.isOpen())
  {
    qDebug() << "DataBaseObject::remove: db not open";
    return 0;
  }

  QString key("table");
  QString table = oc->getString(key);
  if (table.isEmpty())
  {
    qDebug() << "DataBaseObject::remove: invalid" << key;
    return 0;
  }

  key = QString("names");
  QStringList names = oc->getList(key);

  for (int pos = 0; pos < names.size(); pos++)
  {
    if (! remove2(table, names.at(pos)))
    {
      qDebug() << "DataBaseObject::remove: error";
      return 0;
    }
  }

  return 1;
}

int
DataBaseObject::remove2 (QString table, QString name)
{
  QSqlQuery q(_db);
  QString s = "DELETE FROM " + table + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBaseObject::remove2: " << q.lastError().text();
    qDebug() << s;
    return 0;
  }

  return 1;
}

int
DataBaseObject::names (ObjectCommand *oc)
{
  if (! _db.isOpen())
  {
    qDebug() << "DataBaseObject::names: db not open";
    return 0;
  }

  QString key("table");
  QString table = oc->getString(key);
  if (table.isEmpty())
  {
    qDebug() << "DataBaseObject::names: invalid" << key;
    return 0;
  }
  
  QSqlQuery q(_db);

  QString s = "SELECT DISTINCT name FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBaseObject::names: " << q.lastError().text();
    return 0;
  }

  QStringList tl;
  while (q.next())
    tl << q.value(0).toString();
  tl.sort();

  oc->setValue(QString("list"), tl);

  return 1;
}

int
DataBaseObject::search (ObjectCommand *oc)
{
  if (! _db.isOpen())
  {
    qDebug() << "DataBaseObject::search: db not open";
    return 0;
  }
  
  Data search = oc->getData(QString("search"));
  if (! search.size())
  {
    qDebug() << "DataBaseObject::search: invalid search";
    return 0;
  }

  QString key("table");
  QString table = oc->getString(key);
  if (table.isEmpty())
  {
    qDebug() << "DataBaseObject::search: invalid" << key;
    return 0;
  }
  
  int count = 0;
  Data results;
  QSqlQuery q(_db);
  QHashIterator<QString, QVariant> it(search);
  while (it.hasNext())
  {
    it.next();
    QVariant v = it.value();
    
    QString s = "SELECT DISTINCT name FROM " + table + " WHERE";
    
    if (v.toString().contains("%"))
      s.append(" key='" + it.key() + "' AND value LIKE '" + v.toString() + "'");
    else
      s.append(" key='" + it.key() + "' AND value='" + v.toString() + "'");
    
    s.append(" ORDER BY name ASC");

    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBaseObject::search: " << q.lastError().text();
      qDebug() << s;
      return 0;
    }

    count++;
    
    while (q.next())
    {
      int ti = results.value(q.value(0).toString()).toInt();
      ti++;
      results.insert(q.value(0).toString(), QVariant(ti));
    }
  }

  QStringList names;
  QHashIterator<QString, QVariant> it2(results);
  while (it2.hasNext())
  {
    it2.next();
    QVariant v = it2.value();
    if (v.toInt() != count)
      continue;
    
    names << it2.key();
  }
  names.sort();

  oc->setValue(QString("list"), names);
  
  return 1;
}

int
DataBaseObject::getRange (ObjectCommand *oc)
{
  if (! _db.isOpen())
  {
    qDebug() << "DataBaseObject::getAll: db not open";
    return 0;
  }

  QString key("table");
  QString table = oc->getString(key);
  if (table.isEmpty())
  {
    qDebug() << "DataBaseObject::getRange: invalid" << key;
    return 0;
  }

  key = QString("range_start");
  QString startRange = oc->getString(key);
  
  key = QString("range_end");
  QString endRange = oc->getString(key);
  
  QString s = "SELECT name,key,value FROM " + table;
  
  if (! startRange.isEmpty() && ! endRange.isEmpty())
  {
    s.append(" WHERE name >=" + startRange);
    s.append(" AND name <=" + endRange);
  }
  
  s.append(" ORDER BY name ASC");
  
  QSqlQuery q(_db);
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBaseObject::getAll: " << q.lastError().text();
    return 0;
  }

  oc->clearValues();
  
  QString name, ts;
  Data data;
  while (q.next())
  {
    ts = q.value(0).toString();
    if (ts != name)
    {
      if (data.size())
        oc->setValue(ts, data);
      data.clear();
      name = ts;
    }
      
    data.insert(q.value(1).toString(), q.value(2));
  }
  
  if (data.size())
    oc->setValue(ts, data);
  
  return 1;
}

int
DataBaseObject::drop (ObjectCommand *oc)
{
  if (! _db.isOpen())
  {
    qDebug() << "DataBaseObject::drop: db not open";
    return 0;
  }

  QString key("table");
  QString table = oc->getString(key);
  if (table.isEmpty())
  {
    qDebug() << "DataBaseObject::drop: invalid" << key;
    return 0;
  }
  
  QSqlQuery q(_db);
  QString s = "DROP TABLE " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBaseObject::drop: " << q.lastError().text();
    qDebug() << s;
    return 0;
  }

  return 1;
}
