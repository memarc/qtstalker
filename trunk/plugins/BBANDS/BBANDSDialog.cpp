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

#include "BBANDSDialog.h"
#include "Util.h"

BBANDSDialog::BBANDSDialog (QHash<QString, void *> objects, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("BBANDS") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  Util util;
  _input = util.object(QString("IndicatorInput"), QString(), QString("input"));
  
  createTab(objects);
  loadSettings();
}

BBANDSDialog::~BBANDSDialog ()
{
  saveSettings();
  
  if (_input)
    delete _input;
}

void
BBANDSDialog::createTab (QHash<QString, void *> l)
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

  // up deviation
  _udev = new QDoubleSpinBox;
  _udev->setRange(-999.0, 999.0);
  connect(_udev, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Up Deviation"), _udev);

  // down deviation
  _ldev = new QDoubleSpinBox;
  _ldev->setRange(-999.0, 999.0);
  connect(_ldev, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Down Deviation"), _ldev);

  _tabs->addTab(w, tr("Settings"));
}

void
BBANDSDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
BBANDSDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
BBANDSDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
BBANDSDialog::setSettings (QString i, QString ik, QString t, int p, double udev, double ldev)
{
  if (_input)
  {
    ObjectCommand toc(QString("set_input"));
    toc.setValue(QString("input"), i);
    _input->message(&toc);

    toc.setCommand(QString("set_key"));
    toc.setValue(QString("key"), tr("Input"));
    toc.setValue(QString("data"), ik);
    _input->message(&toc);
  }
  
  _type->setCurrentIndex(_type->findText(t));
  _period->setValue(p);
  _udev->setValue(udev);
  _ldev->setValue(ldev);
}

void
BBANDSDialog::settings (QString &i, QString &ik, QString &t, int &p, double &udev, double &ldev)
{
  if (_input)
  {
    ObjectCommand toc(QString("input"));
    _input->message(&toc);
    i = toc.getString(QString("input"));

    toc.setCommand(QString("key"));
    toc.setValue(QString("key"), tr("Input"));
    _input->message(&toc);
    ik = toc.getString(QString("data"));
  }

  t = _type->currentText();
  p = _period->value();
  udev = _udev->value();
  ldev = _ldev->value();
}
