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

#include "T3Dialog.h"
#include "Util.h"

T3Dialog::T3Dialog (QHash<QString, void *> objects, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("T3") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createTab(objects);
  loadSettings();
}

T3Dialog::~T3Dialog ()
{
  saveSettings();
}

void
T3Dialog::createTab (QHash<QString, void *> l)
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
  
  // period
  _period = new QSpinBox;
  _period->setRange(1, 999999);
  connect(_period, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Period"), _period);
  
  // vfactor
  _vfactor = new QDoubleSpinBox;
  _vfactor->setRange(0, 1);
  connect(_vfactor, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Volume Factor"), _vfactor);

  _tabs->addTab(w, tr("Settings"));
}

void
T3Dialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
T3Dialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
T3Dialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
T3Dialog::setSettings (QString i, QString ik, int p, double vf)
{
  _input->setInput(i);
  _input->setKey(tr("Input"), ik);
  _period->setValue(p);
  _vfactor->setValue(vf);
}

void
T3Dialog::settings (QString &i, QString &ik, int &p, double &vf)
{
  i = _input->input();
  ik = _input->key(tr("Input"));
  p = _period->value();
  vf = _vfactor->value();
}
