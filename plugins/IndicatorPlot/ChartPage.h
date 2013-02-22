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

#ifndef PLUGIN_INDICATOR_CHART_PAGE_HPP
#define PLUGIN_INDICATOR_CHART_PAGE_HPP

#include <QStringList>
#include <QWidget>
#include <QMenu>
#include <QPoint>
#include <QHash>
#include <QAction>
#include <QListWidget>

#include "Object.h"

class ChartPage : public QWidget
{
  Q_OBJECT
  
  signals:
    void signalSymbol (QString);
    void signalGroupAdd (QString);

  public:
    enum Action
    {
      _SHOW_ALL,
      _SEARCH,
      _ADD_GROUP,
      _SELECT_ALL,
      _UNSELECT_ALL
    };

    ChartPage (QWidget *, QString, QString profile);
    ~ChartPage ();
    void createActions ();
    void createMenu ();
    void loadSettings ();
    void saveSettings ();
    QString currentSymbol ();

  public slots:
    void rightClick (const QPoint &);
    void addToGroup ();
    void symbolSearch ();
    void allButtonPressed ();
    void buttonStatus ();
    void updateList ();
    void updateList2 ();
    void symbolSelected (QListWidgetItem *);
    void symbolObjectMessage (ObjectCommand);

  protected:
    QString _searchTicker;
    QString _searchExchange;
    QString _searchType;
    QString _searchName;
    QString _profile;
    int _trow;
    QListWidget *_nav;
    QMenu *_menu;
    QHash<int, QAction *> _actions;
    QString _settingsPath;
    Object *_symbolObject;
    QHash<QString, Data> _symbols;
};

#endif
