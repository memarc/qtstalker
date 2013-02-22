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

#ifndef PLUGIN_DATABASE_OBJECT_HPP
#define PLUGIN_DATABASE_OBJECT_HPP

#include <QtSql>
#include <QStringList>

#include "Object.h"

class DataBaseObject : public Object
{
  Q_OBJECT

  public:
    DataBaseObject (QString profile, QString name);
    int message (ObjectCommand *);
    int open (ObjectCommand *);
    int createTable (QString table);
    int transaction ();
    int commit ();
    int remove (ObjectCommand *);
    int remove2 (QString table, QString name);
    int set (ObjectCommand *);
    int get (ObjectCommand *);
    int names (ObjectCommand *);
    int search (ObjectCommand *);
    int getRange (ObjectCommand *);
    int drop (ObjectCommand *);
    
  private:
    QStringList _commandList;
    QSqlDatabase _db;
};

#endif
