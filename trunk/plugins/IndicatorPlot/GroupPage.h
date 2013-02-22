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

#ifndef PLUGIN_INDICATOR_GROUP_PAGE_HPP
#define PLUGIN_INDICATOR_GROUP_PAGE_HPP

#include <QtGui>

#include "Object.h"

class GroupPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalSymbol (QString);
  
  public:
    enum Action
    {
      _GROUP_NEW,
      _GROUP_EDIT,
      _GROUP_DELETE
    };

    GroupPage (QWidget *, QString settingsPath);
    ~GroupPage ();
    void createGUI();
    void createActions ();
    void loadSettings ();
    void saveSettings ();

  public slots:
    void newGroup ();
    void editGroup ();
    void deleteGroup ();
    void rightClick (const QPoint &);
    void loadGroups ();
    void selectionChanged ();
    void itemClicked (QTreeWidgetItem *, int);
    void groupObjectMessage (ObjectCommand);

  protected:
    QTreeWidget *_nav;
    QMenu *_menu;
    QHash<int, QAction *> _actions;
    QHash<QString, Object *> _groups;
    QHash<QString, QTreeWidgetItem *> _items;
    QString _settingsPath;
};

#endif
