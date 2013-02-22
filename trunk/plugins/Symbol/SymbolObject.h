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

#ifndef PLUGIN_SYMBOL_OBJECT_HPP
#define PLUGIN_SYMBOL_OBJECT_HPP

#include <QStringList>
#include <QDateTime>
#include <QMap>

#include "Object.h"
#include "Data.h"
#include "SymbolDialog.h"


class SymbolObject : public Object
{
  Q_OBJECT

  public:
    SymbolObject (QString profile, QString name);
    ~SymbolObject ();
    int loadBars (ObjectCommand *);
    int saveBars (ObjectCommand *);
    int values (ObjectCommand *);
    int search (ObjectCommand *);
    int size (ObjectCommand *);
    int dialog (ObjectCommand *);
    int symbol (ObjectCommand *);
    int load (ObjectCommand *);
    int save (ObjectCommand *);
    int copy (ObjectCommand *);
    int outputKeys (ObjectCommand *);
    int info (ObjectCommand *);
    int exchanges (ObjectCommand *);
    int searchDialog (ObjectCommand *);
    
    int getBars (QDateTime &startDate, QDateTime &endDate, QString table, QString length);
    int getInterval (QString &length, int interval, QDateTime &sd, QDateTime &ed);
    int getInfo (QString symbol, Data &);
    int setInfo (Data &);
    
  public slots:
    int message (ObjectCommand *);
    void dialogDone (void *);
    void searchDialogDone (void *);
    
  private:
    QStringList _commands;
    QStringList _lengths;
    QMap<int, Data *> _bars;
    QString _path;
    QString _file;
    QString _exchange;
    QString _ticker;
    Data _info;
    int _interval;
    Object *_db;
};

#endif
