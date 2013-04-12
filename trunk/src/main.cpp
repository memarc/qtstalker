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

#include <QApplication>
#include <QTranslator>
#include <QString>
#include <QLocale>
#include <QtDebug>

#include "Util.h"
#include "OTA.h"
#include "ObjectCommand.h"

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    qDebug() << "OTA: Usage: ota <-a app> <-p profile>";
    return 1;
  }
  
  QString plugin;
  QString profile;
  int loop;
  for (loop = 1; loop <= argc; loop++)
  {
    QString s = argv[loop];
    if (s == "-a")
    {
      loop++;
      if (loop > argc)
        break;
      
      plugin = argv[loop];
      continue;
    }
    
    if (s == "-p")
    {
      loop++;
      if (loop > argc)
        break;
      
      profile = argv[loop];
      continue;
    }
  }
  
  if (plugin.isEmpty())
  {
    qDebug() << "OTA: Usage: ota <-a app> <-p profile>";
    return 1;
  }
  
  if (profile.isEmpty())
    profile = QString("default");

  Util util;
  Plugin *plug = util.plugin(plugin);
  if (! plug)
  {
    qDebug() << "OTA: plugin error" << plugin;
    return 1;
  }
  
  PluginCommand pc;
  pc.command = QString("type");
  if (! plug->command(&pc))
  {
    qDebug() << "OTA: plugin command error" << pc.command;
    return 1;
  }
  
  if (pc.type != QString("app"))
  {
    qDebug() << "OTA: Invalid app" << plugin;
    return 1;
  }
  
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("OTA");
  QCoreApplication::setApplicationName("OTA");
  QTranslator tor(0);

  QString i18nDir = QString("%1/OTA/i18n").arg(QDir::homePath());
  QString i18nFilename = QString("ota_%1").arg(QLocale::system().name());
  tor.load(i18nFilename, i18nDir);
  a.installTranslator( &tor );

  qRegisterMetaType<ObjectCommand>("ObjectCommand");

  OTA app(plugin, profile);
  app.show();
  return a.exec();
}
