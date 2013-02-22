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

#include "STOCHSDialog.h"
#include "Util.h"

STOCHSDialog::STOCHSDialog (QHash<QString, void *> objects, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("STOCHS") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createTab(objects);
  loadSettings();
}

STOCHSDialog::~STOCHSDialog ()
{
  saveSettings();
}

void
STOCHSDialog::createTab (QHash<QString, void *> l)
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
  form->addRow(tr("Fast Period"), _period);
  
  // k type
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
  
  _ktype = new QComboBox;
  _ktype->addItems(tl);
  connect(_ktype, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("%K MA Type"), _ktype);

  // k period
  _kperiod = new QSpinBox;
  _kperiod->setRange(1, 999999);
  connect(_kperiod, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("%K Period"), _kperiod);

  // d type  
  _dtype = new QComboBox;
  _dtype->addItems(tl);
  connect(_dtype, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("%D MA Type"), _dtype);

  // d period
  _dperiod = new QSpinBox;
  _dperiod->setRange(1, 999999);
  connect(_dperiod, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("%D Period"), _dperiod);

  _tabs->addTab(w, tr("Settings"));
}

void
STOCHSDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
STOCHSDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
STOCHSDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
STOCHSDialog::setSettings (QString i, QString hk, QString lk, QString ck, QString kma, QString dma, int p, int kp, int dp)
{
  _input->setInput(i);
  _input->setKey(tr("High"), hk);
  _input->setKey(tr("Low"), lk);
  _input->setKey(tr("Close"), ck);
  _ktype->setCurrentIndex(_ktype->findText(kma));
  _dtype->setCurrentIndex(_dtype->findText(dma));
  _period->setValue(p);
  _kperiod->setValue(kp);
  _dperiod->setValue(dp);
}

void
STOCHSDialog::settings (QString &i, QString &hk, QString &lk, QString &ck, QString &kma, QString &dma, int &p, int &kp, int &dp)
{
  i = _input->input();
  hk = _input->key(tr("High"));
  lk = _input->key(tr("Low"));
  ck = _input->key(tr("Close"));
  kma = _ktype->currentText();
  dma = _dtype->currentText();
  p = _period->value();
  kp = _kperiod->value();
  dp = _dperiod->value();
}
