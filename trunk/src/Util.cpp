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

#include "Util.h"

#include <QtDebug>
#include <QPluginLoader>
#include <QDir>

Util::Util ()
{
}

Plugin *
Util::plugin (QString name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("lib") << QString("lib");

  QString s = tl.join("/") + name;
#if defined(Q_OS_MAC)  
  s.append(".dylib");
#else
  s.append(".so");
#endif
  
  QPluginLoader pluginLoader(s);
  QObject *tp = pluginLoader.instance();
  if (! tp)
  {
    qDebug() << "Util::plugin:" << pluginLoader.errorString();
    return 0;
  }

  Plugin *plug = qobject_cast<Plugin *>(tp);
  if (! plug)
    qDebug() << "Util::plugin: error casting Plugin";

  return plug;
}

Object *
Util::object (QString d, QString profile, QString name)
{
  Plugin *plug = plugin(d);
  if (! plug)
  {
    qDebug() << "Util::object: plugin error" << d;
    return 0;
  }

  PluginCommand pc;
  pc.command = QString("object");
  pc.profile = profile;
  pc.name = name;
  pc.object = 0;
  if (! plug->command(&pc))
  {
    qDebug() << "Util::load: plugin command error" << pc.command;
    return 0;
  }

  return pc.object;
}

QStringList
Util::plugins (QString type)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("lib");
  dir = QDir(tl.join("/"));

  tl.clear();
  foreach (QString fileName, dir.entryList(QDir::Files))
  {
    QString name = fileName.right(fileName.length() - 3);
    name.truncate(name.lastIndexOf(".", -1));

    Plugin *plug = plugin(name);
    if (! plug)
      continue;
    
    PluginCommand pc;
    pc.command = QString("type");
    if (! plug->command(&pc))
      continue;

    if (pc.type == type)
      tl << name;
  }     

  return tl;
}

void
Util::strip (double d, int p, QString &s)
{
  QString ts = QString::number(d, 'f', p);
  
  while (ts.endsWith("0"))
    ts.chop(1);
  
  while (ts.endsWith("."))
    ts.chop(1);
  
  s = ts;
}
