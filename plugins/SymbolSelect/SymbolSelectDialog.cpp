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

#include "SymbolSelectDialog.h"
#include "Util.h"

#include "../pics/search.xpm"
#include "../pics/select_all.xpm"
#include "../pics/unselect_all.xpm"
#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QtDebug>


SymbolSelectDialog::SymbolSelectDialog (QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("SymbolSelect") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  Util util;
  _symbolObject = util.object(QString("Symbol"), QString(), QString());

  createActions();

  createTab();

  loadExchanges();

  loadSettings();

  searchSelectionChanged();
  symbolSelectionChanged();

  _search->setFocus();

  tl.clear();
  tl << QString("OTA") << "-" << tr("Select Symbols");
  setWindowTitle(tl.join(" "));

  ignoreChanges();
}

SymbolSelectDialog::~SymbolSelectDialog ()
{
  saveSettings();
  
  if (_symbolObject)
    delete _symbolObject;
}

void
SymbolSelectDialog::createActions ()
{
  QAction *a = new QAction(QIcon(add_xpm), tr("Add"), this);
  a->setToolTip(tr("Add selected to symbol list"));
  a->setStatusTip(tr("Add selected to symbol list"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(addButtonPressed()));
  _actions.insert(_ADD, a);

  a = new QAction(QIcon(delete_xpm), tr("Remove"), this);
  a->setToolTip(tr("Remove selected from symbol list"));
  a->setStatusTip(tr("Remove selected from symbol list"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(deleteButtonPressed()));
  _actions.insert(_DELETE, a);

  a = new QAction(QIcon(select_all_xpm), tr("Select all symbols"), this);
  a->setToolTip(tr("Select all symbols"));
  a->setStatusTip(tr("Select all symbols"));
  _actions.insert(_SELECT_ALL, a);

  a = new QAction(QIcon(unselect_all_xpm), tr("Unselect all symbols"), this);
  a->setToolTip(tr("Unselect all symbols"));
  a->setStatusTip(tr("Unselect all symbols"));
  _actions.insert(_UNSELECT_ALL, a);

  a = new QAction(QIcon(search_xpm), tr("Perform Search"), this);
  a->setToolTip(tr("Perform Search"));
  a->setStatusTip(tr("Perform Search"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(searchButtonPressed()));
  _actions.insert(_SEARCH, a);
}

void
SymbolSelectDialog::createTab ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(10);
  w->setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(0);
  vbox->addLayout(form);
  
  _exchanges = new QComboBox;
  _exchanges->setToolTip(tr("Select a specific exchange or % for all"));
  connect(_exchanges, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Exchange"), _exchanges);

  _search = new QLineEdit("%");
  _search->setToolTip(tr("Enter a partial search like %OOG% or * for all"));
  connect(_search, SIGNAL(textChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Symbol pattern"), _search);

  _symbolType = new QLineEdit("%");
  connect(_symbolType, SIGNAL(textChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Symbol Type"), _symbolType);

  _symbolName = new QLineEdit("%");
  connect(_symbolName, SIGNAL(textChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Symbol Name"), _symbolName);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(0);
  vbox->addLayout(hbox);

  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Search Results"));
  hbox->addWidget(gbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(5);
  gbox->setLayout(tvbox);

  QStringList l;
  l << tr("Symbol") << tr("Name");

  _searchList = new QTreeWidget;
  _searchList->setSortingEnabled(TRUE);
  _searchList->setRootIsDecorated(FALSE);
  _searchList->setHeaderLabels(l);
  _searchList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_searchList, SIGNAL(itemSelectionChanged()), this, SLOT(searchSelectionChanged()));
  tvbox->addWidget(_searchList);

  tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  hbox->addLayout(tvbox);
  tvbox->addStretch(1);

  QToolBar *tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  tvbox->addWidget(tb);

  tb->addAction(_actions.value(_SEARCH));
  tb->addAction(_actions.value(_ADD));
  tb->addAction(_actions.value(_DELETE));
  tb->addAction(_actions.value(_SELECT_ALL));
  connect(_actions.value(_SELECT_ALL), SIGNAL(triggered(bool)), _searchList, SLOT(selectAll()));
  tb->addAction(_actions.value(_UNSELECT_ALL));
  connect(_actions.value(_UNSELECT_ALL), SIGNAL(triggered(bool)), _searchList, SLOT(clearSelection()));

  tvbox->addStretch(1);

  gbox = new QGroupBox;
  gbox->setTitle(tr("Symbols"));
  hbox->addWidget(gbox);

  tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(5);
  gbox->setLayout(tvbox);

  _symbolList = new QTreeWidget;
  _symbolList->setSortingEnabled(FALSE);
  _symbolList->setRootIsDecorated(FALSE);
  _symbolList->setHeaderLabels(l);
  _symbolList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_symbolList, SIGNAL(itemSelectionChanged()), this, SLOT(symbolSelectionChanged()));
  tvbox->addWidget(_symbolList);
  
  _tabs->addTab(w, tr("Settings"));
}

void
SymbolSelectDialog::searchSelectionChanged ()
{
  int status = 0;
  QList<QTreeWidgetItem *> sl = _searchList->selectedItems();
  if (sl.count())
    status = 1;

  _actions.value(_ADD)->setEnabled(status);
}

void
SymbolSelectDialog::symbolSelectionChanged ()
{
  int status = 0;
  QList<QTreeWidgetItem *> sl = _symbolList->selectedItems();
  if (sl.count())
    status = 1;

  _actions.value(_DELETE)->setEnabled(status);
  
  status = FALSE;
  if (_symbolList->topLevelItemCount())
    status = TRUE;

  setModified(status);
}

void
SymbolSelectDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
SymbolSelectDialog::addButtonPressed ()
{
  QList<QTreeWidgetItem *> sl = _searchList->selectedItems();

  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QTreeWidgetItem *item = sl.at(loop);

    QTreeWidgetItem *nitem = new QTreeWidgetItem(_symbolList);
    nitem->setText(0, item->text(0));
    nitem->setText(1, item->text(1));
  }

  for (loop = 0; loop < _symbolList->columnCount(); loop++)
    _symbolList->resizeColumnToContents(loop);

  symbolSelectionChanged();
}

void
SymbolSelectDialog::deleteButtonPressed ()
{
  QList<QTreeWidgetItem *> sl = _symbolList->selectedItems();

  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QTreeWidgetItem *item = sl.at(loop);
    delete item;
  }

  for (loop = 0; loop < _symbolList->columnCount(); loop++)
    _symbolList->resizeColumnToContents(loop);

  symbolSelectionChanged();
}

void
SymbolSelectDialog::searchButtonPressed ()
{
  if (! _symbolObject)
    return;

  ObjectCommand toc(QString("search"));
  toc.setValue(QString("exchange"), _exchanges->currentText());
  toc.setValue(QString("ticker"), _search->text());
  toc.setValue(QString("type"), _symbolType->text());
  toc.setValue(QString("name"), _symbolName->text());
  if (! _symbolObject->message(&toc))
  {
    qDebug() << "SymbolSelectDialog::searchButtonPressed: message error" << _symbolObject->plugin() << toc.command();
    return;
  }

  setSearchList(toc.getList(QString("list")));
}

void
SymbolSelectDialog::loadExchanges ()
{
  if (! _symbolObject)
    return;
  
  ObjectCommand toc(QString("exchanges"));
  if (! _symbolObject->message(&toc))
  {
    qDebug() << "SymbolSelectDialog::loadExchanges: message error" << _symbolObject->plugin() << toc.command();
    return;
  }

  QStringList l = toc.getList(QString("names"));
  l.prepend(QString("%"));
  
  _exchanges->clear();
  _exchanges->addItems(l);
  _exchanges->setCurrentIndex(0);
}

void
SymbolSelectDialog::setSettings (QStringList symbols, QString exchange, QString ticker,
                                 QString type, QString name)
{
  if (! exchange.isEmpty())
    _exchanges->setCurrentIndex(_exchanges->findText(exchange));
  
  if (! ticker.isEmpty())
    _search->setText(ticker);
  
  if (! type.isEmpty())
    _symbolType->setText(type);
  
  if (! name.isEmpty())
    _symbolName->setText(name);
  
  setSymbolList(symbols);
  
  setModified(FALSE);
}

void
SymbolSelectDialog::settings (QStringList &symbols, QString &exchange, QString &ticker,
                              QString &type, QString &name)
{
  symbols.clear();
  
  for (int pos = 0; pos < _symbolList->topLevelItemCount(); pos++)
  {
    QTreeWidgetItem *i = _symbolList->topLevelItem(pos);
    symbols << i->text(0);
  }
  
  exchange = _exchanges->currentText();
  ticker = _search->text();
  type = _symbolType->text();
  name = _symbolName->text();
}

void
SymbolSelectDialog::setSearchList (QStringList l)
{
  if (! _symbolObject)
    return;

  ObjectCommand toc(QString("info"));
  toc.setValue(QString("names"), l);
  if (! _symbolObject->message(&toc))
  {
    qDebug() << "SymbolSelectDialog::setSearchList: message error" << _symbolObject->plugin() << toc.command();
    return;
  }
  
  _searchList->clear();

  QHashIterator<QString, Data> it(toc.getDatas());
  while (it.hasNext())
  {
    it.next();
    Data d = it.value();
    
    QTreeWidgetItem *item = new QTreeWidgetItem(_searchList);
    item->setText(0, it.key());
    item->setText(1, d.value(QString("name")).toString());
  }
  
  for (int pos = 0; pos < _searchList->columnCount(); pos++)
    _searchList->resizeColumnToContents(pos);
}

void
SymbolSelectDialog::setSymbolList (QStringList l)
{
  if (! _symbolObject)
    return;

  ObjectCommand toc(QString("info"));
  toc.setValue(QString("names"), l);
  if (! _symbolObject->message(&toc))
  {
    qDebug() << "SymbolSelectDialog::setSymbolList: message error" << _symbolObject->plugin() << toc.command();
    return;
  }
  
  _symbolList->clear();

  QHashIterator<QString, Data> it(toc.getDatas());
  while (it.hasNext())
  {
    it.next();
    Data d = it.value();
    
    QTreeWidgetItem *item = new QTreeWidgetItem(_symbolList);
    item->setText(0, it.key());
    item->setText(1, d.value(QString("name")).toString());
  }
  
  for (int pos = 0; pos < _symbolList->columnCount(); pos++)
    _symbolList->resizeColumnToContents(pos);
}

void
SymbolSelectDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
SymbolSelectDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}
