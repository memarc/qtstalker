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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <QObject>
#include <QString>

#include "ObjectCommand.h"

class Object : public QObject
{
  Q_OBJECT

  signals:
    void signalMessage (ObjectCommand);
    
  public:
    Object ();
    ~Object ();
    void setProfile (QString);
    QString profile ();
    void setName (QString);
    QString name ();
    QString type ();
    QString plugin ();
    bool hasOutput ();
    virtual QWidget * widget ();
    
  public slots:
    virtual int message (ObjectCommand *);
    
  protected:
    QString _profile;
    QString _name;
    QString _type;
    QString _plugin;
    bool _hasOutput;
};

#endif
