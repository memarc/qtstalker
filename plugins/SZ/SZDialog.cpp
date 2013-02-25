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

#include "SZDialog.h"
#include "Util.h"

SZDialog::SZDialog (QHash<QString, void *> objects, QStringList ml, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("SZ") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  Util util;
  _input = util.object(QString("IndicatorInput"), QString(), QString("input"));
  
  createTab(objects, ml);
  loadSettings();
}

SZDialog::~SZDialog ()
{
  saveSettings();
  
  if (_input)
    delete _input;
}

void
SZDialog::createTab (QHash<QString, void *> l, QStringList ml)
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

  _method = new QComboBox;
  _method->addItems(ml);
  connect(_method, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Method"), _method);
  
  // period
  _period = new QSpinBox;
  _period->setRange(1, 999999);
  connect(_period, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Period"), _period);
  
  // no decline
  _noDecline = new QSpinBox;
  _noDecline->setRange(0, 365);
  connect(_noDecline, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("No Decline Period"), _noDecline);
  
  // coeff
  _coeff = new QDoubleSpinBox;
  _coeff->setRange(0, 9999999);
  connect(_coeff, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Coefficient"), _coeff);

  _tabs->addTab(w, tr("Settings"));
}

void
SZDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
SZDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
SZDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
SZDialog::setSettings (QString i, QString hk, QString lk, int p, int nd, double co, QString m)
{
  if (_input)
  {
    ObjectCommand toc(QString("set_input"));
    toc.setValue(QString("input"), i);
    _input->message(&toc);

    toc.setCommand(QString("set_key"));
    toc.setValue(QString("key"), tr("High"));
    toc.setValue(QString("data"), hk);
    _input->message(&toc);
  
    toc.setValue(QString("key"), tr("Low"));
    toc.setValue(QString("data"), lk);
    _input->message(&toc);
  }
  
  _period->setValue(p);
  _noDecline->setValue(nd);
  _coeff->setValue(co);
  _method->setCurrentIndex(_method->findText(m));
}

void
SZDialog::settings (QString &i, QString &hk, QString &lk, int &p, int &nd, double &co, QString &m)
{
  if (_input)
  {
    ObjectCommand toc(QString("input"));
    _input->message(&toc);
    i = toc.getString(QString("input"));

    toc.setCommand(QString("key"));
    toc.setValue(QString("key"), tr("High"));
    _input->message(&toc);
    hk = toc.getString(QString("data"));

    toc.setValue(QString("key"), tr("Low"));
    _input->message(&toc);
    lk = toc.getString(QString("data"));
  }
  
  p = _period->value();
  nd = _noDecline->value();
  co = _coeff->value();
  m = _method->currentText();
}
