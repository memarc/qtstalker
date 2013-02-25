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

#include "SINEDialog.h"
#include "Util.h"

SINEDialog::SINEDialog (QHash<QString, void *> objects, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("SINE") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  Util util;
  _input = util.object(QString("IndicatorInput"), QString(), QString("input"));
  
  createTab(objects);
  loadSettings();
}

SINEDialog::~SINEDialog ()
{
  saveSettings();
  
  if (_input)
    delete _input;
}

void
SINEDialog::createTab (QHash<QString, void *> l)
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
SINEDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
SINEDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
SINEDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
SINEDialog::setSettings (QString io, QString ik)
{
  if (_input)
  {
    ObjectCommand toc(QString("set_input"));
    toc.setValue(QString("input"), io);
    _input->message(&toc);

    toc.setCommand(QString("set_key"));
    toc.setValue(QString("key"), tr("Input"));
    toc.setValue(QString("data"), ik);
    _input->message(&toc);
  }
}

void
SINEDialog::settings (QString &io, QString &ik)
{
  if (_input)
  {
    ObjectCommand toc(QString("input"));
    _input->message(&toc);
    io = toc.getString(QString("input"));

    toc.setCommand(QString("key"));
    toc.setValue(QString("key"), tr("Input"));
    _input->message(&toc);
    ik = toc.getString(QString("data"));
  }
}
