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

#include "PlotWidgetCursorInfo.h"

#include <QDebug>
#include <QDateTime>


PlotWidgetCursorInfo::PlotWidgetCursorInfo (QObject *p, XMap *xmap) : QObject(p)
{
  _xmap = xmap;
  _enabled = TRUE;
  _color = QColor(Qt::white);
  
  _font.setFamily("Helvetica");
  _font.setPointSize(10);
  _font.setWeight(50);
  
  _fields << tr("Plot") << tr("D") << tr("T") << tr("O");
  _fields << tr("H") << tr("L") << tr("C") << tr("V") << tr("I");
}

PlotWidgetCursorInfo::~PlotWidgetCursorInfo ()
{
}

void
PlotWidgetCursorInfo::clear ()
{
  _info.clear();
}

void
PlotWidgetCursorInfo::draw (QPainter *p)
{
  // we draw directly onto the widget in the upper left hand corner

  if (! _enabled)
    return;
  
  p->setFont(_font);
  p->setPen(_color);
  
  QFontMetrics fm(_font);

  QList<QString> keys = _info.keys();
  qSort(keys);
  
  int x = 0;
  int y = fm.height() - 2;
  for (int pos = 0; pos < _fields.size(); pos++)
  {
    if (_info.contains(_fields.at(pos)))
    {
      QVariant v = _info.value(_fields.at(pos));
      p->drawText(x, y, QString(_fields.at(pos) + "=" + v.toString()));
      keys.removeAll(_fields.at(pos));
      y += fm.height() - 2;
    }
  }
  
  for (int pos = 0; pos < keys.size(); pos++, y += fm.height() - 2)
  {
    QVariant v = _info.value(keys.at(pos));
    p->drawText(x, y, QString(keys.at(pos) + "=" + v.toString()));
  }
}

void
PlotWidgetCursorInfo::setInfo (QMouseEvent *event, QHash<QString, Object *> &items, QString &)
{
  _info.clear();
  
  // plot name
//  _info.insert(tr("Plot"), QVariant(name));

  int index = _xmap->xToIndex(event->pos().x());

  ObjectCommand oc(QString("info"));
  oc.setValue(QString("event"), (void *) event);
  oc.setValue(QString("index"), index);
  
  QHashIterator<QString, Object *> it(items);
  while (it.hasNext())
  {
    it.next();
    Object *item = it.value();

    if (! item->message(&oc))
      continue;

    Data i = oc.getData(QString("info"));
    
    QHashIterator<QString, QVariant> it2(i);
    while (it2.hasNext())
    {
      it2.next();
      QVariant v = it2.value();
      _info.insert(it2.key(), v);
    }
  }

  // date
  QDateTime dt = _xmap->indexToDate(index);
  if (dt.isValid())
  {
    _info.insert(tr("D"), QVariant(dt.toString("yyyy-MM-dd")));
    _info.insert(tr("T"), QVariant(dt.toString("HH:mm:ss")));
  }
  
  emit signalDraw();
  
  emit signalInfo(_info);
}

void
PlotWidgetCursorInfo::setEnabled (bool d)
{
  _enabled = d;
}

bool
PlotWidgetCursorInfo::enabled ()
{
  return _enabled;
}

void
PlotWidgetCursorInfo::setColor (QColor d)
{
  _color = d;
}

QColor
PlotWidgetCursorInfo::color ()
{
  return _color;
}

void
PlotWidgetCursorInfo::setFont (QFont d)
{
  _font = d;
}

QFont
PlotWidgetCursorInfo::font ()
{
  return _font;
}
