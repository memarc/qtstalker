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

#include "ChartPage.h"
#include "Util.h"

#include "../pics/add.xpm"
#include "../pics/search.xpm"
#include "../pics/asterisk.xpm"
#include "../pics/select_all.xpm"
#include "../pics/unselect_all.xpm"

#include <QLayout>
#include <QtDebug>
#include <QSettings>

ChartPage::ChartPage (QWidget *p, QString sp, QString profile) : QWidget (p)
{
  _settingsPath = sp;
  _profile = profile;
  _trow = -1;
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);

  _nav = new QListWidget;
  _nav->setFocusPolicy(Qt::StrongFocus);
  _nav->setContextMenuPolicy(Qt::CustomContextMenu);
  _nav->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _nav->setSortingEnabled(TRUE);
  connect(_nav, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(symbolSelected(QListWidgetItem *)));
  connect(_nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  connect(_nav, SIGNAL(itemSelectionChanged()), this, SLOT(buttonStatus()));
  vbox->addWidget(_nav);
  
  Util util;
  _symbolObject = util.object(QString("Symbol"), QString(), QString());
  if (_symbolObject)
    connect(_symbolObject, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(symbolObjectMessage(ObjectCommand)));

  loadSettings();

  createActions();

  createMenu();

  updateList();
  _nav->setCurrentRow(_trow);
}

ChartPage::~ChartPage ()
{
  saveSettings();
  
  if (_symbolObject)
    delete _symbolObject;
}

void
ChartPage::createActions ()
{
  QAction *action  = new QAction(QIcon(asterisk_xpm), tr("Show &All Symbols"), this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Asterisk));
  action->setToolTip(tr("Show All Symbols"));
  action->setStatusTip(tr("Show All Symbols"));
  connect(action, SIGNAL(triggered(bool)), this, SLOT(allButtonPressed()));
  _actions.insert(_SHOW_ALL, action);

  action  = new QAction(QIcon(search_xpm), tr("Symbol &Search") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
  action->setToolTip(tr("Symbol Search") + "...");
  action->setStatusTip(tr("Symbol Search") + "...");
  connect(action, SIGNAL(triggered(bool)), this, SLOT(symbolSearch()));
  _actions.insert(_SEARCH, action);

  action  = new QAction(QIcon(add_xpm), tr("Add To &Group") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
  action->setToolTip(tr("Add symbol to group") + "...");
  action->setStatusTip(tr("Add symbol to group") + "...");
  connect(action, SIGNAL(triggered(bool)), this, SLOT(addToGroup()));
  _actions.insert(_ADD_GROUP, action);

  action  = new QAction(QIcon(select_all_xpm), tr("Select All"), this);
  action->setToolTip(tr("Select All"));
  action->setStatusTip(tr("Select All"));
  connect(action, SIGNAL(triggered(bool)), _nav, SLOT(selectAll()));
  _actions.insert(_SELECT_ALL, action);

  action  = new QAction(QIcon(unselect_all_xpm), tr("Unselect All"), this);
  action->setToolTip(tr("Unselect All"));
  action->setStatusTip(tr("Unselect All"));
  connect(action, SIGNAL(triggered(bool)), _nav, SLOT(clearSelection()));
  _actions.insert(_UNSELECT_ALL, action);
}

void
ChartPage::createMenu ()
{
  _menu = new QMenu(this);
  _menu->addAction(_actions.value(_SHOW_ALL));
  _menu->addAction(_actions.value(_SEARCH));
  _menu->addSeparator();
  _menu->addAction(_actions.value(_ADD_GROUP));
  _menu->addSeparator();
  _menu->addAction(_actions.value(_SELECT_ALL));
  _menu->addAction(_actions.value(_UNSELECT_ALL));
}

void
ChartPage::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("chart_tab"));
  _searchExchange = settings.value(QString("search_exchange"), QString("%")).toString();
  _searchTicker = settings.value(QString("search_ticker"), QString("%")).toString();
  _searchType = settings.value(QString("search_type"), QString("%")).toString();
  _searchName = settings.value(QString("search_name"), QString("%")).toString();
  _trow = settings.value(QString("current_row")).toInt();
  
}

void
ChartPage::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("chart_tab"));
  settings.setValue(QString("search_exchange"), _searchExchange);
  settings.setValue(QString("search_ticker"), _searchTicker);
  settings.setValue(QString("search_type"), _searchType);
  settings.setValue(QString("search_name"), _searchName);
  settings.setValue(QString("current_row"), _nav->currentRow());
  settings.sync();
}

void
ChartPage::rightClick (const QPoint &)
{
  _menu->exec(QCursor::pos());
}

void
ChartPage::addToGroup ()
{
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (! l.count())
    return;

  QStringList tl;
  int loop = 0;
  for (; loop < l.count(); loop++)
    tl << l.at(loop)->text();
  
  Util util;
  Object *db = util.object(QString("Group"), QString(), QString());
  if (! db)
    return;
  
  QString key(QString("groups"));
  ObjectCommand toc(key);
  if (! db->message(&toc))
  {
    delete db;
    return;
  }
  
  QStringList groups = toc.getList(key);
  if (! groups.size())
  {
    delete db;
    
    QStringList wt;
    wt << QString("OTA") << "(" << _profile << ")" << "-" << tr("Add To Group Error");

    QMessageBox msgBox;
    msgBox.setWindowTitle(wt.join(" "));
    msgBox.setText(tr("No groups available.\n\nCreate a new group first."));
    msgBox.exec();
    return;
  }
  
  QStringList wt;
  wt << QString("OTA") << "(" << _profile << ")" << "-" << tr("Select Group");
  
  bool ok;
  QString group = QInputDialog::getItem(this,
					wt.join(" "),
					tr("Groups"),
					groups,
					0,
					FALSE,
					&ok,
					0,
					0);
  if (group.isEmpty())
  {
    delete db;
    return;
  }
  
  toc.setCommand(QString("add"));
  toc.setValue(QString("group"), group);
  toc.setValue(QString("list"), tl);
  if (! db->message(&toc))
  {
    delete db;
    return;
  }
  
  delete db;
  
  emit signalGroupAdd(group);
}

void
ChartPage::updateList ()
{
  if (! _symbolObject)
    return;
  
  ObjectCommand toc(QString("search"));
  toc.setValue(QString("exchange"), _searchExchange);
  toc.setValue(QString("ticker"), _searchTicker);
  toc.setValue(QString("type"), _searchType);
  toc.setValue(QString("name"), _searchName);
  if (! _symbolObject->message(&toc))
  {
    qDebug() << "ChartPage::updateList: message error" << _symbolObject->plugin() << toc.command();
    return;
  }

  toc.setCommand(QString("info"));
  toc.setValue(QString("names"), toc.getList(QString("list")));
  if (! _symbolObject->message(&toc))
  {
    qDebug() << "ChartPage::updateList: message error" << _symbolObject->plugin() << toc.command();
    return;
  }

  _symbols = toc.getDatas();
  
  updateList2();
}

void
ChartPage::symbolSearch ()
{
  if (! _symbolObject)
    return;
  
  ObjectCommand toc(QString("search_dialog"));
  toc.setValue(QString("exchange"), _searchExchange);
  toc.setValue(QString("ticker"), _searchTicker);
  toc.setValue(QString("type"), _searchType);
  toc.setValue(QString("name"), _searchName);
  _symbolObject->message(&toc);
}

void
ChartPage::allButtonPressed ()
{
  _searchExchange = "%";
  _searchTicker = "%";
  _searchType = "%";
  _searchName = "%";
  updateList();
}

void
ChartPage::buttonStatus ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (l.count())
    status = TRUE;

  _actions.value(_ADD_GROUP)->setEnabled(status);
}

QString
ChartPage::currentSymbol ()
{
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (! l.count())
    return QString();
  
  return l.at(0)->text();
}

void
ChartPage::symbolSelected (QListWidgetItem *d)
{
  if (! d)
    return;
  
  emit signalSymbol(d->text());
}

void
ChartPage::symbolObjectMessage (ObjectCommand oc)
{
  if (oc.command() != QString("search"))
    return;

  _symbols = oc.getDatas();
  _searchExchange = oc.getString(QString("exchange"));
  _searchTicker = oc.getString(QString("ticker"));
  _searchType = oc.getString(QString("type"));
  _searchName = oc.getString(QString("name"));
  
  updateList2();
}

void
ChartPage::updateList2 ()
{
  _nav->clear();
  _nav->setSortingEnabled(FALSE);

  if (! _symbolObject)
    return;
  
  _nav->blockSignals(TRUE);

  QHashIterator<QString, Data> it(_symbols);
  while (it.hasNext())
  {
    it.next();
    Data d = it.value();
    
    QListWidgetItem *item = new QListWidgetItem;
    item->setText(it.key());
    item->setToolTip(d.value(QString("name")).toString());
    _nav->addItem(item);
  }
  
  _nav->blockSignals(FALSE);
  _nav->setSortingEnabled(TRUE);

  _nav->sortItems(Qt::AscendingOrder);
  
  buttonStatus();
}
