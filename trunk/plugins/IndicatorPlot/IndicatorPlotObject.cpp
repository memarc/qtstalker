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

#include "IndicatorPlotObject.h"
#include "IndicatorPlotWidget.h"
#include "Util.h"

#include <QDebug>
#include <QMainWindow>


IndicatorPlotObject::IndicatorPlotObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("IndicatorPlot");
  _type = QString("app");

  _commandList << QString("app");
}

int
IndicatorPlotObject::message (ObjectCommand *pc)
{
  int rc = 0;

  switch (_commandList.indexOf(pc->command()))
  {
    case 0: // app
      rc = app(pc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
IndicatorPlotObject::app (ObjectCommand *oc)
{
  QString key("main_window");
  QMainWindow *mw = (QMainWindow *) oc->getObject(key);
  if (! mw)
  {
    qDebug() << "IndicatorPlotObject::app: invalid" << key;
    return 0;
  }
  
  new IndicatorPlotWidget(mw, _profile, _name);
  
  return 1;
}
