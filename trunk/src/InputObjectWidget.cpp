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

#include "InputObjectWidget.h"
#include "Object.h"

#include <QDebug>
#include <QLayout>

InputObjectWidget::InputObjectWidget ()
{
  _offset = 0;

  _grid = new QGridLayout;
  _grid->setSpacing(2);
  _grid->setMargin(0);
  setLayout(_grid);

  _input = new QComboBox;
  _input->setToolTip(tr("Input"));
  connect(_input, SIGNAL(currentIndexChanged(int)), this, SIGNAL(valueChanged()));
  connect(_input, SIGNAL(currentIndexChanged(int)), this, SLOT(objectChanged()));
  _grid->addWidget(_input, 0, 0);
}

InputObjectWidget::~InputObjectWidget ()
{
}

void
InputObjectWidget::setObjects (QHash<QString, void *> d)
{
  _objects = d;
  
  _input->blockSignals(TRUE);
  _input->clear();
  _input->addItems(_objects.keys());
  _input->blockSignals(FALSE);
}

void
InputObjectWidget::setInput (QString d)
{
  int pos = _input->findText(d);
  if (pos == -1)
    pos = 0;
  _input->setCurrentIndex(pos);

  objectChanged();
}

QString
InputObjectWidget::input ()
{
  return _input->currentText();
}

void
InputObjectWidget::setKey (QString k, QString d)
{
  QComboBox *key = _keys.value(k);
  if (! key)
  {
    QString ts = k + " " + tr("Field");

    key = new QComboBox;
    key->setToolTip(ts);

    Object *o = (Object *) _objects.value(_input->currentText());  
    if (o)
    {
      QString ikey("output_keys");
      ObjectCommand toc(ikey);
      if (o->message(&toc))
        key->addItems(toc.getList(ikey));
    }

    connect(key, SIGNAL(currentIndexChanged(int)), this, SIGNAL(valueChanged()));
    _grid->addWidget(key, _keys.size(), 1);
    _keys.insert(k, key);
  }
  
  int pos = key->findText(d);
  if (pos == -1)
    pos = 0;
  key->setCurrentIndex(pos);
}

QString
InputObjectWidget::key (QString d)
{
  QComboBox *key = _keys.value(d);
  if (! key)
    return QString();

  return key->currentText();
}

void
InputObjectWidget::setOffset (int d)
{
  if (! _offset)
  {
    _offset = new QSpinBox;
    _offset->setToolTip(tr("Look Back"));
    _offset->setRange(0, 999999);
    connect(_offset, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged()));
    _grid->addWidget(_offset, 0, 2);
  }

  _offset->setValue(d);
}

int
InputObjectWidget::offset ()
{
  if (! _offset)
    return 0;

  return _offset->value();
}

void
InputObjectWidget::objectChanged ()
{
  Object *o = (Object *) _objects.value(_input->currentText());  
  if (! o)
    return;
  
  QString key("output_keys");
  ObjectCommand toc(key);
  if (! o->message(&toc))
    return;
 
  QHashIterator<QString, QComboBox *> it(_keys);
  while (it.hasNext())
  {
    it.next();
    QComboBox *k = it.value(); 
    k->clear();
    k->addItems(toc.getList(key));
  }
}
