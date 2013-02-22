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

#include "YahooHistoryThread.h"
#include "Util.h"

#include <QDebug>

YahooHistoryThread::YahooHistoryThread (QObject *p, QStringList symbols, QDateTime sd, QDateTime ed) : QThread (p)
{
  _stop = FALSE;
  _symbols = symbols;
  _startDate = sd;
  _endDate = ed;
  
  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void
YahooHistoryThread::run ()
{
  _stop = FALSE;
  
  Util util;
  Object *yho = util.object(QString("YahooHistory"), QString(), QString());
  if (! yho)
    return;
  
  connect(yho, SIGNAL(signalMessage(ObjectCommand)), this, SIGNAL(signalMessage(ObjectCommand)));
  
  for (int pos = 0; pos < _symbols.size(); pos++)
  {
    emit signalProgress(pos);
    
    if (_stop)
      break;
    
    ObjectCommand oc(QString("download"));
    oc.setValue(QString("symbol"), _symbols.at(pos));
    oc.setValue(QString("start_date"), _startDate);
    oc.setValue(QString("end_date"), _endDate);
    yho->message(&oc);
  }
  
  delete yho;
  
  emit signalDone();
}

void
YahooHistoryThread::stop ()
{
  _stop = TRUE;
}
