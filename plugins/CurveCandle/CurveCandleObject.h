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

#ifndef PLUGIN_CURVE_CANDLE_OBJECT_HPP
#define PLUGIN_CURVE_CANDLE_OBJECT_HPP

#include <QStringList>
#include <QMap>
#include <QColor>
#include <QPainter>

#include "Object.h"
#include "CurveCandleDialog.h"


typedef struct
{
  bool fill;
  QColor color;
  double open;
  double high;
  double low;
  double close;
  
} CandleBar;

class CurveCandleObject : public Object
{
  Q_OBJECT

  public:
    CurveCandleObject (QString profile, QString name);
    ~CurveCandleObject ();
    void clear ();
    int draw (ObjectCommand *);
    int info (ObjectCommand *);
    int highLowRange (ObjectCommand *);
    int update (ObjectCommand *);
    int scalePoint (ObjectCommand *);
    int startEndIndexMessage (ObjectCommand *);
    int load (ObjectCommand *);
    int save (ObjectCommand *);
    int dialog (ObjectCommand *);
    int copy (ObjectCommand *);
    int setColor (ObjectCommand *);
    int settings (ObjectCommand *);
    
    int startEndIndex (int &start, int &end);
    QMap<int, CandleBar *> _bars;
    
  public slots:
    int message (ObjectCommand *);
    void dialogDone (void *);
    
  private:
    QStringList _commandList;
    int _penWidth;
    QString _inputObject;
    QString _openKey;
    QString _highKey;
    QString _lowKey;
    QString _closeKey;
    QString _label;
    QString _plotObject;
    QColor _color;
};

#endif
