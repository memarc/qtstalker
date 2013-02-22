/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "CurveBar.h"
#include "CurveBarObject.h"

CurveBar::CurveBar ()
{
  _commandList << QString("type");
  _commandList << QString("object");
}

int
CurveBar::command (PluginCommand *pc)
{
  int rc = 0;

  switch (_commandList.indexOf(pc->command))
  {
    case 0: // type
      pc->type = QString("curve");
      rc = 1;
      break;
    case 1: // object
      pc->object = new CurveBarObject(pc->profile, pc->name);
      rc = 1;
      break;
    default:
      break;
  }
  
  return rc;
}

// do not remove
Q_EXPORT_PLUGIN2(CurveBar, CurveBar);
