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

#ifndef PLUGIN_SYMBOL_SELECT_DIALOG_HPP
#define PLUGIN_SYMBOL_SELECT_DIALOG_HPP

#include <QtGui>

#include "Dialog.h"
#include "Object.h"

class SymbolSelectDialog : public Dialog
{
  Q_OBJECT

  public:
    enum Actions
    {
      _ADD,
      _DELETE,
      _SEARCH,
      _SELECT_ALL,
      _UNSELECT_ALL
    };

    SymbolSelectDialog (QString name);
    ~SymbolSelectDialog ();
    void createActions ();
    void createTab ();
    void loadExchanges ();
    void setSettings (QString exchange, QString ticker, QString type, QString name);
    void settings (QHash<QString, Data> &, QString &exchange, QString &ticker, QString &type, QString &name);
    void setSearchList (QStringList);

  public slots:
    void done ();
    void searchSelectionChanged ();
    void symbolSelectionChanged ();
    void searchButtonPressed ();
    void addButtonPressed ();
    void deleteButtonPressed ();
    void loadSettings ();
    void saveSettings ();

  private:
    QTreeWidget *_searchList;
    QTreeWidget *_symbolList;
    QComboBox *_exchanges;
    QLineEdit *_search;
    QLineEdit *_symbolType;
    QLineEdit *_symbolName;
    Object *_symbolObject;
    QHash<int, QAction *> _actions;
};

#endif
