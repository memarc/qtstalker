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

#include "DataWindowButton.h"
#include "../pics/datawindow.xpm"

#include <QDebug>

DataWindowButton::DataWindowButton ()
{
  setIcon(QIcon(datawindow_xpm));
  setStatusTip(tr("Display Data Window"));
  setToolTip(tr("Display Data Window"));
  setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));

  connect(this, SIGNAL(clicked()), this, SLOT(dialog()));
}

void
DataWindowButton::dialog ()
{
/*  
  if (! g_currentSymbol)
    return;

  DataWindow *dw = new DataWindow(0);

  QList<QString> l = g_plotGroup->plots();

  int loop = 0;
  for (; loop < l.count(); loop++)
    dw->setPlot(g_plotGroup->plot(l.at(loop)));

  dw->scrollToBottom();
  dw->show();
*/
}
