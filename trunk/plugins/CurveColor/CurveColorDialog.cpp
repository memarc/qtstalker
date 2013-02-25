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

#include "CurveColorDialog.h"
#include "Util.h"

CurveColorDialog::CurveColorDialog (QHash<QString, void *> objects, QStringList opList, QString name) :
                                    Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("CurveColor") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  Util util;
  _color = util.object(QString("ColorButton"), QString(), QString("color"));

  _input = util.object(QString("IndicatorInput"), QString(), QString("input"));
  
  createColorTab(objects, opList);
  loadSettings();
}

CurveColorDialog::~CurveColorDialog ()
{
  saveSettings();
  
  if (_color)
    delete _color;
  if (_input)
    delete _input;
}

void
CurveColorDialog::createColorTab (QHash<QString, void *> l, QStringList opList)
{
  QHash<QString, void *> il;
  QStringList cl, pl;
  QHashIterator<QString, void *> it(l);
  while (it.hasNext())
  {
    it.next();
    Object *o = (Object *) it.value();
    if (o->hasOutput())
    {
      il.insert(it.key(), it.value());
      continue;
    }
    
    if (o->type() == QString("curve"))
      cl << it.key();
    
    if (o->plugin() == QString("Plot"))
      pl << it.key();
  }
  
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(10);
  w->setLayout(form);

  // plot
  _plot = new QComboBox;
  _plot->addItems(pl);
  connect(_plot, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Plot"), _plot);

  // input
  if (_input)
  {
    QWidget *w = _input->widget();
    
    ObjectCommand toc(QString("set_objects"));
    toc.setObjects(il);
    _input->message(&toc);
    
    connect(_input, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(modified()));
    form->addRow(tr("Input"), w);
  }
  
  // op
  _op = new QComboBox;
  _op->addItems(opList);
  connect(_op, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Op"), _op);

  // value
  _value = new QDoubleSpinBox;
  _value->setRange(-9999999999.0, 9999999999.0);
  connect(_value, SIGNAL(valueChanged(double)), this, SLOT(modified()));
  form->addRow(tr("Value"), _value);

  // input2 objects
  _input2 = new QComboBox;
  _input2->addItems(cl);
  connect(_input2, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Curve"), _input2);
  
  // color
  if (_color)
  {
    connect(_color, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(modified()));
    form->addRow(tr("Color"), _color->widget());
  }

  _tabs->addTab(w, tr("Settings"));
}

void
CurveColorDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
CurveColorDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
CurveColorDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
CurveColorDialog::setSettings (QColor c, QString i, QString ik, QString i2, int op, double v, QString po)
{
  if (_color)
  {
    ObjectCommand toc(QString("set_color"));
    toc.setValue(QString("color"), c);
    _color->message(&toc);
  }

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
  
  _input2->setCurrentIndex(_input2->findText(i2));
  _op->setCurrentIndex(op);
  _value->setValue(v);
  
  if (! po.isEmpty())
    _plot->setCurrentIndex(_plot->findText(po));
}

void
CurveColorDialog::settings (QColor &c, QString &i, QString &ik, QString &i2, int &op, double &v, QString &po)
{
  if (_color)
  {
    QString key("color");
    ObjectCommand toc(key);
    if (_color->message(&toc))
      c = toc.getColor(key);
  }

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
  
  i2 = _input2->currentText();
  v = _value->value();
  op = _op->currentIndex();
  po = _plot->currentText();
}
