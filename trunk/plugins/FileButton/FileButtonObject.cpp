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

#include "FileButtonObject.h"

#include <QtDebug>
#include <QSettings>


FileButtonObject::FileButtonObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("FileButton");
  _type = QString("widget");
  _widget = 0;
  
  _commandList << QString("files");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("set_files");
}

FileButtonObject::~FileButtonObject ()
{
  if (_widget)
  {
    if (! _widget->parent())
      delete _widget;
  }
}

int
FileButtonObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = files(oc);
      break;
    case 1:
      rc = load(oc);
      break;
    case 2:
      rc = save(oc);
      break;
    case 3:
      rc = setFiles(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
FileButtonObject::files (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "FileButtonObject::files: invalid widget";
    return 0;
  }
  
  oc->setValue(QString("files"), _widget->files());
  
  return 1;
}

int
FileButtonObject::load (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "FileButtonObject::load: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "FileButtonObject::load: invalid" << key;
    return 0;
  }

  _widget->setFiles(settings->value(_name).toStringList());
  
  return 1;
}

int
FileButtonObject::save (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "FileButtonObject::save: widget not active";
    return 0;
  }
  
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "FileButtonObject::save: invalid" << key;
    return 0;
  }

  settings->setValue(_name, _widget->files());
  
  return 1;
}

QWidget *
FileButtonObject::widget ()
{
  if (! _widget)
  {
    _widget = new FileButtonWidget(0);
    connect(_widget, SIGNAL(signalSelectionChanged()), this, SLOT(fileChanged()));
  }
  
  return _widget;
}

void
FileButtonObject::fileChanged ()
{
  if (! _widget)
    return;
  
  ObjectCommand toc(QString("files_changed"));
  toc.setValue(QString("files"), _widget->files());
  emit signalMessage(toc);
}

int
FileButtonObject::setFiles (ObjectCommand *oc)
{
  if (! _widget)
  {
    qDebug() << "FileButtonObject::setFiles: invalid widget";
    return 0;
  }
  
  _widget->setFiles(oc->getList(QString("files")));
  
  return 1;
}
