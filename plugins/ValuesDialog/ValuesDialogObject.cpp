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

#include "ValuesDialogObject.h"
#include "Util.h"
#include "ValuesDialogDialog.h"

#include <QDebug>

ValuesDialogObject::ValuesDialogObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("ValuesDialog");
  _type = QString("dialog");

  _commandList << QString("dialog");
}

ValuesDialogObject::~ValuesDialogObject ()
{
}

int
ValuesDialogObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = dialog(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
ValuesDialogObject::dialog (ObjectCommand *oc)
{
  QStringList header;
  QMap<int, Data *> data;

  QHashIterator<QString, void *> it(oc->getObjects());
  while (it.hasNext())
  {
    it.next();
    Object *o = (Object *) it.value();

    ObjectCommand toc(QString("output"));
    if (! o->message(&toc))
    {
      qDebug() << "ValuesDialogObject::dialog" << o->plugin() << toc.command();
      continue;
    }

    QStringList keys = toc.getList(QString("output_keys"));
    QStringList labels = toc.getList(QString("output_labels"));
    if (keys.size() != labels.size())
    {
      qDebug() << "ValuesDialogObject::dialog: output_keys / output_labels size mismatch" << keys << labels;
      continue;
    }

    header += labels;

    QMapIterator<int, Data *> mit(toc.map());
    while (mit.hasNext())
    {
      mit.next();
      Data *d = mit.value();
      
      Data *nd = data.value(mit.key());
      if (! nd)
      {
        nd = new Data;
        data.insert(mit.key(), nd);
      }

      for (int pos = 0; pos < keys.size(); pos++)
        nd->insert(labels.at(pos), d->value(keys.at(pos)));
    }
  }

  header.sort();

  ValuesDialogDialog *dialog = new ValuesDialogDialog(_profile, _name);
  dialog->setModified(TRUE);
  dialog->show();
  dialog->setData(header, data);

  qDeleteAll(data);

  return 1;
}

