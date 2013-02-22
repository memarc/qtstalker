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


#include "ScannerObject.h"
#include "ScannerWidget.h"
#include "Util.h"

#include <QDebug>
#include <QMainWindow>


ScannerObject::ScannerObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("Scanner");
  _type = QString("app");

  _commandList << QString("app");
}

int
ScannerObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0: // app
      rc = app(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
ScannerObject::app (ObjectCommand *oc)
{
  QMainWindow *mw = (QMainWindow *) oc->getObject(QString("main_window"));
  if (! mw)
  {
    qDebug() << "ScannerObject::app: invalid main_window";
    return 0;
  }

  new ScannerWidget(mw, _profile);
  return 1;
}
