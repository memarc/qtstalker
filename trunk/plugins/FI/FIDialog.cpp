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

#include "FIDialog.h"
#include "Util.h"

FIDialog::FIDialog (QHash<QString, void *> objects, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("FI") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  Util util;
  _input = util.object(QString("IndicatorInput"), QString(), QString("input"));
  
  createTab(objects);
  loadSettings();
}

FIDialog::~FIDialog ()
{
  saveSettings();
  
  if (_input)
    delete _input;
}

void
FIDialog::createTab (QHash<QString, void *> l)
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
  if (_input)
  {
    QWidget *w = _input->widget();
    
    ObjectCommand toc(QString("set_objects"));
    toc.setObjects(ol);
    _input->message(&toc);
    
    connect(_input, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(modified()));
    form->addRow(tr("Input"), w);
  }

  _tabs->addTab(w, tr("Settings"));
}

void
FIDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
FIDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
FIDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
FIDialog::setSettings (QString io, QString ck, QString vk)
{
  if (! _input)
    return;
  
  ObjectCommand toc(QString("set_input"));
  toc.setValue(QString("input"), io);
  _input->message(&toc);

  toc.setCommand(QString("set_key"));
  toc.setValue(QString("key"), tr("Close"));
  toc.setValue(QString("data"), ck);
  _input->message(&toc);
  
  toc.setValue(QString("key"), tr("Volume"));
  toc.setValue(QString("data"), vk);
  _input->message(&toc);
}

void
FIDialog::settings (QString &io, QString &ck, QString &vk)
{
  if (! _input)
    return;
  
  ObjectCommand toc(QString("input"));
  _input->message(&toc);
  io = toc.getString(QString("input"));

  toc.setCommand(QString("key"));
  toc.setValue(QString("key"), tr("Close"));
  _input->message(&toc);
  ck = toc.getString(QString("data"));

  toc.setValue(QString("key"), tr("Volume"));
  _input->message(&toc);
  vk = toc.getString(QString("data"));
}
