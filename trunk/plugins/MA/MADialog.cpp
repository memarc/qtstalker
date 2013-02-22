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

#include "MADialog.h"
#include "Util.h"

MADialog::MADialog (QHash<QString, void *> objects, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("MA") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createMATab(objects);
  loadSettings();
}

MADialog::~MADialog ()
{
  saveSettings();
}

void
MADialog::createMATab (QHash<QString, void *> l)
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
  
  // type
  QStringList tl;
  Util util;
  Object *o = util.object(QString("MA"), QString(), QString());
  if (o)
  {
    QString key("types");
    ObjectCommand toc(key);
    if (o->message(&toc))
      tl = toc.getList(key);
    delete o;
  }
  
  _type = new QComboBox;
  _type->addItems(tl);
  connect(_type, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Type"), _type);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 999999);
  connect(_period, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Period"), _period);

  _tabs->addTab(w, tr("Settings"));
}

void
MADialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
MADialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
MADialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
MADialog::setSettings (QString i, QString ik, QString t, int p)
{
  _input->setInput(i);
  _input->setKey(tr("Input"), ik);
  _type->setCurrentIndex(_type->findText(t));
  _period->setValue(p);
}

void
MADialog::settings (QString &i, QString &ik, QString &t, int &p)
{
  i = _input->input();
  ik = _input->key(tr("Input"));
  t = _type->currentText();
  p = _period->value();
}
