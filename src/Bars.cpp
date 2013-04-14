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

#include <QtDebug>

#include "Bars.h"

Bars::Bars ()
{
}

Bars::~Bars ()
{
  qDeleteAll(_bars);
}

void
Bars::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

void
Bars::setValue (int k, double d)
{
  Bar *tb = _bars.value(k);
  if (! tb)
  {
    tb = new Bar;
    _bars.insert(k, tb);
  }
  
  tb->v = d;
}

void
Bars::setDate (int k, QDateTime d)
{
  Bar *tb = _bars.value(k);
  if (! tb)
  {
    tb = new Bar;
    _bars.insert(k, tb);
  }
  
  tb->date = d;
}

void
Bars::setBar (int k, double v, QDateTime dt)
{
  Bar *tb = _bars.value(k);
  if (! tb)
  {
    tb = new Bar;
    _bars.insert(k, tb);
  }
  
  tb->v = v;
  tb->date = dt;
}

Bar *
Bars::value (int k)
{
  return _bars.value(k);
}

Bars *
Bars::copy ()
{
  Bars *bars = new Bars;
  
  QMapIterator<int, Bar *> it(_bars);
  while (it.hasNext())
  {
    it.next();
    Bar *b = it.value();
    bars->setBar(it.key(), b->v, b->date);
  }
  
  return bars;
}

void
Bars::startEndIndex (int &start, int &end)
{
  start = 0;
  end = 0;
  
  QMapIterator<int, Bar *> it(_bars);
  it.toFront();
  if (it.hasNext())
  {
    it.next();
    start = it.key();
  }
  
  it.toBack();
  if (it.hasPrevious())
  {
    it.previous();
    end = it.key();
  }
}
