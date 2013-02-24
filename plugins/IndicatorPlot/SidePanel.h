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

#ifndef PLUGIN_INDICATOR_SIDE_PANEL_HPP
#define PLUGIN_INDICATOR_SIDE_PANEL_HPP

#include <QtGui>

#include "Object.h"
#include "Data.h"
#include "ChartPage.h"
#include "GroupPage.h"
#include "RecentCharts.h"
#include "ConfigureButton.h"

class SidePanel : public QSplitter
{
  Q_OBJECT

  signals:
    void signalSymbol (QString, QString, QDateTime, QDateTime);
    void signalSlider (int);
    void signalAddIndicator ();

  public:
    SidePanel (QString settingsPath, QString profile);
    ~SidePanel ();
    void createGUI ();

  public slots:
    void loadSettings ();
    void saveSettings ();
    void setChartPanelFocus ();
    void setGroupPanelFocus ();
    void setSlider (int);
    void setSliderValue (int);
    int sliderValue ();
    void setInfo (Data);
    void sendSymbolSignal ();
    void symbolChanged (QString);

  private:
    QTabWidget *_tabs;
    ChartPage *_chartPanel;
    GroupPage *_groupPanel;
    QSlider *_slider;
    QTextEdit *_info;
    RecentCharts *_recentCharts;
    Object *_barLengthButton;
    Object *_rangeButton;
    ConfigureButton *_configureButton;
    QString _settingsPath;
    QString _currentSymbol;
    QString _profile;
};

#endif
