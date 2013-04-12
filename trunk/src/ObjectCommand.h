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

#ifndef OBJECT_COMMAND_HPP
#define OBJECT_COMMAND_HPP

#include <QStringList>
#include <QHash>
#include <QMap>
#include <QColor>
#include <QFont>
#include <QIcon>
#include <QDateTime>

#include "Data.h"
#include "Bars.h"

class ObjectCommand
{
  public:
    ObjectCommand ();
    ObjectCommand (QString command);
    ~ObjectCommand ();
    void clear ();
    void clearValues ();
    void setCommand (QString);
    QString command ();
    void setMsg (QString);
    QString msg ();
    void setValue(QString, int);
    void setValue(QString, double);
    void setValue(QString, bool);
    void setValue(QString, QString);
    void setValue(QString, QStringList);
    void setValue(QString, void *);
    void setValue(QString, QColor);
    void setValue(QString, Data);
    void setValue(QString, QDateTime);
    void setValue(QString, QIcon);
    void setValue(QString, QFont);
    void setValue(QString, Bars *);
    int getInt (QString);
    double getDouble (QString);
    bool getBool (QString);
    QString getString (QString);
    QStringList getList (QString);
    void * getObject (QString);
    QColor getColor (QString);
    Data getData (QString);
    QDateTime getDate (QString);
    QIcon getIcon (QString);
    QFont getFont (QString);
    void setObjects (QHash<QString, void *>);
    QHash<QString, void *> getObjects ();
    QHash<QString, Data> getDatas ();
    void setDatas (QHash<QString, Data>);
    Bars * getBars (QString);

  protected:
    QString _command;
    QString _msg;
    QHash<QString, int> _ints;
    QHash<QString, double> _doubles;
    QHash<QString, QString> _strings;
    QHash<QString, QStringList> _lists;
    QHash<QString, void *> _objects;
    QHash<QString, QColor> _colors;
    QHash<QString, Data> _datas;
    QHash<QString, QDateTime> _dates;
    QHash<QString, bool> _bools;
    QHash<QString, QIcon> _icons;
    QHash<QString, QFont> _fonts;
    QHash<QString, Bars *> _bars;
};

Q_DECLARE_METATYPE(ObjectCommand)

#endif
