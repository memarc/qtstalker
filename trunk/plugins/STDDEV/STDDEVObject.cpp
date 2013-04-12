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

#include "STDDEVObject.h"
#include "ta_libc.h"
#include "STDDEVDialog.h"
#include "Util.h"

#include <QtDebug>


STDDEVObject::STDDEVObject (QString profile, QString name)
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "STDDEVObject::STDDEVObject: error on TA_Initialize";
  
  _profile = profile;
  _name = name;
  _plugin = QString("STDDEV");
  _type = QString("indicator");
  _outputKey = QString("v");
  _hasOutput = TRUE;
  _inputObject = QString("symbol");
  _inputKey = QString("C");
  _period = 14;
  _dev = 2.0;
  
  _bars = new Bars;
  
  _commandList << QString("update");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");
}

STDDEVObject::~STDDEVObject ()
{
  delete _bars;
}

void
STDDEVObject::clear ()
{
  _bars->clear();
}

int
STDDEVObject::message (ObjectCommand *oc)
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
STDDEVObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *io = (Object *) oc->getObject(_inputObject);
  if (! io)
  {
    qDebug() << "STDDEVObject::update: invalid input" << _inputObject;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! io->message(&toc))
  {
    qDebug() << "STDDEVObject::update: message error" << io->plugin() << toc.command();
    return 0;
  }

  Bars *ibars = toc.getBars(_inputKey);
  if (! ibars)
  {
    qDebug() << "STDDEVObject::update: invalid input bars" << _inputKey;
    return 0;
  }

  int size = ibars->_bars.size();
  
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;
  int dpos = 0;
  QMapIterator<int, Bar *> it(ibars->_bars);
  while (it.hasNext())
  {
    it.next();
    Bar *d = it.value();
    input[dpos++] = (TA_Real) d->v;
  }
  
  TA_RetCode rc = TA_STDDEV(0,
			    dpos - 1,
			    &input[0],
			    _period,
                            _dev,
			    &outBeg,
			    &outNb,
			    &out[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "STDDEVObject::update: TA-Lib error" << rc;
    return 0;
  }

  int outLoop = outNb - 1;
  it.toBack();
  while (it.hasPrevious() && outLoop > -1)
  {
    it.previous();
    _bars->setValue(it.key(), (double) out[outLoop--]);
  }
  
  return 1;
}

int
STDDEVObject::dialog (ObjectCommand *oc)
{
  STDDEVDialog *dialog = new STDDEVDialog(oc->getObjects(), _name);
  dialog->setSettings(_inputObject, _inputKey, _period, _dev);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
STDDEVObject::dialogDone (void *dialog)
{
  STDDEVDialog *d = (STDDEVDialog *) dialog;
  d->settings(_inputObject, _inputKey, _period, _dev);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
STDDEVObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _outputKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
STDDEVObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setValue(_outputKey, _bars);
  return 1;
}

int
STDDEVObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "STDDEVObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _inputKey = settings->value(QString("input_key")).toString();
  _period = settings->value(QString("period")).toInt();
  _dev = settings->value(QString("dev")).toDouble();

  return 1;
}

int
STDDEVObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "STDDEVObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("input_key"), _inputKey);
  settings->setValue(QString("period"), _period);
  settings->setValue(QString("dev"), _dev);

  return 1;
}
