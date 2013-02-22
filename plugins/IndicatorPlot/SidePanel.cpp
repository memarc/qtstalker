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

#include "SidePanel.h"

#include "../pics/group.xpm"
#include "../pics/chart.xpm"

#include <QDebug>
#include <QSettings>
#include <QShortcut>

SidePanel::SidePanel (QString settingsPath, QString profile)
{
  _settingsPath = settingsPath;
  _profile = profile;
  createGUI();
  loadSettings();
//  setMaximumWidth(200);
}

SidePanel::~SidePanel ()
{
  saveSettings();
}

void
SidePanel::createGUI ()
{
  setOrientation(Qt::Vertical);
  
  QWidget *w = new QWidget;
  addWidget(w);
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(0);
  w->setLayout(vbox);
  
  _tabs = new QTabWidget;
  _tabs->setTabPosition(QTabWidget::South);
  vbox->addWidget(_tabs, 1);
  
  // chart tab
  _chartPanel = new ChartPage(this, _settingsPath, _profile);
  _tabs->addTab(_chartPanel, QIcon(chart_xpm), QString());
  _tabs->setTabToolTip(0, tr("Charts"));
  connect(_chartPanel, SIGNAL(signalSymbol(QString)), this, SLOT(symbolChanged(QString)));

  // group tab
  _groupPanel = new GroupPage(this, _settingsPath);
  _tabs->addTab(_groupPanel, QIcon(group_xpm), QString());
  _tabs->setTabToolTip(1, tr("Groups"));
  connect(_groupPanel, SIGNAL(signalSymbol(QString)), this, SLOT(symbolChanged(QString)));
  connect(_chartPanel, SIGNAL(signalGroupAdd(QString)), _groupPanel, SLOT(loadGroups()));

  // create shortcut keys for the tabs
  // chart panel
  QShortcut *sc = new QShortcut(this);
  sc->setKey(QKeySequence(Qt::CTRL + Qt::Key_1));
  connect(sc, SIGNAL(activated()), this, SLOT(setChartPanelFocus()));

  // group panel
  sc = new QShortcut(this);
  sc->setKey(QKeySequence(Qt::CTRL + Qt::Key_2));
  connect(sc, SIGNAL(activated()), this, SLOT(setGroupPanelFocus()));
  
  // chart settings toolbar
//  int length = 16;
//  int width = 16;

  QToolBar *tb = new QToolBar;
//  tb->setIconSize(QSize(length, width));
  vbox->addWidget(tb);

  // configure button
  _configureButton = new ConfigureButton;
  connect(_configureButton, SIGNAL(signalAddIndicator()), this, SIGNAL(signalAddIndicator()));
  tb->addWidget(_configureButton);

  // chart toggles toolbar
//  tb = new QToolBar;
//  tb->setIconSize(QSize(length, width));
//  vbox->addWidget(tb);

  // create bar length button
  _barLengthButton = new BarLengthButton(_settingsPath);
  connect(_barLengthButton, SIGNAL(signalLength()), this, SLOT(sendSymbolSignal()));
  tb->addWidget(_barLengthButton);

  // date range controls
  _rangeButton = new RangeButton(_settingsPath);
  connect(_rangeButton, SIGNAL(signalRange()), this, SLOT(sendSymbolSignal()));
  tb->addWidget(_rangeButton);

  // create recent charts combobox
  _recentCharts = new RecentCharts(_settingsPath);
  connect(_chartPanel, SIGNAL(signalSymbol(QString)), _recentCharts, SLOT(addSymbol(QString)));
  connect(_recentCharts, SIGNAL(signalSymbol(QString)), this, SLOT(symbolChanged(QString)));
  tb->addWidget(_recentCharts);

  // slider
  _slider = new QSlider;
  _slider->setRange(0, 0);
  _slider->setValue(0);
  _slider->setOrientation(Qt::Horizontal);
  _slider->setToolTip(tr("Scroll Chart"));
  _slider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
  connect(_slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalSlider(int)));
  vbox->addWidget(_slider);

  // create shortcut key for the slider
  sc = new QShortcut(this);
  sc->setKey(QKeySequence(Qt::CTRL + Qt::Key_L));
  connect(sc, SIGNAL(activated()), _slider, SLOT(setFocus()));

  // info panel
  _info = new QTextEdit;
  _info->setReadOnly(TRUE);
  addWidget(_info);
}

void
SidePanel::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("side_panel"));
  
  QStringList tl = settings.value(QString("splitter")).toStringList();
  QList<int> sizes;
  if (! tl.size())
    tl << QString("200") << QString("100");
  for (int pos = 0; pos < tl.size(); pos++)
    sizes << tl.at(pos).toInt();
  setSizes(sizes);  

  _tabs->setCurrentIndex(settings.value("tab", 0).toInt());
}

void
SidePanel::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("side_panel"));
  
  // save splitter sizes
  QStringList l;
  QList<int> sl = sizes();
  for (int pos = 0; pos < sl.size(); pos++)
    l << QString::number(sl.at(pos));
  settings.setValue(QString("splitter"), l);
  
  settings.setValue("tab", _tabs->currentIndex());
  
  settings.sync();
}

void
SidePanel::setChartPanelFocus ()
{
  _tabs->setCurrentIndex(0);
//  g_chartPanel->list()->setFocus();
}

void
SidePanel::setGroupPanelFocus ()
{
  _tabs->setCurrentIndex(1);
//  g_groupPanel->list()->setFocus();
}

void
SidePanel::setSlider (int size)
{
  _slider->blockSignals(TRUE);
  _slider->setRange(0, size - 1);
  _slider->setPageStep(size / 4);
  _slider->setValue(size - 1);
  _slider->blockSignals(FALSE);
}

void
SidePanel::setSliderValue (int d)
{
  _slider->setValue(d);
}

void
SidePanel::setInfo (Data d)
{
  // list bar values first
  QStringList tl;
  tl << "D" << "T" << "O" << "H" << "L" << "C" << "V";

  QString s;
  QStringList vl;
  for (int pos = 0; pos < tl.size(); pos++)
  {
    if (! d.contains(tl.at(pos)))
      continue;
    
    vl << tl.at(pos) + " " + d.value(tl.at(pos)).toString();
    d.remove(tl.at(pos));
  }

  tl = d.keys();
  qSort(tl);

  for (int pos = 0; pos < tl.size(); pos++)
  {
    QString s = d.value(tl.at(pos)).toString();
    if (s.isEmpty())
      continue;
    
    vl << tl.at(pos) + " " + s;
  }

  _info->setText(vl.join("\n"));
}

void
SidePanel::sendSymbolSignal ()
{
  emit signalSymbol(_currentSymbol,
                    _barLengthButton->length(),
		    _rangeButton->startDate(),
		    _rangeButton->endDate());
}

void
SidePanel::symbolChanged (QString d)
{
  _currentSymbol = d;
  sendSymbolSignal();
}

int
SidePanel::sliderValue ()
{
  return _slider->value();
}
