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

#include "IndicatorPlotWidget.h"
#include "Util.h"
#include "AddIndicator.h"

#include <QDebug>
#include <QTimer>


IndicatorPlotWidget::IndicatorPlotWidget (QMainWindow *mw, QString profile, QString name)
{
  setParent(mw);

  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(saveSettings()));

  _helpFile = "main.html";
  _mw = mw;
  _profile = profile;
  _name = name;

  QStringList tl;
  tl << QString("OTA -") << QString("Indicator") << QString("(") << _profile << QString(")");
  _mw->setWindowTitle(tl.join(" "));

  tl.clear();
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("IndicatorPlot") << _profile << QString("settings") << QString("app");
  _settingsPath = tl.join("/");

  createGUI();

  loadSettings();

  // fix for messed up plot screen if we draw the plot before it has become visible
  // we load settings just after plot is visible with this delay
  QTimer::singleShot(500, this, SLOT(refresh()));
}

IndicatorPlotWidget::~IndicatorPlotWidget ()
{
  qDeleteAll(_indicators);
}

void
IndicatorPlotWidget::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(0);
  vbox->setMargin(0);
  setLayout(vbox);

  _splitter = new QSplitter(Qt::Vertical, this);
  _splitter->setOpaqueResize(FALSE);
  _splitter->setHandleWidth(1);
  _splitter->setChildrenCollapsible(FALSE);
  vbox->addWidget(_splitter);

  _mw->setCentralWidget(this);

  // side panel  
  _sidePanel = new SidePanel(_settingsPath, _profile);
  connect(_sidePanel, SIGNAL(signalSymbol(QString, QString, QDateTime, QDateTime)), this, SLOT(symbolChanged(QString, QString, QDateTime, QDateTime)));
  connect(_sidePanel, SIGNAL(signalSlider(int)), this, SLOT(scrollBarChanged(int)));
  connect(_sidePanel, SIGNAL(signalAddIndicator()), this, SLOT(addIndicator()));

  QDockWidget *dock = new QDockWidget(QString(), this);
  dock->setObjectName(QString("sidePanel"));
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setFeatures(QDockWidget::DockWidgetMovable);
  dock->setWidget(_sidePanel);
  _mw->addDockWidget(Qt::RightDockWidgetArea, dock);

  _mw->setUnifiedTitleAndToolBarOnMac(TRUE);
}

void
IndicatorPlotWidget::loadSettings ()
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("IndicatorPlot") << _profile << QString("indicator");
  dir.setPath(tl.join("/"));
  QStringList indicators = dir.entryList(QStringList(), QDir::Files | QDir::NoDotAndDotDot, QDir::NoSort);
  
  // load indicators
  for (int pos = 0; pos < indicators.size(); pos++)
  {
    QString file = dir.absolutePath() + "/" + indicators.at(pos);
    QSettings tsettings(file, QSettings::NativeFormat);
    addPlot(file,
            indicators.at(pos),
            tsettings.value(QString("row")).toInt(),
            tsettings.value(QString("col")).toInt());     
  }

  QSettings settings(_settingsPath, QSettings::NativeFormat);

  _mw->restoreGeometry(settings.value("main_window_geometry").toByteArray());
  _mw->restoreState(settings.value("main_window_state").toByteArray());

  // restore the size of the app
  QSize sz = settings.value("main_window_size", QSize(800,600)).toSize();
  _mw->resize(sz);

  // restore the position of the app
  QPoint p = settings.value("main_window_position", QPoint(0,0)).toPoint();
  _mw->move(p);
  
  _currentSymbol = settings.value(QString("symbol")).toString();
  _currentLength = settings.value(QString("length")).toString();
  _startDate = settings.value(QString("start_date")).toDateTime();
  _endDate = settings.value(QString("end_date")).toDateTime();
  _splitter->restoreState(settings.value("splitter").toByteArray());
}

void
IndicatorPlotWidget::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  
  settings.setValue("main_window_geometry", _mw->saveGeometry());
  settings.setValue("main_window_state", _mw->saveState());
  settings.setValue("main_window_size", _mw->size());
  settings.setValue("main_window_position", _mw->pos());
  settings.setValue(QString("symbol"), _currentSymbol);
  settings.setValue(QString("length"), _currentLength);
  settings.setValue(QString("start_date"), _startDate);
  settings.setValue(QString("end_date"), _endDate);
  settings.setValue("splitter", _splitter->saveState());

  // save indicator tab positions
  QMapIterator<int, TabWidget *> it(_tabs); 
  while (it.hasNext())
  {
    it.next();
    TabWidget *tabs = it.value();

    for (int pos = 0; pos < tabs->count(); pos++)
    {
      QString ts = tabs->tabText(pos);
      if (ts.isEmpty())
        continue;

      QStringList tl;
      QDir dir(QDir::homePath());
      tl << dir.absolutePath() << QString("OTA") << QString("IndicatorPlot") << _profile << QString("indicator") << ts;
      QString file = tl.join("/");
      if (! dir.exists(file))
        continue;

      QSettings tsettings(file, QSettings::NativeFormat);
      tsettings.setValue(QString("row"), it.key());
      tsettings.setValue(QString("col"), pos);
      tsettings.sync();
    }
  }

  settings.sync();
}

void
IndicatorPlotWidget::addIndicator ()
{
  AddIndicator *ai = new AddIndicator(_profile);
  connect(ai, SIGNAL(signalDone(QString, QString, int)), this, SLOT(addIndicator(QString, QString, int)));
  ai->run();
}

void
IndicatorPlotWidget::addIndicator (QString file, QString name, int row)
{
  if (! addPlot(file, name, row, -1))
    return;
  
  refresh();
}

int
IndicatorPlotWidget::addPlot (QString file, QString name, int row, int col)
{
  QString key("Indicator");
  Util util;
  Object *i = util.object(key, _profile, name);
  if (! i)
  {
    qDebug() << "IndicatorPlotWidget::addPlot: invalid object" << key;
    return 0;
  }
  else
  {
    connect(i, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(indicatorObjectMessage(ObjectCommand)));
  }

  ObjectCommand toc(QString("load"));
  toc.setValue(QString("file"), file);
  if (! i->message(&toc))
  {
    qDebug() << "IndicatorPlotWidget::addPlot: message error" << i->plugin() << toc.command();
    delete i;
    return 0;
  }

  // get plot widget
  QWidget *w = i->widget();
  if (! w)
  {
    qDebug() << "IndicatorPlotWidget::addPlot: invalid Plot widget";
    delete i;
    return 0;
  }

  TabWidget *tabs = addTabs(row);
  if (col == -1)
    tabs->addTab(w, name);
  else
    tabs->insertTab(col, w, name);

  _indicators.insert(name, i);

  return 1;
}

void
IndicatorPlotWidget::scrollBarChanged (int d)
{
  QHashIterator<QString, Object *> it(_indicators);
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();

    ObjectCommand toc(QString("set_index"));
    toc.setValue(QString("index"), d);
    if (! o->message(&toc))
    {
      qDebug() << "IndicatorPlotWidget::scrollBarChanged: message error" << o->plugin() << toc.command();
      continue;
    }
  }
}

void
IndicatorPlotWidget::refresh ()
{	
  if (! _indicators.size())
    return;

  // load bars
  Object *symbol = loadBars();
  if (! symbol)
    return;

  QHashIterator<QString, Object *> it(_indicators);
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();

    refreshIndicator(symbol, it.key());
    
    // load new markers
    ObjectCommand toc(QString("load_markers"));
    toc.setValue(QString("symbol"), _currentSymbol);
    o->message(&toc);
  }

  QString key("size");
  ObjectCommand toc(key);
  if (! symbol->message(&toc))
    qDebug() << "IndicatorPlotWidget::refresh: message error" << symbol->plugin() << key;
  
  int size = toc.getInt(key);
  
  _sidePanel->setSlider(size);

  int pos = size - ((this->width() - _sidePanel->width() - 75) / 8);
  if (pos < 0)
    pos = 0;
  
  _sidePanel->setSliderValue(pos);
  
  delete symbol;
  
  QStringList mess;
  mess << QString("OTA") << QString("-") << _profile << QString("-");
  mess << QString("(") << _currentSymbol << QString(")") << _currentLength;
  _mw->setWindowTitle(mess.join(" "));
}

int
IndicatorPlotWidget::refreshIndicator (Object *symbol, QString name)
{
  if (_currentSymbol.isEmpty())
    return 0;

  Object *o = _indicators.value(name);
  if (! o)
    return 0;

  QStringList tl = _currentSymbol.split(":");
  QString sym = tl.join("_");

qDebug() << "IndicatorPlotWidget::refreshIndicator: about to" << name << sym << _currentLength << symbol;
  
  ObjectCommand toc(QString("set_input"));
  toc.setValue(QString("symbol"), sym);
  toc.setValue(QString("length"), _currentLength);
  toc.setValue(QString("input"), (void *) symbol);
  if (! o->message(&toc))
  {
    qDebug() << "IndicatorPlotWidget::refreshIndicator: message error" << o->plugin() << toc.command();
    return 0;
  }
  
qDebug() << "IndicatorPlotWidget::refreshIndicator: OK";

  return 1;
}

Object *
IndicatorPlotWidget::loadBars ()
{
  if ( _currentSymbol.isEmpty())
    return 0;
  
  Util util;
  Object *symbol = util.object(QString("Symbol"), QString(), QString());
  if (! symbol)
  {
    qDebug() << "IndicatorPlotWidget::loadBars: invalid Symbol object";
    return 0;
  }
  
  QStringList tl = _currentSymbol.split(":");
  if (! tl.size() == 2)
  {
    qDebug() << "IndicatorPlotWidget::loadBars: invalid current symbol" << _currentSymbol;
    delete symbol;
    return 0;
  }
  
qDebug() << "IndicatorPlotWidget::loadBars: about to load symbol" << tl << _startDate << _endDate << _currentLength;

  ObjectCommand oc(QString("get"));
  oc.setValue(QString("exchange"), tl.at(0));
  oc.setValue(QString("ticker"), tl.at(1));
  oc.setValue(QString("start_date"), _startDate);
  oc.setValue(QString("end_date"), _endDate);
  oc.setValue(QString("length"), _currentLength);
  if (! symbol->message(&oc))
  {
    qDebug() << "IndicatorPlotWidget::loadBars: message error" << symbol->plugin() << oc.command();
    delete symbol;
    return 0;
  }

  oc.setCommand(QString("size"));
  symbol->message(&oc);
qDebug() << "IndicatorPlotWidget::loadBars: OK" << symbol->plugin() << oc.getInt(QString("size"));
  
  return symbol;
}

void
IndicatorPlotWidget::removePlot (QString name, int row, int col)
{
  Object *o = _indicators.value(name);
  if (! o)
  {
    qDebug() << "IndicatorPlotWidget::removePlot: plotData not found" << name;
    return;
  }

  // remove tab
  TabWidget *tabs = _tabs.value(row);
  if (! tabs)
  {
    qDebug() << "IndicatorPlotWidget::removePlot: tabs not found";
    return;
  }

  QWidget *w = tabs->widget(col);
  if (! w)
  {
    qDebug() << "IndicatorPlotWidget::removePlot: widget not found";
    return;
  }

  tabs->removeTab(col);
  delete w;

  if (! tabs->count())
  {
    delete tabs;
    _tabs.remove(row);
  }

  // delete indicator  
  delete o;
  _indicators.remove(name);

  // delete indicator file
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("IndicatorPlot") << _profile << QString("indicator") << name;
  QString file = tl.join("/");
  if (! dir.remove(file))
    qDebug() << "IndicatorPlotWidget::removePlot: remove error" << file;
}

void
IndicatorPlotWidget::indicatorObjectMessage (ObjectCommand oc)
{
  if (oc.command() == QString("info"))
    _sidePanel->setInfo(oc.getData(QString("info")));
}

void
IndicatorPlotWidget::symbolChanged (QString symbol, QString length, QDateTime sd, QDateTime ed)
{
  _currentSymbol = symbol;
  _currentLength = length;
  _startDate = sd;
  _endDate = ed;
qDebug() << "IndicatorPlotWidget::symbolChanged:" << symbol << length << sd << ed;
  refresh();
}

void
IndicatorPlotWidget::tabChanged (QTabWidget *tabs, int index)
{
  QString name = tabs->tabText(index);
  Object *o = _indicators.value(name);
  if (! o)
    return;

  ObjectCommand toc(QString("set_index"));
  toc.setValue(QString("index"), _sidePanel->sliderValue());
  if (! o->message(&toc))
    qDebug() << "IndicatorPlotWidget::tabChanged: message error" << o->plugin() << toc.command();
}

void
IndicatorPlotWidget::editIndicator (QString name)
{
  Object *o = _indicators.value(name);
  if (! o)
    return;

  ObjectCommand toc(QString("dialog"));
  o->message(&toc);
}

void
IndicatorPlotWidget::moveIndicator (QString name, int orow, int ocol, int nrow)
{
  Object *o = _indicators.value(name);
  if (! o)
    return;

  TabWidget *otab = _tabs.value(orow);
  if (! otab)
    return;

  QWidget *w = otab->widget(ocol);
  if (! w)
    return;

  TabWidget *ntab = addTabs(nrow);

  // remove otab
  otab->removeTab(ocol);
  if (! otab->count())
  {
    delete otab;
    _tabs.remove(orow);
  }

  ntab->addTab(w, name);
}

TabWidget *
IndicatorPlotWidget::addTabs (int row)
{
  TabWidget *tabs = _tabs.value(row);
  if (! tabs)
  {
    tabs = new TabWidget(this, _profile, row);
    connect(tabs, SIGNAL(signalTabChanged(QTabWidget *, int)), this, SLOT(tabChanged(QTabWidget *, int)));
    connect(tabs, SIGNAL(signalEditIndicator(QString)), this, SLOT(editIndicator(QString)));
    connect(tabs, SIGNAL(signalRemoveIndicator(QString, int, int)), this, SLOT(removePlot(QString, int, int)));
    connect(tabs, SIGNAL(signalMoveIndicator(QString, int, int, int)), this, SLOT(moveIndicator(QString, int, int, int)));
    _splitter->insertWidget(row - 1, tabs);
    _tabs.insert(row, tabs);
  }

  return tabs;
}

