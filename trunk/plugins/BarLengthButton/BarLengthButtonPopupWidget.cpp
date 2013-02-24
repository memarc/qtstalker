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

#include "BarLengthButtonPopupWidget.h"

#include <QtDebug>

BarLengthButtonPopupWidget::BarLengthButtonPopupWidget (QWidget *p) : QMenu (p)
{
  _modified = FALSE;
  connect(this, SIGNAL(aboutToShow()), this, SLOT(clear()));
  
  createGUI();
}

void
BarLengthButtonPopupWidget::createGUI ()
{
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(10);
  setLayout(form);
  
  _length = new QComboBox;
  connect(_length, SIGNAL(currentIndexChanged(int)), this, SLOT(setModified()));
  form->addRow(tr("Bar Length"), _length);
}

void
BarLengthButtonPopupWidget::setSettings (QStringList l, QString length)
{
  _length->addItems(l);
  _length->setCurrentIndex(_length->findText(length));
  _modified = FALSE;
}

QString
BarLengthButtonPopupWidget::length ()
{
  return _length->currentText();
}

void
BarLengthButtonPopupWidget::setModified ()
{
  _modified = TRUE;
}

bool
BarLengthButtonPopupWidget::modified ()
{
  return _modified;
}

void
BarLengthButtonPopupWidget::clear ()
{
  _modified = FALSE;
}
