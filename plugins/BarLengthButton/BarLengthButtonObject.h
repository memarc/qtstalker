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

#ifndef PLUGIN_BAR_LENGTH_BUTTON_OBJECT_HPP
#define PLUGIN_BAR_LENGTH_BUTTON_OBJECT_HPP

#include <QStringList>

#include "Object.h"
#include "BarLengthButtonWidget.h"


class BarLengthButtonObject : public Object
{
  Q_OBJECT

  public:
    BarLengthButtonObject (QString profile, QString name);
    ~BarLengthButtonObject ();
    int length (ObjectCommand *);
    int load (ObjectCommand *);
    int save (ObjectCommand *);
    int list (ObjectCommand *);
    int interval (ObjectCommand *);
    QWidget * widget ();
    
  public slots:    
    int message (ObjectCommand *);
    void lengthChanged ();
    
  private:
    QStringList _commandList;
    QStringList _lengthList;
    BarLengthButtonWidget *_widget;
};

#endif
