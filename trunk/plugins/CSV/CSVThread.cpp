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

#include "CSVThread.h"
#include "CSVObject.h"
#include "Util.h"

#include <QDebug>

CSVThread::CSVThread (QObject *p, QStringList files, QString format, QString dateFormat,
                      QString delimiter, QString type, QString exchange, bool useFilename) : QThread (p)
{
  _stop = FALSE;
  _files = files;
  _format = format;
  _dateFormat = dateFormat;
  _delimiter = delimiter;
  _type = type;
  _exchange = exchange;
  _useFilename = useFilename;
  
  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void
CSVThread::run ()
{
  _stop = FALSE;
  
  Util util;
  Object *csv = util.object(QString("CSV"), QString(), QString());
  if (! csv)
    return;
  
  connect(csv, SIGNAL(signalMessage(ObjectCommand)), this, SIGNAL(signalMessage(ObjectCommand)));
  
  for (int pos = 0; pos < _files.size(); pos++)
  {
    if (_stop)
      break;
  
    ObjectCommand toc(QString("import"));
    toc.setValue(QString("files"), QStringList() << _files.at(pos));
    toc.setValue(QString("format"), _format);
    toc.setValue(QString("date_format"), _dateFormat);
    toc.setValue(QString("delimiter"), _delimiter);
    toc.setValue(QString("type"), _type);
    toc.setValue(QString("exchange"), _exchange);
    toc.setValue(QString("use_filename"), _useFilename);
    csv->message(&toc);
  }
  
  delete csv;
  
  emit signalDone();
}

void
CSVThread::stop ()
{
  _stop = TRUE;
}
