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

#ifndef PLUGIN_INDICATOR_DIALOG_HPP
#define PLUGIN_INDICATOR_DIALOG_HPP

#include <QtGui>
#include <QSettings>

#include "Object.h"
#include "Dialog.h"

class IndicatorDialog : public Dialog
{
  Q_OBJECT
  
  public:
    enum Action
    {
      _CURVE_INSERT,
      _INDICATOR_INSERT,
      _MARKER_INSERT,
      _PLOT_INSERT,
      _STEP_REMOVE
    };
    
    IndicatorDialog (QString file, QString name);
    ~IndicatorDialog ();
    void clear ();
    void createActions ();
    void createTab ();
    int loadIndicator ();
    int saveIndicator ();
    
  public slots:
    void done ();
    void stepDoubleClicked (QListWidgetItem *);
    void insertIndicator ();
    void insertCurve ();
    void insertMarker ();
    void insertPlot ();
    void addStep2 (void *);
    void removeStep ();
    void buttonStatus ();
    void saveSettings ();
    void loadSettings ();
   
  private:
    QHash<int, QAction *> _actions;
    QString _indicatorPath;
    QStringList _indicatorList;
    QStringList _curveList;
    QStringList _markerList;
    QListWidget *_list;
    QHash<QString, Object *> _objects;
};

#endif
