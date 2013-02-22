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

#include "DateRangeControl.h"

#include <QDebug>
#include <QSettings>

DateRangeControl::DateRangeControl ()
{
  QFont font;
  font.setPointSize(9);
  setFont(font);

//  DateRange dr;
//  _lengthList = dr.list();

  int loop = 0;
  for (; loop < _lengthList.count(); loop++)
  {
    QString s;
  //  dr.dateRangeText((DateRange::Range) loop, s);
    _shortList << s;
  }

  createMenu();

  setPopupMode(QToolButton::InstantPopup);
  setToolTip(tr("Date Range"));
  setStatusTip(tr("Date Range"));
}

void
DateRangeControl::createMenu ()
{
  _menu = new QMenu(this);
  _menu->setTitle(tr("Date Range"));
  connect(_menu, SIGNAL(triggered(QAction *)), this, SLOT(rangeChanged(QAction *)));
  setMenu(_menu);

//  QSettings settings(g_localSettings);
//  _dateRange = settings.value("date_range", 5).toInt();
//  settings.setValue("date_range", _dateRange);
//  settings.sync();

  QActionGroup *group = new QActionGroup(this);

  int loop = 0;
  for (; loop < _lengthList.count(); loop++)
  {
    QAction *a = _menu->addAction(_lengthList.at(loop));
    a->setCheckable(TRUE);
    group->addAction(a);

    if (loop == _dateRange)
    {
      a->setChecked(TRUE);
      setText(_shortList.at(loop));
    }
  }
}

void
DateRangeControl::rangeChanged (QAction *d)
{
  _dateRange = _lengthList.indexOf(d->text());

//  QSettings settings(g_localSettings);
//  settings.setValue("date_range", _dateRange);
//  settings.sync();

  setText(_shortList.at(_dateRange));

  // reload chart
//  if (! g_currentSymbol)
//    return;

//  ChartLoad *cl = new ChartLoad(this, g_currentSymbol->symbolFull());
//  cl->run();

//  emit signalDateRangeChanged();
}

int
DateRangeControl::dateRange ()
{
  return _dateRange;
}
