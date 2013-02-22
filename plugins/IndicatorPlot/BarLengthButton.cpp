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

#include "BarLengthButton.h"
#include "Util.h"

#include <QDebug>
#include <QSettings>

BarLengthButton::BarLengthButton (QString sp)
{
  _settingsPath = sp;
  
  setPopupMode(QToolButton::InstantPopup);
  setToolTip(tr("Bar Length"));
  setStatusTip(tr("Bar Length"));
  
  QFont font;
  font.setPointSize(9);
  setFont(font);
  
  Util util;
  Object *o = util.object(QString("Symbol"), QString(), QString());
  if (o)
  {
    QString key("list");
    ObjectCommand toc(key);
    if (o->message(&toc))
      _list = toc.getList(key);
    delete o;
  }

  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("bar_length_button"));
  _length = settings.value(QString("length"), QString("D")).toString();
  _interval = settings.value(QString("interval"), 1).toInt();
  
  createMenu();
}

BarLengthButton::~BarLengthButton ()
{
  saveSettings();
}

void
BarLengthButton::createMenu ()
{
  _menu = new QMenu(this);
  _menu->setTitle(tr("Bar &Length"));
  connect(_menu, SIGNAL(triggered(QAction *)), this, SLOT(lengthChanged(QAction *)));
  setMenu(_menu);

  QActionGroup *group = new QActionGroup(this);

  int loop = 0;
  for (; loop < _list.count(); loop++)
  {
    QAction *a = _menu->addAction(_list.at(loop));
    a->setCheckable(TRUE);
    group->addAction(a);

    if (_list.at(loop) == _length)
    {
      a->setChecked(TRUE);
      setText(_list.at(loop));
    }
  }
}

void
BarLengthButton::lengthChanged (QAction *d)
{
  _length = d->text();
  setText(_length);
  emit signalLength();
}

QString
BarLengthButton::length ()
{
  return _length;
}

int
BarLengthButton::interval ()
{
  return _interval;
}

void
BarLengthButton::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("bar_length_button"));
  settings.setValue(QString("length"), _length);
  settings.setValue(QString("interval"), _interval);
  settings.sync();
}
