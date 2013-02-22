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

#ifndef PLUGIN_INDICATOR_PLOT_TAB_BAR_HPP
#define PLUGIN_INDICATOR_PLOT_TAB_BAR_HPP

#include <QtGui>

class TabBar : public QTabBar
{
  Q_OBJECT

  public:
    TabBar (QWidget * parent = 0);

  public slots:
    void remove ();
    void contextMenu ();
    void moveRow ();
    void edit ();

  protected:
    void mousePressEvent (QMouseEvent *event);
    void mouseMoveEvent (QMouseEvent *event);
    void dragEnterEvent (QDragEnterEvent *event);
//    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent (QDropEvent *event);

  private:
    QPoint m_dragStartPos;
    int m_dragCurrentIndex;
    QMenu *_menu;
    QPoint _tabMenu;

  signals:
    void tabMoveRequested(int fromIndex, int toIndex);
    void signalMoveRow(QString, int, int);
    void signalRemove(QString, int);
    void signalEdit(QString);
};

#endif
