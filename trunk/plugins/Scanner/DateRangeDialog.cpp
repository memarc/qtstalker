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

#include "DateRangeDialog.h"

#include <QtDebug>

DateRangeDialog::DateRangeDialog () : Dialog (0, tr("Date Range Override"))
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("IndicatorPlot");
  tl << QString("settings") << QString("date_range_dialog");
  _settingsPath = tl.join("/");
  
  createTab();
  
  loadSettings();
}

DateRangeDialog::~DateRangeDialog ()
{
  saveSettings();
}

void
DateRangeDialog::createTab ()
{
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  _startDate = new QDateTimeEdit;
  connect(_startDate, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(modified()));
  form->addRow(tr("Start Date"), _startDate);

  _endDate = new QDateTimeEdit;
  connect(_endDate, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(modified()));
  form->addRow(tr("End Date"), _endDate);
  
  _tabs->addTab(w, tr("Settings"));
}

void
DateRangeDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
DateRangeDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  
  _startDate->setDateTime(settings.value(QString("start_date")).toDateTime());
  _endDate->setDateTime(settings.value(QString("end_date")).toDateTime());
  
  Dialog::loadSettings(settings);
}

void
DateRangeDialog::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  
  settings.setValue(QString("start_date"), _startDate->dateTime());
  settings.setValue(QString("end_date"), _endDate->dateTime());
  
  Dialog::saveSettings(settings);
}

void
DateRangeDialog::setSettings (QDateTime sd, QDateTime ed)
{
  _startDate->setDateTime(sd);
  _endDate->setDateTime(ed);
  setModified(FALSE);
}

void
DateRangeDialog::settings (QDateTime &sd, QDateTime &ed)
{
  sd = _startDate->dateTime();
  ed = _endDate->dateTime();
}
