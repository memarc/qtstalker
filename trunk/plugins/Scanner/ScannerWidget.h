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

#ifndef PLUGIN_SCANNER_WIDGET_H
#define PLUGIN_SCANNER_WIDGET_H

#include <QtGui>

#include "Object.h"

class ScannerWidget : public QWidget
{
  Q_OBJECT

  signals:
    void signalStop ();
    
  public:
    enum Action
    {
      _INDICATOR_EDIT,
      _INDICATOR_NEW,
      _INDICATOR_REMOVE,
      _RESULT_SAVE_GROUP,
      _SCAN,
      _STOP
    };
    
    ScannerWidget (QMainWindow *, QString profile);
    ~ScannerWidget ();
    void createActions ();
    void createGUI ();
    void loadSettings ();
    int addIndicatorItem (QString file, QString name);
    void saveIndicatorSettings ();
    
  public slots:
    void scan ();
    void buttonStatus ();
    void help ();
    void done ();
    void threadMessage (ObjectCommand);
    void newIndicator ();
    void editIndicator ();
    void removeIndicator ();
    void saveSettings ();
    void saveResults ();
    void indicatorMessage (ObjectCommand);
    
    void indicatorItemClicked (QTreeWidgetItem *, int);
//    void indicatorItemDoubleClicked (QTreeWidgetItem *, int);
    
  private:
    Object *_symbolButton;
    QTreeWidget *_indicators;
    QListWidget *_results;
    QProgressBar *_progBar;
    QToolBar *_toolBar;
    QStatusBar *_statusBar;
    QHash<int, QAction *> _actions;
    QMainWindow *_mw;
    Object *_rangeButton;
    Object *_barLengthButton;
    QString _profile;
    QString _helpFile;
    QString _settingsPath;
    QString _indicatorPath;
    bool _scanning;
    QHash<QString, Object *> _objects;
};

#endif
