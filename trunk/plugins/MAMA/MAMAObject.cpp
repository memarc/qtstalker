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

#include "MAMAObject.h"
#include "ta_libc.h"
#include "MAMADialog.h"
#include "Util.h"

#include <QtDebug>


MAMAObject::MAMAObject (QString profile, QString name)
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "MAMAObject::MAMAObject: error on TA_Initialize";
  
  _profile = profile;
  _name = name;
  _plugin = QString("MAMA");
  _type = QString("indicator");
  _fast = 0.5;
  _slow = 0.05;
  _mamaKey = QString("m");
  _famaKey = QString("f");
  _hasOutput = TRUE;
  _inputObject = QString("symbol");
  _inputKey = QString("C");
  
  _commandList << QString("update");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");
}

MAMAObject::~MAMAObject ()
{
  clear();
}

void
MAMAObject::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

int
MAMAObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = update(oc);
      break;
    case 1:
      rc = dialog(oc);
      break;
    case 2:
      rc = output(oc);
      break;
    case 3:
      rc = load(oc);
      break;
    case 4:
      rc = save(oc);
      break;
    case 5:
      rc = outputKeys(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
MAMAObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *io = (Object *) oc->getObject(_inputObject);
  if (! io)
  {
    qDebug() << "MAMAObject::update: invalid input" << _inputObject;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! io->message(&toc))
  {
    qDebug() << "MAMAObject::update: message error" << io->plugin() << toc.command();
    return 0;
  }

  QMap<int, Data *> data = toc.map();
  int size = data.size();
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;
  int dpos = 0;
  QMapIterator<int, Data *> it(data);
  while (it.hasNext())
  {
    it.next();
    Data *d = it.value();
    
    if (d->contains(_inputKey))
      input[dpos++] = (TA_Real) d->value(_inputKey).toDouble();
  }
  
  TA_RetCode rc = TA_MAMA(0,
			  dpos - 1,
			  &input[0],
			  _fast,
			  _slow,
			  &outBeg,
			  &outNb,
    			  &out[0],
                          &out2[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MAMAObject::update: TA-Lib error" << rc;
    return 0;
  }

  int outLoop = outNb - 1;
  it.toBack();
  while (it.hasPrevious() && outLoop > -1)
  {
    it.previous();
    Data *b = new Data;
    b->insert(_mamaKey, out[outLoop]);
    b->insert(_famaKey, out2[outLoop--]);
    _bars.insert(it.key(), b);
  }
  
  return 1;
}

int
MAMAObject::dialog (ObjectCommand *oc)
{
  MAMADialog *dialog = new MAMADialog(oc->getObjects(), _name);
  dialog->setSettings(_inputObject, _inputKey, _fast, _slow);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
MAMAObject::dialogDone (void *dialog)
{
  MAMADialog *d = (MAMADialog *) dialog;
  d->settings(_inputObject, _inputKey, _fast, _slow);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
MAMAObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _mamaKey << _famaKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
MAMAObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setMap(_bars);
  return 1;
}

int
MAMAObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MAMAObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _inputKey = settings->value(QString("input_key")).toString();
  _fast = settings->value(QString("fast")).toDouble();
  _slow = settings->value(QString("slow")).toDouble();

  return 1;
}

int
MAMAObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MAMAObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("input_key"), _inputKey);
  settings->setValue(QString("fast"), _fast);
  settings->setValue(QString("slow"), _slow);

  return 1;
}
