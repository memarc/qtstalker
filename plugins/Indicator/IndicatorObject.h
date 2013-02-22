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

#ifndef PLUGIN_INDICATOR_OBJECT_HPP
#define PLUGIN_INDICATOR_OBJECT_HPP

//#include <QHash>

#include "Object.h"
#include "IndicatorDialog.h"

class IndicatorObject : public Object
{
  Q_OBJECT
  
  public:
    IndicatorObject (QString profile, QString name);
    ~IndicatorObject ();
    int calculate (ObjectCommand *);
    int setInput (ObjectCommand *);
    int setIndex (ObjectCommand *);
    int loadMarkers (ObjectCommand *);
    int dialog (ObjectCommand *);
    int load (ObjectCommand *);
    int pluginSteps (ObjectCommand *);

    void clear();
    int load ();
    int plot ();
    void savePlotSettings ();
    int remove ();
    void saveMarkers ();
    QWidget * widget ();
    
  public slots:
    int message (ObjectCommand *);
    void dialogDone ();
    void dialogCancel ();
    void plotMessage (ObjectCommand);
   
  private:
    QStringList _commandList;
    QStringList _order;
    QStringList _plotCommands;
    QString _indicatorPath;
    QString _symbol;
    QString _length;
    QHash<QString, Object *> _objects;
    QWidget *_widget;
    QGridLayout *_grid;
};

#endif
