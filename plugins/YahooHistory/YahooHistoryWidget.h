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

#ifndef PLUGIN_YAHOO_HISTORY_WIDGET_H
#define PLUGIN_YAHOO_HISTORY_WIDGET_H

#include <QtGui>

#include "Object.h"

class YahooHistoryWidget : public QWidget
{
  Q_OBJECT

  signals:
    void signalStop ();
    
  public:
    enum Action
    {
      _DOWNLOAD,
      _HELP,
      _QUIT,
      _STOP
    };
    
    YahooHistoryWidget (QMainWindow *, QString profile);
    ~YahooHistoryWidget ();
    void createActions ();
    void createGUI ();
    void loadSettings ();
    void saveSettings ();
    
  public slots:
    void downloadHistory ();
    void buttonStatus ();
    void help ();
    void downloadDone ();
    void threadMessage (ObjectCommand);
    
  private:
    Object *_symbolButton;
    QTextEdit *_log;
    QString _helpFile;
    QDateTimeEdit *_startDate;
    QDateTimeEdit *_endDate;
    bool _downloading;
    QString _dbPath;
    QProgressBar *_progBar;
    QToolBar *_toolBar;
    QStatusBar *_statusBar;
    QHash<int, QAction *> _actions;
    QMainWindow *_mw;
    QString _profile;
};

#endif
