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

#ifndef PLUGIN_INDICATOR_INPUT_OBJECT_HPP
#define PLUGIN_INDICATOR_INPUT_OBJECT_HPP

#include <QStringList>

#include "Object.h"
#include "IndicatorInputWidget.h"


class IndicatorInputObject : public Object
{
  Q_OBJECT

  public:
    IndicatorInputObject (QString profile, QString name);
    ~IndicatorInputObject ();
    int setObjects (ObjectCommand *);
    int setInput (ObjectCommand *);
    int input (ObjectCommand *);
    int setKey (ObjectCommand *);
    int key (ObjectCommand *);
    int setOffset (ObjectCommand *);
    int offset (ObjectCommand *);
    int setEnabled (ObjectCommand *);
    QWidget * widget ();
    
  public slots:    
    int message (ObjectCommand *);
    void inputChanged ();
    
  private:
    QStringList _commandList;
    IndicatorInputWidget *_widget;
};

#endif
