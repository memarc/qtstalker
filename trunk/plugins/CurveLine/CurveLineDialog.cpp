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

#include "CurveLineDialog.h"
#include "Util.h"
#include "Object.h"


CurveLineDialog::CurveLineDialog (QHash<QString, void *> l, QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("CurveLine") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");

  createPlotTab(l);
  loadSettings();
}

CurveLineDialog::~CurveLineDialog ()
{
  saveSettings();
}

void
CurveLineDialog::createPlotTab (QHash<QString, void *> l)
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

  // plot
  _plot = new QComboBox;
  _plot->addItems(pl);
  connect(_plot, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Plot"), _plot);

  _input = new InputObjectWidget;
  _input->setObjects(ol);
  connect(_input, SIGNAL(valueChanged()), this, SLOT(modified()));
  form->addRow(tr("Input"), _input);
  
  // color
  _color = new ColorButton(0, QColor(Qt::red));
  connect(_color, SIGNAL(valueChanged()), this, SLOT(modified()));
  form->addRow(tr("Color"), _color);
  
  // style
  Util util;
  QStringList tl;
  Object *o = util.object(QString("CurveLine"), QString(), QString());
  if (o)
  {
    QString key("styles");
    ObjectCommand toc(key);
    if (o->message(&toc))
      tl = toc.getList(key);
    delete o;
  }
  
  _style = new QComboBox;
  _style->addItems(tl);
  connect(_style, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _style);
  
  // label
  _label = new QLineEdit;
  connect(_label, SIGNAL(textEdited(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Label"), _label);
  
  // width
  _width = new QSpinBox;
  connect(_width, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Thickness"), _width);

  _tabs->addTab(w, tr("Plot"));
}

void
CurveLineDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
CurveLineDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
CurveLineDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
CurveLineDialog::setSettings (QColor c, QString l, QString s, int w, QString i, QString ik, QString po)
{
  _color->setColor(c);
  _label->setText(l);
  _style->setCurrentIndex(_style->findText(s));
  _width->setValue(w);
  _input->setInput(i);
  _input->setKey(tr("Input"), ik);
  
  if (! po.isEmpty())
    _plot->setCurrentIndex(_plot->findText(po));
}

void
CurveLineDialog::settings (QColor &c, QString &l, QString &s, int &w, QString &i, QString &ik, QString &po)
{
  c = _color->color();
  i = _input->input();
  ik = _input->key(tr("Input"));
  l = _label->text();
  s = _style->currentText();
  w = _width->value();
  po = _plot->currentText();
}
