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

#include <QtDebug>
#include <QSettings>
#include <QDir>

#include "ScannerWidget.h"
#include "ScannerObject.h"
#include "ScannerThread.h"
#include "Util.h"

#include "../../pics/stop.xpm"
#include "../../pics/help.xpm"
#include "../../pics/quit.xpm"
#include "../../pics/search.xpm"
#include "../../pics/add.xpm"
#include "../../pics/delete.xpm"
#include "../../pics/edit.xpm"
#include "../../pics/new.xpm"
#include "../../pics/save.xpm"


ScannerWidget::ScannerWidget (QMainWindow *mw, QString profile)
{
  setParent(mw);
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(saveSettings()));

  _helpFile = "main.html";
  _mw = mw;
  _profile = profile;
  _scanning = FALSE;
  _rangeButton = 0;
  _barLengthButton = 0;

  createActions();

  QStringList tl;
  tl << QString("OTA -") << QString("Scanner") << QString("(") << _profile << QString(")");
  _mw->setWindowTitle(tl.join(" "));

  tl.clear();
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Scanner") << QString("profile") << _profile << QString("settings");
  _settingsPath = tl.join("/");

  tl.clear();
  tl << dir.absolutePath() << QString("OTA") << QString("Scanner") << QString("profile") << _profile << QString("indicator");
  _indicatorPath = tl.join("/");

  Util util;
  _symbolObject = util.object(QString("Symbol"), QString(), QString());
  if (_symbolObject)
    connect(_symbolObject, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(symbolObjectMessage(ObjectCommand)));

  _rangeButton = util.object(QString("DateRangeButton"), _profile, QString());

  _barLengthButton = util.object(QString("BarLengthButton"), _profile, QString());

  createGUI();
  
  loadSettings();

  indicatorItemClicked(0, 0);
  symbolItemClicked(0);
}

ScannerWidget::~ScannerWidget ()
{
  if (_symbolObject)
    delete _symbolObject;
  
  if (_rangeButton)
    delete _rangeButton;
  
  if (_barLengthButton)
    delete _barLengthButton;
  
  qDeleteAll(_objects);
}

void
ScannerWidget::createActions ()
{
  QAction *a = new QAction(QIcon(search_xpm), tr("Start Scan"), this);
  a->setToolTip(tr("Start Scan"));
  a->setStatusTip(tr("Start Scan"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(scan()));
  _actions.insert(_SCAN, a);
  
  a = new QAction(QIcon(stop_xpm), tr("Stop Scan"), this);
  a->setToolTip(tr("Stop Scan"));
  a->setStatusTip(tr("Stop Scan"));
  a->setEnabled(FALSE);
  connect(a, SIGNAL(triggered(bool)), this, SIGNAL(signalStop()));
  _actions.insert(_STOP, a);
  
  a = new QAction(QIcon(help_xpm), tr("Help"), this);
  a->setToolTip(tr("Help"));
  a->setStatusTip(tr("Help"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(help()));
  _actions.insert(_HELP, a);
  
  a = new QAction(QIcon(quit_xpm), tr("Quit"), this);
  a->setToolTip(tr("Quit"));
  a->setStatusTip(tr("Quit"));
  connect(a, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
  _actions.insert(_QUIT, a);
  
  a = new QAction(QIcon(new_xpm), tr("New Indicator"), this);
  a->setToolTip(tr("New Indicator"));
  a->setStatusTip(tr("New Indicator"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(newIndicator()));
  _actions.insert(_INDICATOR_NEW, a);
  
  a = new QAction(QIcon(edit_xpm), tr("Edit Indicator"), this);
  a->setToolTip(tr("Edit Indicator"));
  a->setStatusTip(tr("Edit Indicator"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(editIndicator()));
  _actions.insert(_INDICATOR_EDIT, a);
  
  a = new QAction(QIcon(delete_xpm), tr("Remove Indicator"), this);
  a->setToolTip(tr("Remove Indicator"));
  a->setStatusTip(tr("Remove Indicator"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(removeIndicator()));
  _actions.insert(_INDICATOR_REMOVE, a);
  
  a = new QAction(QIcon(add_xpm), tr("Add Symbols"), this);
  a->setToolTip(tr("Add Symbols"));
  a->setStatusTip(tr("Add Symbols"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(addSymbols()));
  _actions.insert(_SYMBOL_ADD, a);
  
  a = new QAction(QIcon(delete_xpm), tr("Remove Symbols"), this);
  a->setToolTip(tr("Remove Symbols"));
  a->setStatusTip(tr("Remove Symbols"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(removeSymbols()));
  _actions.insert(_SYMBOL_REMOVE, a);
  
  a = new QAction(QIcon(save_xpm), tr("Save Results As Group"), this);
  a->setToolTip(tr("Save Results As Group"));
  a->setStatusTip(tr("Save Results As Group"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(saveResults()));
  _actions.insert(_RESULT_SAVE_GROUP, a);
}

void
ScannerWidget::createGUI ()
{
  _mw->setCentralWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(10);
  setLayout(vbox);
  
  // indicator area
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Indicators"));
  vbox->addWidget(gbox);
  
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(0);
  hbox->setMargin(5);
  gbox->setLayout(hbox);

  QStringList tl;
  tl << tr("Indicator") << tr("Usage") << tr("Scan Step");
  
  _indicators = new QTreeWidget;
  _indicators->setHeaderLabels(tl);
  _indicators->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_indicators, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(indicatorItemClicked(QTreeWidgetItem *, int)));
  hbox->addWidget(_indicators);
  
  QToolBar *tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  tb->addAction(_actions.value(_INDICATOR_NEW));
  tb->addAction(_actions.value(_INDICATOR_EDIT));
  tb->addAction(_actions.value(_INDICATOR_REMOVE));
  hbox->addWidget(tb);
  
  // symbols
  hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(0);
  vbox->addLayout(hbox);
 
  gbox = new QGroupBox;
  gbox->setTitle(tr("Scan Symbols"));
  hbox->addWidget(gbox);
  
  QHBoxLayout *thbox = new QHBoxLayout;
  thbox->setSpacing(0);
  thbox->setMargin(5);
  gbox->setLayout(thbox);
  
  _symbols = new QListWidget;
  connect(_symbols, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(symbolItemClicked(QListWidgetItem *)));
  thbox->addWidget(_symbols);

  // toolbar
  tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  tb->addAction(_actions.value(_SYMBOL_ADD));
  tb->addAction(_actions.value(_SYMBOL_REMOVE));
  tb->addSeparator();
  if (_barLengthButton)
    tb->addWidget(_barLengthButton->widget());
  if (_rangeButton)
    tb->addWidget(_rangeButton->widget());
  thbox->addWidget(tb);
  
  // results
  gbox = new QGroupBox;
  gbox->setTitle(tr("Results"));
  hbox->addWidget(gbox);
  
  thbox = new QHBoxLayout;
  thbox->setSpacing(0);
  thbox->setMargin(5);
  gbox->setLayout(thbox);
  
  _results = new QListWidget;
  thbox->addWidget(_results);
  
  // toolbar
  tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  tb->addAction(_actions.value(_SCAN));
  tb->addAction(_actions.value(_STOP));
  tb->addSeparator();
  tb->addAction(_actions.value(_HELP));
  tb->addAction(_actions.value(_QUIT));
  thbox->addWidget(tb);
  
  // statusbar
  _statusBar = _mw->statusBar();
  _statusBar->showMessage(tr("Ready"), 2000);

  // progress bar  
  _progBar = new QProgressBar;
  _statusBar->addPermanentWidget(_progBar, 0);
  
  _mw->setUnifiedTitleAndToolBarOnMac(TRUE);
}

void
ScannerWidget::loadSettings ()
{
  QSettings *settings = new QSettings(_settingsPath, QSettings::NativeFormat);
  
  // app size;
  QSize sz = settings->value(QString("size"), QSize(500,300)).toSize();
  _mw->resize(sz);

  // app position
  QPoint p = settings->value(QString("pos"), QPoint(0, 0)).toPoint();
  _mw->move(p);
  
  _symbols->addItems(settings->value(QString("symbols")).toStringList());
  _results->addItems(settings->value(QString("results")).toStringList());
  
  // indicator column sizes
  _indicators->setColumnWidth(0, settings->value(QString("indicator_col_0"), 300).toInt());
  _indicators->setColumnWidth(1, settings->value(QString("indicator_col_2"), 100).toInt());

  QDir dir(_indicatorPath);
  QStringList tl = dir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::NoSort);
  for (int pos = 0; pos < tl.size(); pos++)
  {
    QString file = _indicatorPath + "/" + tl.at(pos);
    addIndicatorItem(file, tl.at(pos));
  }
  
  if (_rangeButton)
  {
    ObjectCommand toc(QString("load"));
    toc.setValue(QString("QSettings"), (void *) settings);
    _rangeButton->message(&toc);
  }
  
  if (_barLengthButton)
  {
    ObjectCommand toc(QString("load"));
    toc.setValue(QString("QSettings"), (void *) settings);
    _barLengthButton->message(&toc);
  }
  
  delete settings;
}

void
ScannerWidget::saveSettings()
{
  QSettings *settings = new QSettings(_settingsPath, QSettings::NativeFormat);
  settings->setValue(QString("size"), _mw->size());
  settings->setValue(QString("pos"), _mw->pos());
  
  // symbols
  QStringList tl;
  for (int pos = 0; pos < _symbols->count(); pos++)
    tl << _symbols->item(pos)->text();
  settings->setValue(QString("symbols"), tl);
  
  // results
  tl.clear();
  for (int pos = 0; pos < _results->count(); pos++)
    tl << _results->item(pos)->text();
  settings->setValue(QString("results"), tl);

  // indicator column sizes
  settings->setValue(QString("indicator_col_0"), _indicators->columnWidth(0));
  settings->setValue(QString("indicator_col_1"), _indicators->columnWidth(1));

  if (_rangeButton)
  {
    ObjectCommand toc(QString("save"));
    toc.setValue(QString("QSettings"), (void *) settings);
    _rangeButton->message(&toc);
  }

  if (_barLengthButton)
  {
    ObjectCommand toc(QString("save"));
    toc.setValue(QString("QSettings"), (void *) settings);
    _barLengthButton->message(&toc);
  }
  
  delete settings;
  
  saveIndicatorSettings();
}

void
ScannerWidget::saveIndicatorSettings ()
{
  for (int pos = 0; pos < _indicators->topLevelItemCount(); pos++)
  {
    QTreeWidgetItem *i = _indicators->topLevelItem(pos);

    QString file = _indicatorPath + "/" + i->text(0);
    
    QSettings settings(file, QSettings::NativeFormat);
    
    QComboBox *w = (QComboBox *) _indicators->itemWidget(i, 1);
    settings.setValue(QString("usage"), w->currentText());
    
    w = (QComboBox *) _indicators->itemWidget(i, 2);
    settings.setValue(QString("step"), w->currentText());
    
    settings.sync();
  }
}

void
ScannerWidget::help ()
{
/*  
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("bin") << QString("OTA -a Help");
  
  if (! QProcess::startDetached(tl.join(" ")))
    qDebug() << "OTA::help: error launching process" << tl;
*/
}

void
ScannerWidget::scan ()
{
  _scanning = TRUE;
  setEnabled(FALSE);
  
  _results->clear();
  
  buttonStatus();

  QStringList tl;
  for (int pos = 0; pos < _symbols->count(); pos++)
    tl << _symbols->item(pos)->text();

  saveIndicatorSettings();
  
  _progBar->setRange(0, tl.size());

  QDateTime sd, ed;
  if (_rangeButton)
  {
    ObjectCommand toc(QString("dates"));
    if (! _rangeButton->message(&toc))
    {
      done();
      return;
    }
    
    sd = toc.getDate(QString("start_date"));
    ed = toc.getDate(QString("end_date"));
  }
  
  QString length;
  if (_barLengthButton)
  {
    ObjectCommand toc(QString("length"));
    if (! _barLengthButton->message(&toc))
    {
      done();
      return;
    }
    
    length = toc.getString(QString("length"));
  }
  
  ScannerThread *thread = new ScannerThread(0, _profile, tl, length, sd, ed);
  connect(thread, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(threadMessage(ObjectCommand)));
  connect(this, SIGNAL(signalStop()), thread, SLOT(stop()));
  connect(thread, SIGNAL(signalDone()), this, SLOT(done()));
  connect(thread, SIGNAL(signalProgress(int)), _progBar, SLOT(setValue(int)));
  thread->start();  
}

void
ScannerWidget::buttonStatus ()
{
  if (_scanning)
  {
    _actions.value(_SCAN)->setEnabled(FALSE);
    _actions.value(_STOP)->setEnabled(TRUE);
    _actions.value(_HELP)->setEnabled(FALSE);
    _actions.value(_QUIT)->setEnabled(FALSE);
    return;
  }
  else
  {
    _actions.value(_STOP)->setEnabled(FALSE);
    _actions.value(_HELP)->setEnabled(TRUE);
    _actions.value(_QUIT)->setEnabled(TRUE);
  }
  
  int count = 0;
  if (_indicators->topLevelItemCount())
    count++;
  if (_symbols->count())
    count++;
  
  switch (count)
  {
    case 2:
      _actions.value(_SCAN)->setEnabled(TRUE);
      break;
    default:
      _actions.value(_SCAN)->setEnabled(FALSE);
      break;
  }
}

void
ScannerWidget::done ()
{
  _scanning = FALSE;
  _progBar->reset();
  setEnabled(TRUE);
  buttonStatus();
}

void
ScannerWidget::threadMessage (ObjectCommand d)
{
  _results->addItems(d.getList(QString("symbols")));
}

void
ScannerWidget::indicatorItemClicked (QTreeWidgetItem *i, int)
{
  bool status = FALSE;
  if (i)
    status = TRUE;
  
  _actions.value(_INDICATOR_EDIT)->setEnabled(status);
  _actions.value(_INDICATOR_REMOVE)->setEnabled(status);
  
  buttonStatus();
}

int
ScannerWidget::addIndicatorItem (QString file, QString name)
{
  Util util;
  Object *o = util.object(QString("Indicator"), _profile, name);
  if (! o)
  {
    qDebug() << "ScannerWidget::addIndicatorItem: invalid Indicator object";
    return 0;
  }
  else
    _objects.insert(name, o);
  
  ObjectCommand toc(QString("load"));
  toc.setValue(QString("file"), file);
  o->message(&toc);

  QSettings settings(file, QSettings::NativeFormat);
  QString tusage = settings.value(QString("usage"), QString("AND")).toString();
  QString tstep = settings.value(QString("step")).toString();
  
  // create item
  QTreeWidgetItem *i = new QTreeWidgetItem;
  _indicators->addTopLevelItem(i);
  i->setText(0, name);

  QStringList tl;
  tl << tr("AND") << tr("OR");
  QComboBox *usage = new QComboBox;
  usage->addItems(tl);
  usage->setCurrentIndex(usage->findText(tusage));
  _indicators->setItemWidget(i, 1, usage);
  
  QComboBox *steps = new QComboBox;
  _indicators->setItemWidget(i, 2, steps);
    
  // get plugin compare steps  
  toc.setCommand(QString("plugin_steps"));
  toc.setValue(QString("plugin"), QString("CompareValues"));
  if (! o->message(&toc))
    qDebug() << "ScannerWidget::addIndicatorItem: message error" << o->plugin() << toc.command();
  
  tl = toc.getList(QString("steps"));
  steps->addItems(tl);
  if (! tstep.isEmpty())
    steps->setCurrentIndex(tl.indexOf(tstep));
  
  return 1;
}

void
ScannerWidget::newIndicator ()
{
  bool ok;
  QString name = QInputDialog::getText(this,
                                       tr("OTA - Scanner New Indicator"),
                                       tr("Indicator name"),
                                       QLineEdit::Normal,
                                       tr("New Indicator"),
                                       &ok,
                                       0,
                                       0);
  
  if (name.isEmpty())
    return;
  
  if (_objects.contains(name))
  {
    QMessageBox msg;
    msg.setWindowTitle(tr("OTA - Scanner New Indicator Error"));
    msg.setText(tr("Duplicate indicator name."));
    msg.exec();
    return;
  }

  QString file = _indicatorPath + "/" + name;
  if (! addIndicatorItem(file, name))
    return;
  
  Object *o = _objects.value(name);
  if (! o)
    return;

  QStringList tl;
  tl << _indicatorPath << name;
  
  ObjectCommand toc(QString("dialog"));
  o->message(&toc);
}

void
ScannerWidget::editIndicator ()
{
  QList<QTreeWidgetItem *> il = _indicators->selectedItems();
  if (! il.size())
    return;
  
  QTreeWidgetItem *i = il.at(0);
  Object *o = _objects.value(i->text(0));
  if (! o)
    return;
  
  ObjectCommand toc(QString("dialog"));
  o->message(&toc);
}

void
ScannerWidget::removeIndicator ()
{
  QList<QTreeWidgetItem *> il = _indicators->selectedItems();
  
  for (int pos = 0; pos < il.size(); pos++)
  {
    QTreeWidgetItem *i = il.at(pos);
    
    Object *o = _objects.value(i->text(0));
    if (! o)
    {
      qDebug() << "ScannerWidget::removeIndicator: invalid" << i->text(0);
      continue;
    }

    QStringList tl;
    tl << _indicatorPath << i->text(0);
    QDir dir;
    if (! dir.remove(tl.join("/")))
    {
      qDebug() << "ScannerWidget::removeIndicator: remove error" << tl.join("/");
      continue;
    }
      
    delete o;
    _objects.remove(i->text(0));
    
    delete i;
  }
  
  indicatorItemClicked(0, 0);
}

void
ScannerWidget::symbolItemClicked (QListWidgetItem *i)
{
  bool status = FALSE;
  if (i)
    status = TRUE;
  
  _actions.value(_SYMBOL_REMOVE)->setEnabled(status);
  
  buttonStatus();
}

void
ScannerWidget::addSymbols ()
{
  if (! _symbolObject)
    return;
  
  ObjectCommand toc(QString("search_dialog"));
  _symbolObject->message(&toc);
}

void
ScannerWidget::symbolObjectMessage (ObjectCommand oc)
{
  if (oc.command() != QString("search"))
    return;

  _symbols->clear();
  
  QHashIterator<QString, Data> it(oc.getDatas());
  while (it.hasNext())
  {
    it.next();
    _symbols->addItem(it.key());
  }

  buttonStatus();
}

void
ScannerWidget::removeSymbols ()
{
  QList<QListWidgetItem *> il = _symbols->selectedItems();
  qDeleteAll(il);
  symbolItemClicked(0);
}

void
ScannerWidget::saveResults ()
{
  bool ok;
  QString name = QInputDialog::getText(this,
                                       tr("OTA - Scanner Save Results As Group"),
                                       tr("Group Name"),
                                       QLineEdit::Normal,
                                       tr("New Group"),
                                       &ok,
                                       0,
                                       0);
  
  if (name.isEmpty())
    return;
  
  Util util;
  Object *group = util.object(QString("Group"), _profile, QString());
  if (! group)
  {
    qDebug() << "ScannerWidget::saveResults: invalid Group object";
    return;
  }
  
  ObjectCommand toc(QString("groups"));
  if (! group->message(&toc))
  {
    qDebug() << "ScannerWidget::saveResults: message error" << group->plugin() << toc.command();
    delete group;
    return;
  }
  
  QStringList gl = toc.getList(QString("groups"));
  
  if (gl.indexOf(name) != -1)
  {
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("OTA - Scanner Save Group Warning"));
    msgBox.setText("Group already exists.");
    msgBox.setInformativeText("Replace group ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if (ret == QMessageBox::No)
    {
      delete group;
      return;
    }
  }
  
  group->setName(name);
}
