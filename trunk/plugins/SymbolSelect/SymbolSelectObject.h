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

#ifndef PLUGIN_SYMBOL_SELECT_OBJECT_HPP
#define PLUGIN_SYMBOL_SELECT_OBJECT_HPP

#include <QStringList>

#include "Object.h"


class SymbolSelectObject : public Object
{
  Q_OBJECT

  public:
    SymbolSelectObject (QString profile, QString name);
    ~SymbolSelectObject ();
    int dialog (ObjectCommand *);
    int load (ObjectCommand *);
    int save (ObjectCommand *);
    int setSettings (ObjectCommand *);
    int settings (ObjectCommand *);
    int size (ObjectCommand *);
    
  public slots:    
    int message (ObjectCommand *);
    void symbolsChanged ();
    void dialogDone (void *);
    
  private:
    QStringList _commandList;
    QString _exchange;
    QString _ticker;
    QString _corpType;
    QString _corp;
    QStringList _symbols;
};

#endif
