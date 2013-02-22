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

#include "IndicatorDialog.h"
#include "Util.h"
#include "IndicatorAddStepDialog.h"

#include "../../pics/delete.xpm"
#include "../../pics/indicator.xpm"
#include "../../pics/chart.xpm"
#include "../../pics/trend.xpm"

#include <QDebug>

IndicatorDialog::IndicatorDialog (QString file, QString name) : Dialog (0, name)
{
  _indicatorPath = file;
  
  QStringList tl;
  tl << QString("OTA -") << QString("Edit Indicator") << QString("(") << name << QString(")");
  setWindowTitle(tl.join(" "));
  
  tl.clear();
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Indicator") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  Util util;
  _indicatorList = util.plugins(QString("indicator"));
  _curveList = util.plugins(QString("curve"));
  _markerList = util.plugins(QString("marker"));

  createActions();
  createTab();
  loadSettings();
  loadIndicator();
  buttonStatus();
}

IndicatorDialog::~IndicatorDialog ()
{
  saveSettings();
}

void
IndicatorDialog::clear ()
{
  qDeleteAll(_objects);
  _objects.clear();
  _list->clear();
}

void
IndicatorDialog::createActions ()
{
  QAction *a = new QAction(QIcon(delete_xpm), tr("&Remove Step"), this);
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  a->setToolTip(tr("Remove Step"));
  a->setStatusTip(tr("Remove Step"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(removeStep()));
  _actions.insert(_STEP_REMOVE, a);
  
  a = new QAction(QIcon(indicator_xpm), tr("Insert Indicator"), this);
  a->setToolTip(tr("Insert Indicator"));
  a->setStatusTip(tr("Insert Indicator"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(insertIndicator()));
  _actions.insert(_INDICATOR_INSERT, a);
  
  a = new QAction(QIcon(chart_xpm), tr("Insert Curve"), this);
  a->setToolTip(tr("Insert Curve"));
  a->setStatusTip(tr("Insert Curve"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(insertCurve()));
  _actions.insert(_CURVE_INSERT, a);
  
  a = new QAction(QIcon(trend_xpm), tr("Insert Marker"), this);
  a->setToolTip(tr("Insert Marker"));
  a->setStatusTip(tr("Insert Marker"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(insertMarker()));
  _actions.insert(_MARKER_INSERT, a);
  
  a = new QAction(QIcon(chart_xpm), tr("Insert Plot"), this);
  a->setToolTip(tr("Insert Plot"));
  a->setStatusTip(tr("Insert Plot"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(insertPlot()));
  _actions.insert(_PLOT_INSERT, a);
}

void
IndicatorDialog::createTab ()
{
  QWidget *w = new QWidget;
  
  // main vbox
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(10);
  w->setLayout(vbox);
  
  QGroupBox *gbox = new QGroupBox(tr("Steps"));
  vbox->addWidget(gbox);
  
  QHBoxLayout *thbox = new QHBoxLayout;
  thbox->setSpacing(0);
  thbox->setMargin(5);
  gbox->setLayout(thbox);
  
  // symbol combo
  _list = new QListWidget;
  _list->setSortingEnabled(FALSE);
  _list->setDragDropMode(QAbstractItemView::InternalMove);
  _list->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(stepDoubleClicked(QListWidgetItem *)));
  connect(_list, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(buttonStatus()));
  thbox->addWidget(_list);
  
  // setup toolbar
  QToolBar *tb = new QToolBar(QString("main"));
  tb->setOrientation(Qt::Vertical);
  tb->addAction(_actions.value(_INDICATOR_INSERT));
  tb->addAction(_actions.value(_CURVE_INSERT));
  tb->addAction(_actions.value(_MARKER_INSERT));
  tb->addAction(_actions.value(_PLOT_INSERT));
  tb->addSeparator();
  tb->addAction(_actions.value(_STEP_REMOVE));
  thbox->addWidget(tb);

  _tabs->addTab(w, tr("Settings"));
}

void
IndicatorDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
IndicatorDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
IndicatorDialog::done ()
{
  saveIndicator();
  emit signalDone((void *) this);
  Dialog::done();
}

void
IndicatorDialog::buttonStatus ()
{
  if (! _list->count())
  {
    _actions.value(_STEP_REMOVE)->setEnabled(FALSE);
    _actions.value(_CURVE_INSERT)->setEnabled(TRUE);
    _actions.value(_INDICATOR_INSERT)->setEnabled(TRUE);
    _actions.value(_MARKER_INSERT)->setEnabled(TRUE);
    _actions.value(_PLOT_INSERT)->setEnabled(TRUE);
    return;
  }
  
  QListWidgetItem *i = _list->currentItem();
  if (i)
  {
    _actions.value(_CURVE_INSERT)->setEnabled(TRUE);
    _actions.value(_INDICATOR_INSERT)->setEnabled(TRUE);
    _actions.value(_MARKER_INSERT)->setEnabled(TRUE);
    _actions.value(_PLOT_INSERT)->setEnabled(TRUE);
    
    if (i->text() != QString("symbol"))
      _actions.value(_STEP_REMOVE)->setEnabled(TRUE);
    else
    _actions.value(_STEP_REMOVE)->setEnabled(FALSE);
  }
  else
  {
    _actions.value(_STEP_REMOVE)->setEnabled(FALSE);
    _actions.value(_CURVE_INSERT)->setEnabled(FALSE);
    _actions.value(_INDICATOR_INSERT)->setEnabled(FALSE);
    _actions.value(_MARKER_INSERT)->setEnabled(FALSE);
    _actions.value(_PLOT_INSERT)->setEnabled(FALSE);
  }
}

void
IndicatorDialog::stepDoubleClicked (QListWidgetItem *i)
{
  if (! i)
    return;
  
  Object *o = _objects.value(i->text());
  if (! o)
    return;
  
  QHash<QString, void *> objects;
  for (int pos = 0; pos < _list->currentRow(); pos++)
  {
    QListWidgetItem *item = _list->item(pos);
    Object *o = _objects.value(item->text());
    if (! o)
      continue;
    
    objects.insert(item->text(), (void *) o);
  }
  
  ObjectCommand toc(QString("dialog"));
  toc.setObjects(objects);
  o->message(&toc);
}

void
IndicatorDialog::insertIndicator ()
{
  QStringList wt;
  wt << QString("OTA") << "-" << tr("Insert Indicator Step");

  IndicatorAddStepDialog *dialog = new IndicatorAddStepDialog(_objects.keys());
  dialog->setWindowTitle(wt.join(" "));
  dialog->setSettings(_indicatorList);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(addStep2(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return;
}

void
IndicatorDialog::insertCurve ()
{
  QStringList wt;
  wt << QString("OTA") << "-" << tr("Insert Curve Step");

  IndicatorAddStepDialog *dialog = new IndicatorAddStepDialog(_objects.keys());
  dialog->setWindowTitle(wt.join(" "));
  dialog->setSettings(_curveList);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(addStep2(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return;
}

void
IndicatorDialog::insertMarker ()
{
  QStringList wt;
  wt << QString("OTA") << "-" << tr("Insert Marker Step");

  IndicatorAddStepDialog *dialog = new IndicatorAddStepDialog(_objects.keys());
  dialog->setWindowTitle(wt.join(" "));
  dialog->setSettings(_markerList);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(addStep2(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return;
}

void
IndicatorDialog::insertPlot ()
{
  QStringList wt;
  wt << QString("OTA") << "-" << tr("Insert Plot Step");

  IndicatorAddStepDialog *dialog = new IndicatorAddStepDialog(_objects.keys());
  dialog->setWindowTitle(wt.join(" "));
  dialog->setSettings(QStringList() << QString("Plot"));
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(addStep2(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return;
}

void
IndicatorDialog::addStep2 (void *dialog)
{
  IndicatorAddStepDialog *d = (IndicatorAddStepDialog *) dialog;
  QString name, plugin;
  d->settings(name, plugin);
  
  Util util;
  Object *o = util.object(plugin, QString(), name);
  if (! o)
  {
    qDebug() << "IndicatorDialog::addStep: invalid object" << plugin;
    return;
  }
  connect(o, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(modified()));

  if (o->type() == QString("marker"))
  {
    ObjectCommand toc(QString("set_read_only"));
    toc.setValue(QString("read_only"), TRUE);
    o->message(&toc);
  }

  Object *to = _objects.value(name);
  if (to)
    delete to;
  _objects.insert(name, o);

  QListWidgetItem *i = new QListWidgetItem;
  i->setText(name);
  
  if (! _list->count())
    _list->addItem(i);
  else
    _list->insertItem(_list->currentRow() + 1, i);
  _list->setCurrentItem(i);

  modified();
}

void
IndicatorDialog::removeStep ()
{
  QListWidgetItem *i = _list->currentItem();
  QString name = i->text();

  // do not let user remove symbol object
  if (name == QString("symbol"))
    return;
  
  Object *o = _objects.value(name);
  if (! o)
    return;
  
  QStringList mess;
  mess << tr("Are you sure you want to remove step") << "(" << name << ")" << tr("?");
  
  QStringList wt;
  wt << QString("OTA") << "-" << tr("Confirm Remove Step");

  QMessageBox msg;
  msg.setWindowTitle(wt.join(" "));
  msg.setText(mess.join(" "));
  msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msg.setDefaultButton(QMessageBox::No);
  int ret = msg.exec();
  if (ret == QMessageBox::No)
    return;
  
  delete o;
  _objects.remove(name);
  
  delete i;
  
  modified();
  
  buttonStatus();
}

int
IndicatorDialog::loadIndicator ()
{
  clear();

  QDir dir;
  if (! dir.exists(_indicatorPath))
  {
    // new indicator
    Util util;
    Object *symbol = util.object(QString("Symbol"), QString(), QString());
    if (! symbol)
    {
      qDebug() << "IndicatorDialog::loadIndicator: invalid Symbol object";
      return 0;
    }
    else
      connect(symbol, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(modified()));
  
    QString key("symbol");
    _objects.insert(key, symbol);
    _list->addItem(key);
    modified();
    return 1;
  }
  
  QSettings *settings = new QSettings(_indicatorPath, QSettings::NativeFormat);
  
  QStringList order = settings->value(QString("order")).toStringList();
  
  Util util;
  for (int pos = 0; pos < order.size(); pos++)
  {
    settings->beginGroup(order.at(pos));
    
    QString plugin = settings->value(QString("plugin")).toString();
    
    Object *o = util.object(plugin, QString(), order.at(pos));
    if (o)
    {
      ObjectCommand toc(QString("load"));
      toc.setValue(QString("QSettings"), (void *) settings);
      if (o->message(&toc))
      {
        _objects.insert(order.at(pos), o);
	_list->addItem(order.at(pos));
        connect(o, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(modified()));
      }
      else
      {
        qDebug() << "IndicatorDialog::loadIndicator: object message error" << o->name() << toc.command();
        delete o;
      }
    }
    else
      qDebug() << "IndicatorDialog::loadIndicator: invalid object" << order.at(pos);
    
    settings->endGroup();
  }

  delete settings;
  
  _okButton->setEnabled(FALSE);
  
  return 1;
}

int
IndicatorDialog::saveIndicator ()
{
  if (_name.isEmpty())
  {
    qDebug() << "IndicatorObject::save: invalid object name";
    return 0;
  }
  
  if (! _okButton->isEnabled())
    return 0;
  
  QSettings *settings = new QSettings(_indicatorPath, QSettings::NativeFormat);

  // remove old data and start fresh
  // leave General group alone due to other plugins may use it for local settings
  QStringList tl = settings->childGroups();
  for (int pos = 0; pos < tl.size(); pos++)
  {
    settings->beginGroup(tl.at(pos));
    settings->remove("");
    settings->endGroup();
  }

  QStringList order;
  for (int pos = 0; pos < _list->count(); pos++)
  {
    QListWidgetItem *i = _list->item(pos);

    order << i->text();
  
    Object *o = _objects.value(i->text());
    if (! o)
      continue;
    
    settings->beginGroup(i->text());
    
    ObjectCommand toc(QString("save"));
    toc.setValue(QString("QSettings"), (void *) settings);
    if (! o->message(&toc))
    {
      qDebug() << "IndicatorObject::save: object message error" << o->name() << toc.command();
      settings->endGroup();
      continue;
    }
    
    settings->endGroup();
  }
  
  settings->setValue(QString("order"), order);
  
  delete settings;  
  
  _okButton->setEnabled(FALSE);
  
  return 1;
}
