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

#ifndef PLUGIN_COMMAND_PARSE_HPP
#define PLUGIN_COMMAND_PARSE_HPP

#include <QStringList>
#include <QHash>

class CommandParse
{
  public:
    enum Verb
    {
      _CREATE,
      _GET,
      _SET,
      _RUN
    };
    
    CommandParse ();
    int parse (QString);
    QString value ();
    QString prop (int);
    int props ();
    QString object ();
    QString command ();
    int verb ();

  private:
    int _verb;
    QString _object;
    QString _command;
    QString _value;
    QHash<int, QString> _props;
};

#endif
