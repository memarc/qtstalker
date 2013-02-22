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

#include "MarkerTLineDialog.h"
#include "Object.h"

#include <QDebug>

MarkerTLineDialog::MarkerTLineDialog (QHash<QString, void *> l, QString name) : Dialog (0, name)
{
  QStringList tl;
  tl << QString("OTA") << QString("-") << tr("Editing Marker TLine");
  setWindowTitle(tl.join(" "));

  tl.clear();
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("MarkerTLine") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createTab(l);
  loadSettings();
}

MarkerTLineDialog::~MarkerTLineDialog ()
{
  saveSettings();
}

void
MarkerTLineDialog::createTab (QHash<QString, void *> l)
{
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(10);
  w->setLayout(form);

  // plot
  _plot = new QComboBox;
  connect(_plot, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));

  QStringList tl;
  QHashIterator<QString, void *> it(l);
  while (it.hasNext())
  {
    it.next();
    Object *o = (Object *) it.value();
    if (o->plugin() == QString("Plot"))
      tl << it.key();
  }
  if (tl.size())
  {
    _plot->addItems(tl);
    form->addRow(tr("Plot"), _plot);
  }

  // color
  _color = new ColorButton(this, QColor(Qt::red));
  connect(_color, SIGNAL(valueChanged()), this, SLOT(modified()));
  form->addRow(tr("Color"), _color);
  
  // start date
  _startDate = new QDateTimeEdit;
  _startDate->setCalendarPopup(TRUE);
  connect(_startDate, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(modified()));
  form->addRow(tr("Start Date"), _startDate);
  
  // end date
  _endDate = new QDateTimeEdit;
  _endDate->setCalendarPopup(TRUE);
  connect(_endDate, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(modified()));
  form->addRow(tr("End Date"), _endDate);
  
  // start price
  _startPrice = new QDoubleSpinBox;
  _startPrice->setRange(-99999999.0, 99999999.0);
  _startPrice->setDecimals(4);
  connect(_startPrice, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Start Price"), _startPrice);
  
  // end price
  _endPrice = new QDoubleSpinBox;
  _endPrice->setRange(-99999999.0, 99999999.0);
  _endPrice->setDecimals(4);
  connect(_endPrice, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("End Price"), _endPrice);

  _extend = new QCheckBox;
  connect(_extend, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Extend"), _extend);

  _tabs->addTab(w, tr("Settings"));
}

void
MarkerTLineDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
MarkerTLineDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
MarkerTLineDialog::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
MarkerTLineDialog::setSettings (QColor color, QDateTime sd, QDateTime ed, double sp, double ep, bool ex, QString po)
{
  _color->setColor(color);
  _startDate->setDateTime(sd);
  _endDate->setDateTime(ed);
  _startPrice->setValue(sp);
  _endPrice->setValue(ep);
  _extend->setChecked(ex);

  int pos = _plot->findText(po);
  if (pos == -1)
    pos = 0;
  _plot->setCurrentIndex(pos);
}

void
MarkerTLineDialog::settings (QColor &color, QDateTime &sd, QDateTime &ed, double &sp, double &ep, bool &ex, QString &po)
{
  color = _color->color();
  sd = _startDate->dateTime();
  ed = _endDate->dateTime();
  sp = _startPrice->value();
  ep = _endPrice->value();
  ex = _extend->isChecked();
  po = _plot->currentText();
}
