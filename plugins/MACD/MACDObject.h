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

#ifndef PLUGIN_MACD_OBJECT_HPP
#define PLUGIN_MACD_OBJECT_HPP

#include <QStringList>

#include "Object.h"
#include "MACDDialog.h"
#include "Bars.h"

class MACDObject : public Object
{
  Q_OBJECT

  public:
    MACDObject (QString profile, QString name);
    ~MACDObject ();
    void clear ();
    int update (ObjectCommand *);
    int dialog (ObjectCommand *);
    int output (ObjectCommand *);
    int load (ObjectCommand *);
    int save (ObjectCommand *);
    int outputKeys (ObjectCommand *);
    
  public slots:    
    int message (ObjectCommand *);
    void dialogDone (void *);
    
  private:
    QStringList _commandList;
    QStringList _maList;
    Bars *_mbars;
    Bars *_sbars;
    Bars *_hbars;
    QString _inputObject;
    QString _inputKey;
    QString _slowMAType;
    QString _fastMAType;
    QString _signalMAType;
    QString _macdKey;
    QString _signalKey;
    QString _histKey;
    int _fastPeriod;
    int _slowPeriod;
    int _signalPeriod;
};

#endif
