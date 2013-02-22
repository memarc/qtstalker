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

#include "CurveOHLCDialog.h"
#include "Util.h"
#include "Object.h"


CurveOHLCDialog::CurveOHLCDialog (QHash<QString, void *> l, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("CurveOHLC") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  createTab(l);
  loadSettings();
}

CurveOHLCDialog::~CurveOHLCDialog ()
{
  saveSettings();
}

void
CurveOHLCDialog::createTab (QHash<QString, void *> l)
{
  QStringList pl;
  QHash<QString, void *> ol;
  QHashIterator<QString, void *> it(l);
  while (it.hasNext())
  {
    it.next();
    Object *o = (Object *) it.value();
    
    if (o->hasOutput())
      ol.insert(it.key(), it.value());
    
    if (o->plugin() == QString("Plot"))
      pl << it.key();
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

  // plot
  _plot = new QComboBox;
  _plot->addItems(pl);
  connect(_plot, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Plot"), _plot);
  
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
CurveOHLCDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
CurveOHLCDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
CurveOHLCDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
CurveOHLCDialog::setSettings (QColor c, QString l, QString i, QString ok,
			      QString hk, QString lk, QString ck, QString po)
{
  _color->setColor(c);
  _label->setText(l);
  
  _input->setInput(i);
  _input->setKey(tr("Open"), ok);
  _input->setKey(tr("High"), hk);
  _input->setKey(tr("Low"), lk);
  _input->setKey(tr("Close"), ck);
  
  if (! po.isEmpty())
    _plot->setCurrentIndex(_plot->findText(po));
}

void
CurveOHLCDialog::settings (QColor &c, QString &l, QString &i, QString &ok,
			   QString &hk, QString &lk, QString &ck, QString &po)
{
  c = _color->color();
  l = _label->text();
  i = _input->input();
  ok = _input->key(tr("Open"));
  hk = _input->key(tr("High"));
  lk = _input->key(tr("Low"));
  ck = _input->key(tr("Close"));
  po = _plot->currentText();
}
