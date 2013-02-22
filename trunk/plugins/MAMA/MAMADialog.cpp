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

#include "MAMADialog.h"
#include "Util.h"

MAMADialog::MAMADialog (QHash<QString, void *> objects, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("MAMA") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createTab(objects);
  loadSettings();
}

MAMADialog::~MAMADialog ()
{
  saveSettings();
}

void
MAMADialog::createTab (QHash<QString, void *> l)
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

  // input
  _input = new InputObjectWidget;
  _input->setObjects(ol);
  connect(_input, SIGNAL(valueChanged()), this, SLOT(modified()));
  form->addRow(tr("Input"), _input);
  
  // fast
  _fast = new QDoubleSpinBox;
  _fast->setRange(0, 1);
  connect(_fast, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Fast Limit"), _fast);
  
  // slow
  _slow = new QDoubleSpinBox;
  _slow->setRange(0, 1);
  connect(_slow, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Slow Limit"), _slow);

  _tabs->addTab(w, tr("Settings"));
}

void
MAMADialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
MAMADialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
MAMADialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
MAMADialog::setSettings (QString io, QString ik, double fast, double slow)
{
  _input->setInput(io);
  _input->setKey(tr("Input"), ik);
  _fast->setValue(fast);
  _slow->setValue(slow);
}

void
MAMADialog::settings (QString &io, QString &ik, double &fast, double &slow)
{
  io = _input->input();
  ik = _input->key(tr("Input"));
  fast = _fast->value();
  slow = _slow->value();
}

