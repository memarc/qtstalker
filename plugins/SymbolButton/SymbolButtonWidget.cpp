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

#include "SymbolButtonWidget.h"
#include "Util.h"
#include "../pics/chart.xpm"

#include <QDebug>

SymbolButtonWidget::SymbolButtonWidget ()
{
  Util util;
  _dialog = util.object(QString("SymbolSelect"), QString(), QString());
  if (_dialog)
    connect(_dialog, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(updateButtonText()));
  
  connect(this, SIGNAL(clicked()), this, SLOT(symbolDialog()));
  
  setIcon(QIcon(chart_xpm));
  
  updateButtonText();
}

SymbolButtonWidget::~SymbolButtonWidget ()
{
  if (_dialog)
    delete _dialog;
}

QStringList
SymbolButtonWidget::symbols ()
{
  if (! _dialog)
    return QStringList();
  
  ObjectCommand toc(QString("settings"));
  _dialog->message(&toc);
  
  return toc.getList(QString("symbols"));
}

void
SymbolButtonWidget::setSymbols (QStringList l)
{
  ObjectCommand toc(QString("settings"));
  _dialog->message(&toc);
  
  toc.setValue(QString("symbols"), l);
  
  toc.setCommand(QString("set_settings"));
  _dialog->message(&toc);
  
  updateButtonText();
}

void
SymbolButtonWidget::symbolDialog ()
{
  if (! _dialog)
    return;

  ObjectCommand toc(QString("dialog"));
  _dialog->message(&toc);
}

void
SymbolButtonWidget::updateButtonText ()
{
  if (! _dialog)
    return;
  
  QStringList tl;
  tl << QString::number(count()) << tr("symbols selected");
  setToolTip(tl.join(" "));
}

int
SymbolButtonWidget::count ()
{
  if (! _dialog)
    return 0;
  
  ObjectCommand toc(QString("size"));
  _dialog->message(&toc);
  
  return toc.getInt(QString("size"));
}
