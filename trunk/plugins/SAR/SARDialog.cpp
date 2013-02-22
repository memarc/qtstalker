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

#include "SARDialog.h"
#include "Util.h"

SARDialog::SARDialog (QHash<QString, void *> objects, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("SAR") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createTab(objects);
  loadSettings();
}

SARDialog::~SARDialog ()
{
  saveSettings();
}

void
SARDialog::createTab (QHash<QString, void *> l)
{
  QHash<QString, void *> ol;
  QHashIterator<QString, void *> it(l);
  while (it.hasNext())
  {
    it.next();
    Object *o = (Object *) it.value();
    if (o->hasOutput())
      ol.insert(it.key(), it.value());
  }

  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(10);
  w->setLayout(form);

  _input = new InputObjectWidget;
  _input->setObjects(ol);
  connect(_input, SIGNAL(valueChanged()), this, SLOT(modified()));
  form->addRow(tr("Input"), _input);
  
  // init
  _init = new QDoubleSpinBox;
  _init->setRange(0, 1);
  connect(_init, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Acceleration"), _init);
  
  // max
  _max = new QDoubleSpinBox;
  _max->setRange(0, 1);
  connect(_max, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Acceleration Maximum"), _max);

  _tabs->addTab(w, tr("Settings"));
}

void
SARDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
SARDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
SARDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
SARDialog::setSettings (QString i, QString hk, QString lk, double init, double max)
{
  _input->setInput(i);
  _input->setKey(tr("High"), hk);
  _input->setKey(tr("Low"), lk);
  _init->setValue(init);
  _max->setValue(max);
}

void
SARDialog::settings (QString &i, QString &hk, QString &lk, double &init, double &max)
{
  i = _input->input();
  hk = _input->key(tr("High"));
  lk = _input->key(tr("Low"));
  init = _init->value();
  max = _max->value();
}
