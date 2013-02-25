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

#include "CompareValuesDialog.h"
#include "Util.h"


CompareValuesDialog::CompareValuesDialog (QHash<QString, void *> objects, QStringList opList, QString name) :
					  Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("CompareValues") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  Util util;
  _input = util.object(QString("IndicatorInput"), QString(), QString("input"));
  _input2 = util.object(QString("IndicatorInput"), QString(), QString("input2"));

  createCompareTab(objects, opList);
  loadSettings();
}

CompareValuesDialog::~CompareValuesDialog ()
{
  saveSettings();
  
  if (_input)
    delete _input;
  if (_input2)
    delete _input2;
}

void
CompareValuesDialog::createCompareTab (QHash<QString, void *> l, QStringList opList)
{
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(10);
  w->setLayout(form);

  // input objects
  QHash<QString, void *> ol;
  QHashIterator<QString, void *> it(l);
  while (it.hasNext())
  {
    it.next();
    Object *o = (Object *) it.value();
    if (o->hasOutput())
      ol.insert(it.key(), it.value());
  }

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

  // op
  _op = new QComboBox;
  _op->addItems(opList);
  connect(_op, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Op"), _op);

  // input2
  if (_input2)
  {
    QWidget *w = _input2->widget();
    
    ObjectCommand toc(QString("set_objects"));
    toc.setObjects(ol);
    _input2->message(&toc);
    
    connect(_input2, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(modified()));
    form->addRow(tr("Input 2"), w);
  }

  // constant
  _constant = new QCheckBox;
  connect(_constant, SIGNAL(toggled(bool)), this, SLOT(modified()));
  connect(_constant, SIGNAL(toggled(bool)), this, SLOT(constantChanged(bool)));
  form->addRow(tr("Use Constant Value"), _constant);
  
  // value
  _value = new QDoubleSpinBox;
  connect(_value, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Constant Value"), _value);
  
  _tabs->addTab(w, tr("Settings"));
}

void
CompareValuesDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
CompareValuesDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
CompareValuesDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
CompareValuesDialog::setSettings (QString i, QString ik, int o, QString i2, QString i2k,
                                  int o2, int op, bool constant, double value)
{
  if (_input && _input2)
  {
    ObjectCommand toc(QString("set_input"));
    toc.setValue(QString("input"), i);
    _input->message(&toc);

    toc.setValue(QString("input"), i2);
    _input2->message(&toc);
  
    toc.setCommand(QString("set_key"));
    toc.setValue(QString("key"), tr("Input"));
    toc.setValue(QString("data"), ik);
    _input->message(&toc);
  
    toc.setValue(QString("key"), tr("Input 2"));
    toc.setValue(QString("data"), i2k);
    _input2->message(&toc);

    toc.setCommand(QString("set_offset"));
    toc.setValue(QString("offset"), o);
    _input->message(&toc);

    toc.setValue(QString("offset"), o2);
    _input2->message(&toc);
  }
  
  _op->setCurrentIndex(op);
  _constant->setChecked(constant);
  _value->setValue(value);
  
  constantChanged(constant);
}

void
CompareValuesDialog::settings (QString &i, QString &ik, int &o, QString &i2, QString &i2k,
                               int &o2, int &op, bool &constant, double &value)
{
  if (_input && _input2)
  {
    ObjectCommand toc(QString("input"));
    _input->message(&toc);
    i = toc.getString(QString("input"));

    _input2->message(&toc);
    i2 = toc.getString(QString("input"));

    toc.setCommand(QString("key"));
    toc.setValue(QString("key"), tr("Input"));
    _input->message(&toc);
    ik = toc.getString(QString("data"));

    toc.setValue(QString("key"), tr("Input 2"));
    _input2->message(&toc);
    i2k = toc.getString(QString("data"));

    toc.setCommand(QString("offset"));
    _input->message(&toc);
    o = toc.getInt(QString("offset"));

    _input2->message(&toc);
    o2 = toc.getInt(QString("offset"));
  }
  
  op = _op->currentIndex();
  constant = _constant->isChecked();
  value = _value->value();
}

void
CompareValuesDialog::constantChanged (bool d)
{
  ObjectCommand toc(QString("set_enabled"));
  if (d)
  {
    toc.setValue(QString("status"), FALSE);
    _value->setEnabled(TRUE);
  }
  else
  {
    toc.setValue(QString("status"), TRUE);
    _value->setEnabled(FALSE);
  }
  
  if (_input2)
    _input2->message(&toc);
}
