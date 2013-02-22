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

#include "CCIDialog.h"
#include "Util.h"

CCIDialog::CCIDialog (QHash<QString, void *> objects, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("CCI") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createTab(objects);
  loadSettings();
}

CCIDialog::~CCIDialog ()
{
  saveSettings();
}

void
CCIDialog::createTab (QHash<QString, void *> l)
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

  _tabs->addTab(w, tr("Settings"));
}

void
CCIDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
CCIDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
CCIDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
CCIDialog::setSettings (QString io, QString hk, QString lk, QString ck, int p)
{
  _input->setInput(io);
  _input->setKey(tr("High"), hk);
  _input->setKey(tr("Low"), lk);
  _input->setKey(tr("Close"), ck);
  _period->setValue(p);
}

void
CCIDialog::settings (QString &io, QString &hk, QString &lk, QString &ck, int &p)
{
  io = _input->input();
  hk = _input->key(tr("High"));
  lk = _input->key(tr("Low"));
  ck = _input->key(tr("Close"));
  p = _period->value();
}

