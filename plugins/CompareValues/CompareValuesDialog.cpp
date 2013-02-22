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
#include "Object.h"


CompareValuesDialog::CompareValuesDialog (QHash<QString, void *> objects, QStringList opList, QString name) :
					  Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("CompareValues") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  createCompareTab(objects, opList);
  loadSettings();
}

CompareValuesDialog::~CompareValuesDialog ()
{
  saveSettings();
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

  _input = new InputObjectWidget;
  _input->setObjects(ol);
  connect(_input, SIGNAL(valueChanged()), this, SLOT(modified()));
  form->addRow(tr("Input"), _input);

  // op
  _op = new QComboBox;
  _op->addItems(opList);
  connect(_op, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Op"), _op);

  _input2 = new InputObjectWidget;
  _input2->setObjects(ol);
  connect(_input2, SIGNAL(valueChanged()), this, SLOT(modified()));
  form->addRow(tr("Input 2"), _input2);

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
  _input->setInput(i);
  _input->setKey(tr("Input"), ik);
  _input->setOffset(o);
  _input2->setInput(i2);
  _input2->setKey(tr("Input"), i2k);
  _input2->setOffset(o2);
  _op->setCurrentIndex(op);
  _constant->setChecked(constant);
  _value->setValue(value);
  
  constantChanged(constant);
}

void
CompareValuesDialog::settings (QString &i, QString &ik, int &o, QString &i2, QString &i2k,
                               int &o2, int &op, bool &constant, double &value)
{
  i = _input->input();
  ik = _input->key(tr("Input"));
  o = _input->offset();
  i2 = _input2->input();
  i2k = _input2->key(tr("Input"));
  o2 = _input2->offset();
  op = _op->currentIndex();
  constant = _constant->isChecked();
  value = _value->value();
}

void
CompareValuesDialog::constantChanged (bool d)
{
  if (d)
  {
    _input2->setEnabled(FALSE);
    _value->setEnabled(TRUE);
  }
  else
  {
    _input2->setEnabled(TRUE);
    _value->setEnabled(FALSE);
  }
}
