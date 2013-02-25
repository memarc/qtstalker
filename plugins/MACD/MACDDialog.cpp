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

#include "MACDDialog.h"
#include "Util.h"

MACDDialog::MACDDialog (QHash<QString, void *> objects, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("MACD") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  Util util;
  _input = util.object(QString("IndicatorInput"), QString(), QString("input"));
  
  createTab(objects);
  loadSettings();
}

MACDDialog::~MACDDialog ()
{
  saveSettings();
  
  if (_input)
    delete _input;
}

void
MACDDialog::createTab (QHash<QString, void *> l)
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
  
  // fast ma type
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
  
  _fastMAType = new QComboBox;
  _fastMAType->addItems(tl);
  connect(_fastMAType, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Fast MA"), _fastMAType);

  // fast period
  _fastPeriod = new QSpinBox;
  _fastPeriod->setRange(1, 999999);
  connect(_fastPeriod, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Fast Period"), _fastPeriod);
  
  // slow ma type
  _slowMAType = new QComboBox;
  _slowMAType->addItems(tl);
  connect(_slowMAType, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Slow MA"), _slowMAType);

  // slow period
  _slowPeriod = new QSpinBox;
  _slowPeriod->setRange(1, 999999);
  connect(_slowPeriod, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Slow Period"), _slowPeriod);
  
  // signal ma type
  _signalMAType = new QComboBox;
  _signalMAType->addItems(tl);
  connect(_signalMAType, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Signal MA"), _signalMAType);

  // signal period
  _signalPeriod = new QSpinBox;
  _signalPeriod->setRange(1, 999999);
  connect(_signalPeriod, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Signal Period"), _signalPeriod);

  _tabs->addTab(w, tr("Settings"));
}

void
MACDDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
MACDDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
MACDDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
MACDDialog::setSettings (QString io, QString ik, int fp, QString fma, int sp, QString sma, int sigp, QString sigma)
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
  
  _fastPeriod->setValue(fp);
  _fastMAType->setCurrentIndex(_fastMAType->findText(fma));
  _slowPeriod->setValue(sp);
  _slowMAType->setCurrentIndex(_slowMAType->findText(sma));
  _signalPeriod->setValue(sigp);
  _signalMAType->setCurrentIndex(_signalMAType->findText(sigma));
}

void
MACDDialog::settings (QString &io, QString &ik, int &fp, QString &fma, int &sp, QString &sma, int &sigp, QString &sigma)
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

  fma = _fastMAType->currentText();
  fp = _fastPeriod->value();
  sma = _slowMAType->currentText();
  sp = _slowPeriod->value();
  sigma = _signalMAType->currentText();
  sigp = _signalPeriod->value();
}
