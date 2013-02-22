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

#ifndef PLUGIN_PLOT_OBJECT_HPP
#define PLUGIN_PLOT_OBJECT_HPP

#include <QStringList>

#include "Object.h"
#include "PlotWidget.h"

class PlotObject : public Object
{
  Q_OBJECT

  public:
    PlotObject (QString profile, QString name);
    ~PlotObject ();
    int setDates (ObjectCommand *);
    int addObjects (ObjectCommand *);
    int setIndex (ObjectCommand *);
    int startPosition (ObjectCommand *);
    int setGrid (ObjectCommand *);
    int setSpacing (ObjectCommand *);
    int clear (ObjectCommand *);
    int draw (ObjectCommand *);
    int load (ObjectCommand *);
    int save (ObjectCommand *);
    int objects (ObjectCommand *);
    int setHighLow (ObjectCommand *);
    int convertToY (ObjectCommand *);
    int dateToIndex (ObjectCommand *);
    int indexToX (ObjectCommand *);
    int xToIndex (ObjectCommand *);
    int indexToDate (ObjectCommand *);
    int convertToVal (ObjectCommand *);
    int size (ObjectCommand *);
    int xToDate (ObjectCommand *);
    int endPageIndex (ObjectCommand *);
    int loadMarkers (ObjectCommand *);
    int removeMarkers (ObjectCommand *);
    int dialog (ObjectCommand *);

    QWidget * widget ();
    
  public slots:
    int message (ObjectCommand *);
    void widgetSettingsChanged ();
    void markerNew (QString, QString);
    void markerDelete (QStringList);
    void plotInfo (Data);
    void dialogDone (void *);
    void dataWindow ();
    
  private:
    QStringList _commandList;
    PlotWidget *_widget;
    QString _symbol;
    int _row;
    int _col;
};

#endif
