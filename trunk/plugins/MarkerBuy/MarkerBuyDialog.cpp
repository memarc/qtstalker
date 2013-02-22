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

#include "MarkerBuyDialog.h"
#include "Object.h"

#include <QDebug>

MarkerBuyDialog::MarkerBuyDialog (QHash<QString, void *> l, QString) : Dialog (0, QString())
{
  QStringList tl;
  tl << QString("OTA") << QString("-") << tr("Editing Marker Buy");
  setWindowTitle(tl.join(" "));

  tl.clear();
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("MarkerBuy") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createTab(l);
  loadSettings();
}

MarkerBuyDialog::~MarkerBuyDialog ()
{
  saveSettings();
}

void
MarkerBuyDialog::createTab (QHash<QString, void *> l)
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
  
  // date
  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  connect(_date, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(modified()));
  form->addRow(tr("Date"), _date);
  
  // price
  _price = new QDoubleSpinBox;
  _price->setRange(-99999999.0, 99999999.0);
  _price->setDecimals(4);
  connect(_price, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Price"), _price);

  _tabs->addTab(w, tr("Settings"));
}

void
MarkerBuyDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
MarkerBuyDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
MarkerBuyDialog::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
MarkerBuyDialog::setSettings (QColor color, QDateTime date, double price, QString po)
{
  _color->setColor(color);
  _date->setDateTime(date);
  _price->setValue(price);

  int pos = _plot->findText(po);
  if (pos == -1)
    pos = 0;
  _plot->setCurrentIndex(pos);
}

void
MarkerBuyDialog::settings (QColor &color, QDateTime &date, double &price, QString &po)
{
  color = _color->color();
  date = _date->dateTime();
  price = _price->value();
  po = _plot->currentText();
}
