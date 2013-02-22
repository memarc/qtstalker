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

#include "GroupDialog.h"
#include "Util.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"
#include "../pics/select_all.xpm"
#include "../pics/unselect_all.xpm"

#include <QtDebug>

GroupDialog::GroupDialog (QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Group") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  Util util;
  _symbolObject = util.object(QString("Symbol"), QString(), QString());
  if (_symbolObject)
    connect(_symbolObject, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(symbolObjectMessage(ObjectCommand)));
    
  createActions();
  createTab();
  loadSettings();
  selectionChanged();

  tl.clear();
  tl << QString("OTA") << "-" << tr("Editing Group") << "(" << _name << ")";
  setWindowTitle(tl.join(" "));
}

GroupDialog::~GroupDialog ()
{
  saveSettings();

  if (_symbolObject)
    delete _symbolObject;
}

void
GroupDialog::createActions ()
{
  QAction *a = new QAction(QIcon(add_xpm), tr("Add Symbols"), this);
  a->setToolTip(tr("Add Symbols"));
  a->setStatusTip(tr("Add Symbols"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(addButtonPressed()));
  _actions.insert(_ADD, a);

  a = new QAction(QIcon(delete_xpm), tr("Delete Symbols"), this);
  a->setToolTip(tr("Delete Symbols"));
  a->setStatusTip(tr("Delete Symbols"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(deleteButtonPressed()));
  _actions.insert(_DELETE, a);

  a = new QAction(QIcon(select_all_xpm), tr("Select All"), this);
  a->setToolTip(tr("Select All"));
  a->setStatusTip(tr("Select All"));
  _actions.insert(_SELECT_ALL, a);

  a = new QAction(QIcon(unselect_all_xpm), tr("Unselect All"), this);
  a->setToolTip(tr("Unselect All"));
  a->setStatusTip(tr("Unselect All"));
  _actions.insert(_UNSELECT_ALL, a);
}

void
GroupDialog::createTab ()
{
  QWidget *w = new QWidget;

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(10);
  w->setLayout(hbox);
  
  _list = new QTreeWidget;
  _list->setHeaderLabels(QStringList() << tr("Symbol") << tr("Name"));
  _list->setSortingEnabled(TRUE);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _list->setUniformRowHeights(TRUE);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  hbox->addWidget(_list);

  QToolBar *tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  hbox->addWidget(tb);
  
  tb->addAction(_actions.value(_ADD));
  tb->addAction(_actions.value(_DELETE));
  tb->addAction(_actions.value(_SELECT_ALL));
  tb->addAction(_actions.value(_UNSELECT_ALL));

  // wait until we create list before we connect the actions
  connect(_actions.value(_SELECT_ALL), SIGNAL(triggered()), _list, SLOT(selectAll()));
  connect(_actions.value(_UNSELECT_ALL), SIGNAL(triggered()), _list, SLOT(clearSelection()));
  
  _tabs->addTab(w, tr("Symbols"));
}

void
GroupDialog::selectionChanged ()
{
  bool status = FALSE;
  QList<QTreeWidgetItem *> sl = _list->selectedItems();
  if (sl.count())
    status = TRUE;

  _actions.value(_DELETE)->setEnabled(status);
}

void
GroupDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
GroupDialog::addButtonPressed ()
{
  if (! _symbolObject)
    return;
  
  ObjectCommand toc(QString("search_dialog"));
  _symbolObject->message(&toc);
}

void
GroupDialog::deleteButtonPressed ()
{
  QList<QTreeWidgetItem *> l = _list->selectedItems();
  if (! l.size())
    return;
  
  for (int pos = 0; pos < l.size(); pos++)
    _symbols.remove(l.at(pos)->text(0));
  
  qDeleteAll(l);
  
  modified();
}

void
GroupDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);

  _list->setColumnWidth(0, settings.value(QString("list_column_width_0"), 150).toInt());

  Dialog::loadSettings(settings);
}

void
GroupDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);

  settings.setValue(QString("list_column_width_0"), _list->columnWidth(0));

  Dialog::saveSettings(settings);
}

void
GroupDialog::setSettings (QHash<QString, Data> &d)
{
  _symbols = d;
  
  updateList();
}

void
GroupDialog::settings (QHash<QString, Data> &d)
{
  d = _symbols;
}

void
GroupDialog::symbolObjectMessage (ObjectCommand oc)
{
  if (oc.command() != QString("search"))
    return;

  QHashIterator<QString, Data> it(oc.getDatas());
  while (it.hasNext())
  {
    it.next();
    Data d = it.value();
    _symbols.insert(it.key(), d);
  }

  updateList();
  
  modified();
}

void
GroupDialog::updateList ()
{
  _list->clear();

  QHashIterator<QString, Data> it(_symbols);
  while (it.hasNext())
  {
    it.next();
    Data d = it.value();
    QTreeWidgetItem *i = new QTreeWidgetItem(_list);
    i->setText(0, it.key());
    i->setText(1, d.value(QString("name")).toString());
  }
}

