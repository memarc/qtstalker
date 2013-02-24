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

#include "DateRangeButtonWidget.h"
#include "DateRangeButtonDialog.h"
#include "../pics/date.xpm"

#include <QtDebug>


DateRangeButtonWidget::DateRangeButtonWidget (QString profile)
{
  _profile = profile;
  
  createGUI();
}

DateRangeButtonWidget::~DateRangeButtonWidget ()
{
}

void
DateRangeButtonWidget::createGUI ()
{
  QFont font;
  font.setPointSize(9);
  setFont(font);

  setToolTip(tr("Date Range"));
  setStatusTip(tr("Date Range"));
  setMaximumSize(QSize(25, 25));
  
  _popup = new PopupWidget(this);
  setMenu(_popup);
  connect(this, SIGNAL(clicked()), this, SLOT(showPopup()));
  connect(_popup, SIGNAL(aboutToHide()), this, SLOT(popupChanged()));
}

void
DateRangeButtonWidget::setSettings (QStringList l, QString range, QDateTime sd, QDateTime ed, bool cus)
{
  _popup->setSettings(l, range, sd, ed, cus);
  
  if (cus)
  {
    setText(QString());
    setIcon(QIcon(date_xpm));
  }
  else
  {
    setIcon(QIcon());
    setText(range);
  }
}

QString
DateRangeButtonWidget::range ()
{
  return _popup->range();
}

QDateTime
DateRangeButtonWidget::startDate ()
{
  return _popup->startDate();
}

QDateTime
DateRangeButtonWidget::endDate ()
{
  return _popup->endDate();
}

bool
DateRangeButtonWidget::custom ()
{
  return _popup->custom();
}

void
DateRangeButtonWidget::showPopup ()
{
  _popup->exec(QCursor::pos());
}

void
DateRangeButtonWidget::popupChanged ()
{
  if (! _popup->modified())
    return;
  
  if (_popup->custom())
  {
    setText(QString());
    setIcon(QIcon(date_xpm));
  }
  else
  {
    setIcon(QIcon());
    setText(_popup->range());
  }
  
  emit signalRange();
}
