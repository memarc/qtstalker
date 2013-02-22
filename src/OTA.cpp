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

#include "OTA.h"
#include "Util.h"
#include "../pics/qtstalker.xpm"

#include <QtDebug>


OTA::OTA (QString plugin, QString profile)
{
  setWindowIcon(QIcon(qtstalker_xpm));

  loadApp(plugin, profile);
}

int
OTA::loadApp (QString d, QString pro)
{
  if (d.isEmpty() || pro.isEmpty())
    return 0;

  Util fac;
  Object *o = fac.object(d, pro, QString("main_app"));
  if (! o)
    return 0;

  ObjectCommand oc(QString("app"));
  oc.setValue(QString("main_window"), (void *) this);
  if (! o->message(&oc))
  {
    qDebug() << "OTA::loadApp: object command error" << oc.command();
    delete o;
    return 0;
  }

  delete o;

  return 1;
}
