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

#ifndef PLUGIN_CURVE_BAR_OBJECT_HPP
#define PLUGIN_CURVE_BAR_OBJECT_HPP

#include <QStringList>
#include <QMap>
#include <QColor>
#include <QPainter>

#include "Object.h"
#include "CurveBarDialog.h"


typedef struct
{
  QColor color;
  float value;
  
} BVBar;

class CurveBarObject : public Object
{
  Q_OBJECT

  public:
    CurveBarObject (QString profile, QString name);
    ~CurveBarObject ();
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
    QMap<int, BVBar *> _bars;

  public slots:
    int message (ObjectCommand *);
    void dialogDone (void *);
    
  private:
    QStringList _commandList;
    int _penWidth;
    QColor _color;
    QString _label;
    QString _inputObject;
    QString _inputKey;
    QString _plotObject;
};

#endif
