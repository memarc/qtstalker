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
#include "Object.h"


CurveCloudDialog::CurveCloudDialog (QHash<QString, void *> l, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("CurveCloud") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  createPlotTab(l);
  loadSettings();
}

CurveCloudDialog::~CurveCloudDialog ()
{
  saveSettings();
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

  _base = new InputObjectWidget;
  _base->setObjects(tobjects);
  connect(_base, SIGNAL(valueChanged()), this, SLOT(modified()));
  form->addRow(tr("Low Input"), _base);

  _value = new InputObjectWidget;
  _value->setObjects(tobjects);
  connect(_value, SIGNAL(valueChanged()), this, SLOT(modified()));
  form->addRow(tr("High Input"), _value);
  
  // color
  _color = new ColorButton(0, QColor(Qt::red));
  connect(_color, SIGNAL(valueChanged()), this, SLOT(modified()));
  form->addRow(tr("Color"), _color);
  
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
  _color->setColor(c);
  _label->setText(l);
  _base->setInput(bi);
  _base->setKey(tr("Low"), bk);
  _value->setInput(vi);
  _value->setKey(tr("High"), vk);
  
  if (! po.isEmpty())
    _plot->setCurrentIndex(_plot->findText(po));
}

void
CurveCloudDialog::settings (QColor &c, QString &l, QString &bi, QString &bk, QString &vi, QString &vk, QString &po)
{
  c = _color->color();
  l = _label->text();
  bi = _base->input();
  bk = _base->key(tr("Low"));
  vi = _value->input();
  vk = _value->key(tr("High"));
  po = _plot->currentText();
}
