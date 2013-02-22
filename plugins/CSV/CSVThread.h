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

#ifndef PLUGIN_CSV_THREAD_HPP
#define PLUGIN_CSV_THREAD_HPP

#include <QThread>
#include <QStringList>

#include "ObjectCommand.h"

class CSVThread : public QThread
{
  Q_OBJECT
  
  signals:
    void signalMessage (ObjectCommand);
    void signalDone ();
  
  public:
    CSVThread (QObject *, QStringList files, QString format, QString dateFormat,
               QString delimiter, QString type, QString exchange, bool useFilename);
    void run ();
    
  public slots:
    void stop ();
    
  private:
    QStringList _files;
    QString _format;
    QString _dateFormat;
    QString _delimiter;
    QString _type;
    QString _exchange;
    bool _useFilename;
    bool _stop;
};

#endif
