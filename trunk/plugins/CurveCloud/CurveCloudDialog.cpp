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

#include "CurveCloudDialog.h"
#include "Util.h"


CurveCloudDialog::CurveCloudDialog (QHash<QString, void *> l, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("CurveCloud") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  Util util;
  _color = util.object(QString("ColorButton"), QString(), QString("color"));

  _base = util.object(QString("IndicatorInput"), QString(), QString("base"));
  _value = util.object(QString("IndicatorInput"), QString(), QString("value"));

  createPlotTab(l);
  loadSettings();
}

CurveCloudDialog::~CurveCloudDialog ()
{
  saveSettings();
  
  if (_color)
    delete _color;
  if (_base)
    delete _base;
  if (_value)
    delete _value;
}

void
CurveCloudDialog::createPlotTab (QHash<QString, void *> l)
{
  QStringList pl;
  QHash<QString, void *> tobjects;
  QHashIterator<QString, void *> it(l);
  while (it.hasNext())
  {
    it.next();
    Object *o = (Object *) it.value();
    
    if (o->hasOutput())
      tobjects.insert(it.key(), it.value());
    
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

  // base
  if (_base)
  {
    QWidget *w = _base->widget();
    
    ObjectCommand toc(QString("set_objects"));
    toc.setObjects(tobjects);
    _base->message(&toc);
    
    connect(_base, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(modified()));
    form->addRow(tr("Low Input"), w);
  }
  
  // value
  if (_value)
  {
    QWidget *w = _value->widget();
    
    ObjectCommand toc(QString("set_objects"));
    toc.setObjects(tobjects);
    _value->message(&toc);
    
    connect(_value, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(modified()));
    form->addRow(tr("High Input"), w);
  }

  // color
  if (_color)
  {
    connect(_color, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(modified()));
    form->addRow(tr("Color"), _color->widget());
  }
  
  // label
  _label = new QLineEdit;
  connect(_label, SIGNAL(textEdited(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Label"), _label);

  _tabs->addTab(w, tr("Plot"));
}

void
CurveCloudDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
CurveCloudDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
CurveCloudDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
CurveCloudDialog::setSettings (QColor c, QString l, QString bi, QString bk, QString vi, QString vk, QString po)
{
  if (_color)
  {
    ObjectCommand toc(QString("set_color"));
    toc.setValue(QString("color"), c);
    _color->message(&toc);
  }
  
  if (_base)
  {
    ObjectCommand toc(QString("set_input"));
    toc.setValue(QString("input"), bi);
    _base->message(&toc);

    toc.setCommand(QString("set_key"));
    toc.setValue(QString("key"), tr("Low"));
    toc.setValue(QString("data"), bk);
    _base->message(&toc);
  }
  
  if (_value)
  {
    ObjectCommand toc(QString("set_input"));
    toc.setValue(QString("input"), vi);
    _value->message(&toc);

    toc.setCommand(QString("set_key"));
    toc.setValue(QString("key"), tr("High"));
    toc.setValue(QString("data"), vk);
    _value->message(&toc);
  }
  
  _label->setText(l);
  
  if (! po.isEmpty())
    _plot->setCurrentIndex(_plot->findText(po));
}

void
CurveCloudDialog::settings (QColor &c, QString &l, QString &bi, QString &bk, QString &vi, QString &vk, QString &po)
{
  if (_color)
  {
    QString key("color");
    ObjectCommand toc(key);
    if (_color->message(&toc))
      c = toc.getColor(key);
  }

  if (_base)
  {
    ObjectCommand toc(QString("input"));
    _base->message(&toc);
    bi = toc.getString(QString("input"));

    toc.setCommand(QString("key"));
    toc.setValue(QString("key"), tr("Low"));
    _base->message(&toc);
    bk = toc.getString(QString("data"));
  }

  if (_value)
  {
    ObjectCommand toc(QString("input"));
    _value->message(&toc);
    vi = toc.getString(QString("input"));

    toc.setCommand(QString("key"));
    toc.setValue(QString("key"), tr("Low"));
    _value->message(&toc);
    vk = toc.getString(QString("data"));
  }
  
  l = _label->text();
  po = _plot->currentText();
}
