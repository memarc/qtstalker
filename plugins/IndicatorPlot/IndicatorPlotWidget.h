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

#ifndef PLUGIN_INDICATOR_PLOT_WIDGET_HPP
#define PLUGIN_INDICATOR_PLOT_WIDGET_HPP

#include <QtGui>

#include "Object.h"
#include "SidePanel.h"
#include "TabWidget.h"


class IndicatorPlotWidget : public QWidget
{
  Q_OBJECT
  
  signals:

  public:
    IndicatorPlotWidget (QMainWindow *, QString profile, QString name);
    ~IndicatorPlotWidget ();
    void createGUI ();
    void loadSettings ();
    Object * loadBars ();
    int refreshIndicator (Object *symbol, QString name);
    TabWidget * addTabs (int row);
    
  public slots:
    int addPlot (QString file, QString name, int row, int col);
    void scrollBarChanged (int);
    void refresh ();
    void addIndicator ();
    void addIndicator (QString file, QString name, int row);
    void indicatorObjectMessage (ObjectCommand oc);
    void symbolChanged (QString, QString, QDateTime, QDateTime);
    void saveSettings ();
    void tabChanged (QTabWidget *, int);
    void removePlot (QString, int row, int col);
    void editIndicator (QString name);
    void moveIndicator (QString name, int orow, int ocol, int nrow);

  private:
    QHash<QString, Object *> _indicators;
    QMap<int, TabWidget *> _tabs;
    QMainWindow *_mw;
    SidePanel *_sidePanel;
    QString _profile;
    QString _helpFile;
    QString _settingsPath;    
    QString _currentSymbol;
    QString _currentLength;
    QDateTime _startDate;
    QDateTime _endDate;
    QString _name;
    QSplitter *_splitter;
};

#endif
