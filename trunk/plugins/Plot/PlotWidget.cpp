/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "PlotWidget.h"
#include "Util.h"
#include "ObjectCommand.h"

#include <QDebug>


PlotWidget::PlotWidget (QWidget *w, Object *plotObject) : QWidget (w)
{
  _xmap = new XMap;
  _ymap = new YMap;

  // create layout
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(0);
  vbox->setMargin(0);
  setLayout(vbox);
  
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(0);
  hbox->setMargin(0);
  vbox->addLayout(hbox);

  // create the scale object
  _plotScale = new PlotWidgetScale(this, _ymap);
  
  // create view
  _plotArea = new PlotWidgetArea(this, _xmap, _ymap, _plotScale, plotObject);  
  hbox->addWidget(_plotArea);
  hbox->addWidget(_plotScale);
 
  // create the date object
  _plotDate = new PlotWidgetDate(this, _xmap);
  vbox->addWidget(_plotDate);

  // connnect all the signals after all objects created - just to be safe
  connect(_plotArea, SIGNAL(signalDateChanged(bool)), this, SLOT(dateToggled(bool)));
}

PlotWidget::~PlotWidget ()
{
  clear();
  delete _xmap;
  delete _ymap;
}

void
PlotWidget::clear ()
{
  _ymap->clear();
  _xmap->clear();
  _plotArea->clear();
  _plotScale->clear();
  _plotDate->clear();
}

void
PlotWidget::draw ()
{
  _plotArea->draw();
  _plotScale->draw();
  _plotDate->draw();
}

void
PlotWidget::setDates (Bars *d, QString key, QString length)
{
  clear();
  _plotDate->setDates(d, key);
  _xmap->setDates(d, key, length);
}

PlotWidgetArea *
PlotWidget::plotArea ()
{
  return _plotArea;
}

PlotWidgetScale *
PlotWidget::plotScale ()
{
  return _plotScale;
}

PlotWidgetDate *
PlotWidget::plotDate ()
{
  return _plotDate;
}

YMap *
PlotWidget::getYMap ()
{
  return _ymap;
}

XMap *
PlotWidget::getXMap ()
{
  return _xmap;
}

void
PlotWidget::setSpacing (int d)
{
  _xmap->setSpacing(d);
}

int
PlotWidget::spacing ()
{
  return _xmap->spacing();
}

void
PlotWidget::setIndex (int d)
{
  _xmap->setIndex(d);
}

int
PlotWidget::index ()
{
  return _xmap->index();
}

void
PlotWidget::setName (QString d)
{
  _plotArea->setName(d);
}

QString
PlotWidget::name ()
{
  return _plotArea->name();
}

void
PlotWidget::dateToggled (bool d)
{
  _plotDate->setVisible(d);
  _plotDate->draw();
  emit signalSettingsChanged();
}
