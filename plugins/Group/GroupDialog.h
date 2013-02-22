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

#ifndef PLUGIN_GROUP_DIALOG_HPP
#define PLUGIN_GROUP_DIALOG_HPP

#include <QtGui>

#include "Dialog.h"
#include "Object.h"

class GroupDialog : public Dialog
{
  Q_OBJECT

  public:
    enum Actions
    {
      _ADD,
      _DELETE,
      _SELECT_ALL,
      _UNSELECT_ALL
    };
    
    GroupDialog (QString);
    ~GroupDialog ();
    void createActions ();
    void createTab ();
    void setSettings (QHash<QString, Data> &);
    void settings (QHash<QString, Data> &);
    void updateList ();

  public slots:
    void done ();
    void selectionChanged ();
    void addButtonPressed ();
    void deleteButtonPressed ();
    void loadSettings ();
    void saveSettings ();
    void symbolObjectMessage (ObjectCommand);

  private:
    QHash<int, QAction *> _actions;
    QTreeWidget *_list;
    Object *_symbolObject;
    QHash<QString, Data> _symbols;
};

#endif
