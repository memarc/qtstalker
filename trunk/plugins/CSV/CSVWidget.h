/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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


#ifndef PLUGIN_CSV_GUI_WIDGET_H
#define PLUGIN_CSV_GUI_WIDGET_H

#include <QtGui>
#include <QAction>
#include <QStringList>

#include "Object.h"

class CSVWidget : public QWidget
{
  Q_OBJECT

  signals:
    void signalStop ();
    
  public:
    enum Action
    {
      _IMPORT,
      _HELP,
      _QUIT,
      _STOP
    };
    
    CSVWidget (QMainWindow *mw, QString profile);
    ~CSVWidget ();
    void createActions ();
    void createGUI ();
    void loadSettings ();
    void saveSettings ();
   
  public slots:
    void importStart ();
    void importThread ();
    void importDone ();
    void buttonStatus ();
    void help ();
    void threadMessage (ObjectCommand);
   
  private:
    Object *_csvButton;
    QLineEdit *_format;
    QLineEdit *_dateFormat;
    QComboBox *_delimiter;
    QComboBox *_type;
    QLineEdit *_exchange;
    QCheckBox *_filename;
    QTextEdit *_log;
    QProgressBar *_progBar;
    QToolBar *_toolBar;
    QStatusBar *_statusBar;
    QHash<int, QAction *> _actions;
    QMainWindow *_mw;
    QString _profile;
    bool _cancel;
    QString _helpFile;
    QString _dbPath;
};

#endif
