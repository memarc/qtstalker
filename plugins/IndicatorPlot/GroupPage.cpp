/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2011 Stefan S. Stratigakos
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

#include "GroupPage.h"
#include "Util.h"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/new.xpm"
#include "../pics/refresh.xpm"

#include <QDebug>

GroupPage::GroupPage (QWidget *p, QString settingsPath) : QWidget (p)
{
  _settingsPath = settingsPath;
  
  createActions();
  createGUI();
  loadSettings();
  loadGroups();
  selectionChanged();
}

GroupPage::~GroupPage ()
{
  saveSettings();
  qDeleteAll(_groups);
}

void
GroupPage::createActions ()
{
  QAction *action  = new QAction(QIcon(new_xpm), tr("&New Group") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
  action->setToolTip(tr("Create a new group") + "...");
  action->setStatusTip(tr("Create a new group") + "...");
  connect(action, SIGNAL(triggered(bool)), this, SLOT(newGroup()));
  _actions.insert(_GROUP_NEW, action);

  action  = new QAction(QIcon(edit_xpm), tr("&Edit Group") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
  action->setToolTip(tr("Edit group") + "...");
  action->setStatusTip(tr("Edit group") + "...");
  connect(action, SIGNAL(triggered(bool)), this, SLOT(editGroup()));
  _actions.insert(_GROUP_EDIT, action);

  action  = new QAction(QIcon(delete_xpm), tr("&Delete Group") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
  action->setToolTip(tr("Delete group") + "...");
  action->setStatusTip(tr("Delete group") + "...");
  connect(action, SIGNAL(triggered(bool)), this, SLOT(deleteGroup()));
  _actions.insert(_GROUP_DELETE, action);

  action  = new QAction(QIcon(refresh_xpm), tr("&Refresh List"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  action->setToolTip(tr("Refresh List"));
  action->setStatusTip(tr("Refresh List"));
  connect(action, SIGNAL(triggered(bool)), this, SLOT(loadGroups()));
  _actions.insert(_REFRESH, action);
}

void
GroupPage::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(3);
  setLayout(vbox);

  _nav = new QTreeWidget;
  _nav->setFocusPolicy(Qt::StrongFocus);
  _nav->setContextMenuPolicy(Qt::CustomContextMenu);
  _nav->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _nav->setSortingEnabled(TRUE);
  _nav->setHeaderLabels(QStringList() << tr("Group") << tr("Symbol"));
  _nav->setUniformRowHeights(TRUE);
  connect(_nav, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemClicked(QTreeWidgetItem *, int)));
  connect(_nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  connect(_nav, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(selectionChanged()));
  vbox->addWidget(_nav);
  
  _menu = new QMenu(this);
  _menu->addAction(_actions.value(_GROUP_NEW));
  _menu->addAction(_actions.value(_GROUP_EDIT));
  _menu->addAction(_actions.value(_GROUP_DELETE));
  _menu->addSeparator();
  _menu->addAction(_actions.value(_REFRESH));
}

void
GroupPage::newGroup ()
{
  bool ok;
  QString name = QInputDialog::getText(this,
				       tr("OTA - Indicator Plot - New Group"),
				       tr("Enter new group name"),
				       QLineEdit::Normal,
				       tr("New Group"),
				       &ok,
				       0,
				       0);
  if (name.isEmpty())
    return;

  if (_groups.contains(name))
  {
    QStringList mess;
    mess << name << tr("already exists");
    
    QMessageBox msgBox;
    msgBox.setText(mess.join(" "));
    msgBox.exec();
    return;
  }

  Util util;
  Object *group = util.object(QString("Group"), QString(), name);
  if (! group)
  {
    qDebug() << "GroupPage::newGroup: Group object error";
    return;
  }

  connect(group, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(groupObjectMessage(ObjectCommand)));
  _groups.insert(name, group);
  
  QTreeWidgetItem *i = new QTreeWidgetItem(_nav);
  i->setText(0, name);
  _items.insert(name, i);

  ObjectCommand toc(QString("dialog"));
  group->message(&toc);
}

void
GroupPage::editGroup ()
{
  QList<QTreeWidgetItem *> l = _nav->selectedItems();
  if (! l.size())
    return;

  QTreeWidgetItem *i = l.at(0);
  if (i->text(0).isEmpty())
    i = i->parent();
  
  Object *group = _groups.value(i->text(0));
  if (! group)
    return;
  
  ObjectCommand toc(QString("dialog"));
  group->message(&toc);
}

void
GroupPage::deleteGroup ()
{
  QList<QTreeWidgetItem *> l = _nav->selectedItems();
  if (! l.size())
    return;

  QTreeWidgetItem *i = l.at(0);
  if (i->text(0).isEmpty())
    i = i->parent();
  
  Object *group = _groups.value(i->text(0));
  if (! group)
    return;
  
  QStringList mess;
  mess << tr("Are you sure you want to delete") << "(" << i->text(0) << ")" << "?";

  QStringList wt;
  wt << QString("OTA") << "-" << tr("Confirm Group Delete");

  QMessageBox msgBox;
  msgBox.setWindowTitle(wt.join(" "));
  msgBox.setText(mess.join(" "));
  msgBox.setInformativeText("Confirm delete");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  int ret = msgBox.exec();
  if (ret == QMessageBox::No)
    return;

  ObjectCommand toc(QString("remove"));
  if (! group->message(&toc))
  {
    qDebug() << "GroupPage::deleteGroup: message error" << group->plugin() << toc.command();
    return;
  }

  delete group;
  _groups.remove(i->text(0));

  _items.remove(i->text(0));
  delete i;
  
  selectionChanged();
}

void
GroupPage::rightClick (const QPoint &)
{
  _menu->exec(QCursor::pos());
}

void
GroupPage::loadGroups ()
{
  _nav->clear();
  
  qDeleteAll(_groups);
  _groups.clear();
  
  _items.clear();
  
  Util util;
  Object *o = util.object(QString("Group"), QString(), QString());
  if (! o)
  {
    qDebug() << "GroupPage::loadGroups: invalid Group object";
    return;
  }
  
  QString key("groups");
  ObjectCommand toc(key);
  if (! o->message(&toc))
  {
    qDebug() << "GroupPage::loadGroups: message error" << o->plugin() << key;
    delete o;
    return;
  }
  else
    delete o;
  
  QStringList groups = toc.getList(key);

  Object *db = util.object(QString("Symbol"), QString(), QString());
  if (! db)
    return;
  
  for (int pos = 0; pos < groups.size(); pos++)
  {
    Object *group = util.object(QString("Group"), QString(), groups.at(pos));
    if (! group)
      continue;
    
    connect(group, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(groupObjectMessage(ObjectCommand)));
    
    QTreeWidgetItem *p = new QTreeWidgetItem(_nav);
    p->setText(0, groups.at(pos));
    
    ObjectCommand goc(QString("symbols"));
    if (! group->message(&goc))
    {
      delete group;
      continue;
    }

    QStringList tl = goc.getList(QString("symbols"));
    
    goc.setCommand(QString("info"));
    goc.setValue(QString("names"), tl);
    db->message(&goc);
    
    QHashIterator<QString, Data> it(goc.getDatas());
    while (it.hasNext())
    {
      it.next();
      Data d = it.value();
    
      QTreeWidgetItem *i = new QTreeWidgetItem(p);
      i->setText(1, it.key());
      i->setToolTip(1, d.value(QString("name")).toString());
    }

    _items.insert(groups.at(pos), p);
    _groups.insert(groups.at(pos), group);
  }

  delete db;
  
  selectionChanged();
}

void
GroupPage::selectionChanged ()
{
  bool status = FALSE;
  QList<QTreeWidgetItem *> l = _nav->selectedItems();
  if (l.size())
    status = TRUE;

  _actions.value(_GROUP_EDIT)->setEnabled(status);
  _actions.value(_GROUP_DELETE)->setEnabled(status);
}

void
GroupPage::itemClicked (QTreeWidgetItem *d, int col)
{
  if (! d)
    return;
  
  if (col != 1)
    return;
  
  QString name = d->text(1);
  if (name.isEmpty())
    return;

  emit signalSymbol(name);
}

void
GroupPage::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("group_tab"));
  
  _nav->setColumnWidth(0, settings.value(QString("column_0_width"), 50).toInt());
}

void
GroupPage::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("group_tab"));
  
  settings.setValue(QString("column_0_width"), _nav->columnWidth(0));
}

void
GroupPage::groupObjectMessage (ObjectCommand oc)
{
  if (oc.command() != QString("modified"))
    return;
  
  QString name = oc.getString(QString("group"));
  
  QTreeWidgetItem *p = _items.value(name);
  if (! p)
  {
    qDebug() << "GroupPage::groupObjectCommand: parent item not found" << name;
    return;
  }
  
  p->takeChildren();
  
  QStringList sl = oc.getList(QString("symbols"));
  
  Util util;
  Object *db = util.object(QString("Symbol"), QString(), QString());
  if (! db)
    return;
  
  ObjectCommand toc(QString("info"));
  toc.setValue(QString("names"), sl);
  db->message(&toc);
  
  QHashIterator<QString, Data> it(toc.getDatas());
  while (it.hasNext())
  {
    it.next();
    Data d = it.value();
  
    QTreeWidgetItem *i = new QTreeWidgetItem(p);
    i->setText(1, it.key());
    i->setToolTip(1, d.value(QString("name")).toString());
  }
  
  delete db;
  
  selectionChanged();
}
