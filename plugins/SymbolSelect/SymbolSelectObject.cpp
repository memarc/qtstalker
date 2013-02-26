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

#include "SymbolSelectObject.h"
#include "SymbolSelectDialog.h"

#include <QtDebug>
#include <QSettings>


SymbolSelectObject::SymbolSelectObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("SymbolSelect");
  _type = QString("dialog");
  _exchange = QString("%");
  _ticker = QString("%");
  _corp = QString("%");
  _corpType = QString("%");
  
  _commandList << QString("dialog");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("set_settings");
  _commandList << QString("settings");
  _commandList << QString("size");
}

SymbolSelectObject::~SymbolSelectObject ()
{
}

int
SymbolSelectObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = dialog(oc);
      break;
    case 1:
      rc = load(oc);
      break;
    case 2:
      rc = save(oc);
      break;
    case 3:
      rc = setSettings(oc);
      break;
    case 4:
      rc = settings(oc);
      break;
    case 5:
      rc = size(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
SymbolSelectObject::dialog (ObjectCommand *)
{
  SymbolSelectDialog *d = new SymbolSelectDialog(QString());
  connect(d, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  d->setSettings(_symbols, _exchange, _ticker, _corpType, _corp);
  d->show();
  return 1;
}

void
SymbolSelectObject::dialogDone (void *dialog)
{
  SymbolSelectDialog *d = (SymbolSelectDialog *) dialog;
  d->settings(_symbols, _exchange, _ticker, _corpType, _corp);
  symbolsChanged();
}

int
SymbolSelectObject::setSettings (ObjectCommand *oc)
{
  _symbols = oc->getList(QString("symbols"));
  _exchange = oc->getString(QString("exchange"));
  _ticker = oc->getString(QString("ticker"));
  _corp = oc->getString(QString("name"));
  _corpType = oc->getString(QString("type"));
  
  return 1;
}

int
SymbolSelectObject::settings (ObjectCommand *oc)
{
  oc->setValue(QString("symbols"), _symbols);
  oc->setValue(QString("exchange"), _exchange);
  oc->setValue(QString("ticker"), _ticker);
  oc->setValue(QString("name"), _corp);
  oc->setValue(QString("type"), _corpType);
  
  return 1;
}

int
SymbolSelectObject::load (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "SymbolSelectObject::load: invalid" << key;
    return 0;
  }

  _symbols = settings->value(QString("symbols")).toStringList();
  _exchange = settings->value(QString("exchange")).toString();
  _ticker = settings->value(QString("ticker")).toString();
  _corp = settings->value(QString("corp")).toString();
  _corpType = settings->value(QString("corp_type")).toString();
  
  return 1;
}

int
SymbolSelectObject::save (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "SymbolSelectObject::save: invalid" << key;
    return 0;
  }

  settings->setValue(QString("symbols"), _symbols);
  settings->setValue(QString("exchange"), _exchange);
  settings->setValue(QString("ticker"), _ticker);
  settings->setValue(QString("corp"), _corp);
  settings->setValue(QString("corp_type"), _corpType);
  
  return 1;
}

void
SymbolSelectObject::symbolsChanged ()
{
  ObjectCommand toc(QString("symbols_changed"));
  emit signalMessage(toc);
}

int
SymbolSelectObject::size (ObjectCommand *oc)
{
  oc->setValue(QString("size"), _symbols.size());
  return 1;
}
