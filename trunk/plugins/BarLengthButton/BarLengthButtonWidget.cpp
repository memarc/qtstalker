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

#include "BarLengthButtonWidget.h"

#include <QtDebug>


BarLengthButtonWidget::BarLengthButtonWidget (QString profile)
{
  _profile = profile;
  
  createGUI();
}

BarLengthButtonWidget::~BarLengthButtonWidget ()
{
}

void
BarLengthButtonWidget::createGUI ()
{
  QFont font;
  font.setPointSize(9);
  setFont(font);

  setToolTip(tr("Bar Length"));
  setStatusTip(tr("Bar Length"));
  setMaximumSize(QSize(25, 25));
  
  _popup = new BarLengthButtonPopupWidget(this);
  setMenu(_popup);
  connect(this, SIGNAL(clicked()), this, SLOT(showPopup()));
  connect(_popup, SIGNAL(aboutToHide()), this, SLOT(popupChanged()));
}

void
BarLengthButtonWidget::setSettings (QStringList l, QString length)
{
  _popup->setSettings(l, length);
  setText(length);
}

QString
BarLengthButtonWidget::length ()
{
  return _popup->length();
}

void
BarLengthButtonWidget::showPopup ()
{
  // try to center widget under the cursor
  QPoint p = QCursor::pos();
  p.setX(p.x() - (_popup->width() / 2));
  p.setY(p.y() - (_popup->height() / 2));
  _popup->exec(p);
}

void
BarLengthButtonWidget::popupChanged ()
{
  setText(_popup->length());
  emit signalLength();
}
