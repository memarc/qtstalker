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

#include "PopupWidget.h"

#include <QtDebug>

PopupWidget::PopupWidget (QWidget *p) : QMenu (p)
{
  _modified = FALSE;
  connect(this, SIGNAL(aboutToShow()), this, SLOT(clear()));
  
  createGUI();
}

void
PopupWidget::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(10);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(0);
  vbox->addLayout(form);
  
  _range = new QComboBox;
  connect(_range, SIGNAL(currentIndexChanged(int)), this, SLOT(setModified()));
  form->addRow(tr("Range"), _range);
  
  _custom = new QGroupBox;
  _custom->setCheckable(TRUE);
  _custom->setChecked(FALSE);
  _custom->setTitle(tr("Custom"));
  connect(_custom, SIGNAL(toggled(bool)), this, SLOT(setModified()));
  connect(_custom, SIGNAL(toggled(bool)), this, SLOT(customChanged(bool)));
  vbox->addWidget(_custom);
  
  form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  _custom->setLayout(form);
  
  _startDate = new QDateTimeEdit;
  connect(_startDate, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(setModified()));
  form->addRow(tr("Start Date"), _startDate);
  
  _endDate = new QDateTimeEdit;
  connect(_endDate, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(setModified()));
  form->addRow(tr("End Date"), _endDate);
}

void
PopupWidget::setSettings (QStringList l, QString range, QDateTime sd, QDateTime ed, bool cus)
{
  _range->addItems(l);
  _range->setCurrentIndex(_range->findText(range));
  
  _startDate->setDateTime(sd);
  _endDate->setDateTime(ed);
  _custom->setChecked(cus);
  
  _modified = FALSE;
}

QString
PopupWidget::range ()
{
  return _range->currentText();
}

QDateTime
PopupWidget::startDate ()
{
  return _startDate->dateTime();
}

QDateTime
PopupWidget::endDate ()
{
  return _endDate->dateTime();
}

bool
PopupWidget::custom ()
{
  return _custom->isChecked();
}

void
PopupWidget::setModified ()
{
  _modified = TRUE;
}

bool
PopupWidget::modified ()
{
  return _modified;
}

void
PopupWidget::clear ()
{
  _modified = FALSE;
}

void
PopupWidget::customChanged (bool d)
{
  if (d)
    _range->setEnabled(FALSE);
  else
    _range->setEnabled(TRUE);
}
