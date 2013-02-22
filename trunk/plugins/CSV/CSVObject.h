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

#ifndef PLUGIN_CSV_OBJECT_HPP
#define PLUGIN_CSV_OBJECT_HPP

#include <QHash>
#include <QStringList>
#include <QObject>

#include "Object.h"

class CSVObject : public Object
{
  Q_OBJECT
  
  public:
    CSVObject (QString profile, QString name);
    ~CSVObject ();
    int message (ObjectCommand *);
    int app (ObjectCommand *);
    int import (ObjectCommand *);
    int import2 (QStringList files, QString format, QString dateFormat, QString delimiter,
                QString type, QString exchange, bool useFilename);
    void errorMessage (QString file, int line, QString mess);
    
  private:
    QStringList _commandList;
};

#endif
