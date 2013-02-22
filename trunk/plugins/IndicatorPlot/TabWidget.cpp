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

#include "TabWidget.h"
#include "TabBar.h"

#include <QtDebug>

TabWidget::TabWidget (QWidget *parent, QString profile, int row) : QTabWidget (parent)
{
  _profile = profile;
  _row = row;

  QTabBar* tb = new TabBar;
  connect(tb, SIGNAL(tabMoveRequested(int, int)), this, SLOT(moveTab(int, int)));
  connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
  connect(tb, SIGNAL(signalMoveRow(QString, int, int)), this, SLOT(moveIndicator(QString, int, int)));
  connect(tb, SIGNAL(signalRemove(QString, int)), this, SLOT(removeIndicator(QString, int)));
  connect(tb, SIGNAL(signalEdit(QString)), this, SIGNAL(signalEditIndicator(QString)));
  setTabBar(tb);
}

void
TabWidget::moveTab (int fromIndex, int toIndex)
{
  QWidget *w = widget(fromIndex);
  QIcon icon = tabIcon(fromIndex);
  QString text = tabText(fromIndex);

  removeTab(fromIndex);
  insertTab(toIndex, w, icon, text);
  setCurrentIndex(toIndex);
}

void
TabWidget::tabChanged (int index)
{
  if (isVisible())
    emit signalTabChanged(this, index);
}

void
TabWidget::moveIndicator (QString name, int ocol, int nrow)
{
  if (_row == nrow)
    return; // ignore move if old and new are the same

  emit signalMoveIndicator(name, _row, ocol, nrow);
}

void
TabWidget::removeIndicator (QString name, int col)
{
  emit signalRemoveIndicator(name, _row, col);
}

