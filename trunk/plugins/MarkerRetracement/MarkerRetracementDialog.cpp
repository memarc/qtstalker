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

#include "MarkerRetracementDialog.h"
#include "Object.h"

#include <QDebug>

MarkerRetracementDialog::MarkerRetracementDialog (QHash<QString, void *> l, QString name) : Dialog (0, name)
{
  QStringList tl;
  tl << QString("OTA") << QString("-") << tr("Editing Marker Retracement");
  setWindowTitle(tl.join(" "));

  tl.clear();
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("MarkerRetracement") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createTab(l);
  loadSettings();
}

MarkerRetracementDialog::~MarkerRetracementDialog ()
{
  saveSettings();
}

void
MarkerRetracementDialog::createTab (QHash<QString, void *> l)
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
  
  // high price
  _highPrice = new QDoubleSpinBox;
  _highPrice->setRange(-99999999.0, 99999999.0);
  _highPrice->setDecimals(4);
  connect(_highPrice, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("High Price"), _highPrice);
  
  // low price
  _lowPrice = new QDoubleSpinBox;
  _lowPrice->setRange(-99999999.0, 99999999.0);
  _lowPrice->setDecimals(4);
  connect(_lowPrice, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Low Price"), _lowPrice);
  
  // level1
  _level1 = new QDoubleSpinBox;
  _level1->setRange(0.0, 1.0);
  _level1->setDecimals(4);
  connect(_level1, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Level 1"), _level1);
  
  // level2
  _level2 = new QDoubleSpinBox;
  _level2->setRange(0.0, 1.0);
  _level2->setDecimals(4);
  connect(_level2, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Level 2"), _level2);
  
  // level3
  _level3 = new QDoubleSpinBox;
  _level3->setRange(0.0, 1.0);
  _level3->setDecimals(4);
  connect(_level3, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Level 3"), _level3);

  _extend = new QCheckBox;
  connect(_extend, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Extend"), _extend);

  _tabs->addTab(w, tr("Settings"));
}

void
MarkerRetracementDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
MarkerRetracementDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
MarkerRetracementDialog::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}


void
MarkerRetracementDialog::setSettings (QColor c, QDateTime sd, QDateTime ed, double hp, double lp,
		                      bool extend, double l1, double l2, double l3, QString po)
{
  _color->setColor(c);
  _startDate->setDateTime(sd);
  _endDate->setDateTime(ed);
  _highPrice->setValue(hp);
  _lowPrice->setValue(lp);
  _extend->setChecked(extend);
  _level1->setValue(l1);
  _level2->setValue(l2);
  _level3->setValue(l3);

  int pos = _plot->findText(po);
  if (pos == -1)
    pos = 0;
  _plot->setCurrentIndex(pos);
}

void
MarkerRetracementDialog::settings (QColor &c, QDateTime &sd, QDateTime &ed, double &hp, double &lp,
		                   bool &extend, double &l1, double &l2, double &l3, QString &po)
{
  c = _color->color();
  sd = _startDate->dateTime();
  ed = _endDate->dateTime();
  hp = _highPrice->value();
  lp = _lowPrice->value();
  extend = _extend->isChecked();
  l1 = _level1->value();
  l2 = _level2->value();
  l3 = _level3->value();
  po = _plot->currentText();
}
