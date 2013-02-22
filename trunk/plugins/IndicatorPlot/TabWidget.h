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

#ifndef PLUGIN_INDICATOR_PLOT_TAB_WIDGET_HPP
#define PLUGIN_INDICATOR_PLOT_TAB_WIDGET_HPP

#include <QtGui>

class TabWidget : public QTabWidget
{
  Q_OBJECT

  signals:
    void signalTabChanged (QTabWidget *, int);
    void signalEditIndicator (QString);
    void signalRemoveIndicator (QString, int, int);
    void signalMoveIndicator (QString, int, int, int);

  public:
    TabWidget (QWidget *parent, QString profile, int row);

  public slots:
    void moveTab (int fromIndex, int toIndex);
    void tabChanged (int index);
    void moveIndicator (QString, int ocol, int nrow);
    void removeIndicator (QString, int col);

  private:
    QString _profile;
    int _row;
};

#endif

