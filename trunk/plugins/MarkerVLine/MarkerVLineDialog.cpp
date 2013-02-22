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

#include "MarkerVLineDialog.h"
#include "Object.h"

#include <QDebug>

MarkerVLineDialog::MarkerVLineDialog (QHash<QString, void *> l, QString name) : Dialog (0, name)
{
  QStringList tl;
  tl << QString("OTA") << QString("-") << tr("Editing Marker VLine");
  setWindowTitle(tl.join(" "));

  tl.clear();
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("MarkerVLine") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createTab(l);
  loadSettings();
}

MarkerVLineDialog::~MarkerVLineDialog ()
{
  saveSettings();
}

void
MarkerVLineDialog::createTab (QHash<QString, void *> l)
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

  _tabs->addTab(w, tr("Settings"));
}

void
MarkerVLineDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
MarkerVLineDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
MarkerVLineDialog::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
MarkerVLineDialog::setSettings (QColor color, QDateTime date, QString po)
{
  _color->setColor(color);
  _date->setDateTime(date);

  int pos = _plot->findText(po);
  if (pos == -1)
    pos = 0;
  _plot->setCurrentIndex(pos);
}

void
MarkerVLineDialog::settings (QColor &color, QDateTime &date, QString &po)
{
  color = _color->color();
  date = _date->dateTime();
  po = _plot->currentText();
}

