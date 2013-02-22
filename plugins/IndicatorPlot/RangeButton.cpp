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

#include "RangeButton.h"
#include "DateRangeDialog.h"

#include <QtDebug>


RangeButton::RangeButton (QString sp)
{
  _settingsPath = sp;
  
  _list << tr("OR"); // 0
  _list << tr("*"); // 1
  _list << tr("1D"); // 2
  _list << tr("1W"); // 3
  _list << tr("1M"); // 4
  _list << tr("3M"); // 5
  _list << tr("6M"); // 6
  _list << tr("1Y"); // 7
  _list << tr("2Y"); // 8
  _list << tr("5Y"); // 9
  _list << tr("10Y"); // 10
  _list << tr("25Y"); // 11
  _list << tr("50Y"); // 12

  _startDate = QDateTime::currentDateTime();
  _endDate = _startDate;
  
  createGUI();
  
  loadSettings();
}

RangeButton::~RangeButton ()
{
  saveSettings();
}

void
RangeButton::createGUI ()
{
  QFont font;
  font.setPointSize(9);
  setFont(font);

  setPopupMode(QToolButton::InstantPopup);
  setToolTip(tr("Date Range"));
  setStatusTip(tr("Date Range"));
  setMaximumSize(QSize(25, 25));

  QMenu *menu = new QMenu(this);
  connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(rangeChanged(QAction *)));
  setMenu(menu);
  
  _group = new QActionGroup(this);

  for (int pos = 0; pos < _list.size(); pos++)
  {
    QAction *a = menu->addAction(_list.at(pos));
    a->setCheckable(TRUE);
    _group->addAction(a);
    _actions.insert(_list.at(pos), a);
  }
}

void
RangeButton::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("range_button"));
  
  QAction *a = _actions.value(settings.value(QString("range")).toString());
  if (! a)
    a = _actions.value(QString("1Y"));
  
  a->setChecked(TRUE);
  rangeChanged(a);
}

void
RangeButton::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("range_button"));
  settings.setValue(QString("range"), _group->checkedAction()->text());
  settings.sync();  
}

void
RangeButton::rangeChanged (QAction *d)
{
  switch (_list.indexOf(d->text()))
  {
    case 0: // override
    {
      DateRangeDialog *dialog = new DateRangeDialog;
      dialog->setSettings(_startDate, _endDate);
      connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dateRangeDialog(void *)));
      dialog->show();
      setText(d->text());
      return;
      break;
    }
    case 1: // all
      _startDate = QDateTime::fromString("1900-01-01 00:00:00", Qt::ISODate);
      break;
    case 2: // 1D
      _startDate = QDateTime::currentDateTime().addDays(-1);
      break;
    case 3: // 1W
      _startDate = QDateTime::currentDateTime().addDays(-7);
      break;
    case 4: // 1M
      _startDate = QDateTime::currentDateTime().addMonths(-1);
      break;
    case 5: // 3M
      _startDate = QDateTime::currentDateTime().addMonths(-3);
      break;
    case 6: // 6M
      _startDate = QDateTime::currentDateTime().addMonths(-6);
      break;
    case 7: // 1Y
      _startDate = QDateTime::currentDateTime().addYears(-1);
      break;
    case 8: // 2Y
      _startDate = QDateTime::currentDateTime().addYears(-2);
      break;
    case 9: // 5Y
      _startDate = QDateTime::currentDateTime().addYears(-5);
      break;
    case 10: // 10Y
      _startDate = QDateTime::currentDateTime().addYears(-10);
      break;
    case 11: // 25Y
      _startDate = QDateTime::currentDateTime().addYears(-25);
      break;
    case 12: // 50Y
      _startDate = QDateTime::currentDateTime().addYears(-50);
      break;
    default:
      return;
      break;
  }

  _endDate = QDateTime::currentDateTime();
  
  setText(d->text());
  
  emit signalRange();
}

QDateTime
RangeButton::startDate ()
{
  return _startDate;
}

QDateTime
RangeButton::endDate ()
{
  return _endDate;
}

void
RangeButton::dateRangeDialog (void *dialog)
{
  DateRangeDialog *d = (DateRangeDialog *) dialog;
  d->settings(_startDate, _endDate);
  emit signalRange();
}
