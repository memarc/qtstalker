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

#include "HTObject.h"
#include "ta_libc.h"
#include "HTDialog.h"
#include "Util.h"

#include <QtDebug>


HTObject::HTObject (QString profile, QString name)
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "HTObject::HTObject: error on TA_Initialize";
  
  _profile = profile;
  _name = name;
  _plugin = QString("HT");
  _type = QString("indicator");
  _outputKey = QString("v");
  _hasOutput = TRUE;
  _inputObject = QString("symbol");
  _inputKey = QString("C");
  _method = QString("TRENDLINE");
  
  _commandList << QString("update");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");

  _methodList << QString("DCPERIOD");
  _methodList << QString("DCPHASE");
  _methodList << QString("TRENDLINE");
  _methodList << QString("TRENDMODE");
}

HTObject::~HTObject ()
{
  clear();
}

void
HTObject::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

int
HTObject::message (ObjectCommand *oc)
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
HTObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *io = (Object *) oc->getObject(_inputObject);
  if (! io)
  {
    qDebug() << "HTObject::update: invalid input" << _inputObject;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! io->message(&toc))
  {
    qDebug() << "HTObject::update: message error" << io->plugin() << toc.command();
    return 0;
  }

  QMap<int, Data *> data = toc.map();
  
  int size = data.size();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer iout[size];
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
  
  TA_RetCode rc = TA_SUCCESS;
  switch (_methodList.indexOf(_method))
  {
    case 0:
      rc = TA_HT_DCPERIOD (0, dpos - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_HT_DCPHASE (0, dpos - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 2:
      rc = TA_HT_TRENDLINE (0, dpos - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 3:
      rc = TA_HT_TRENDMODE (0, dpos - 1, &input[0], &outBeg, &outNb, &iout[0]);
      break;
    default:
      break;
  }
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "HTObject::update: TA-Lib error" << rc;
    return 0;
  }

  int outLoop = outNb - 1;
  it.toBack();

  switch (_methodList.indexOf(_method))
  {
    case 0:
    case 1:
    case 2:
    {
      while (it.hasPrevious() && outLoop > -1)
      {
        it.previous();
        Data *b = new Data;
        b->insert(_outputKey, out[outLoop--]);
        _bars.insert(it.key(), b);
      }
      break;
    }
    case 3:
    {
      while (it.hasPrevious() && outLoop > -1)
      {
        it.previous();
        Data *b = new Data;
        b->insert(_outputKey, iout[outLoop--]);
        _bars.insert(it.key(), b);
      }
      break;
    }
    default:
      break;
  }

  return 1;
}

int
HTObject::dialog (ObjectCommand *oc)
{
  HTDialog *dialog = new HTDialog(oc->getObjects(), _methodList, _name);
  dialog->setSettings(_inputObject, _inputKey, _method);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
HTObject::dialogDone (void *dialog)
{
  HTDialog *d = (HTDialog *) dialog;
  d->settings(_inputObject, _inputKey, _method);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
HTObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _outputKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
HTObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setMap(_bars);
  return 1;
}

int
HTObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "HTObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _inputKey = settings->value(QString("input_key")).toString();
  _method = settings->value(QString("method")).toString();

  return 1;
}

int
HTObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "HTObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("input_key"), _inputKey);
  settings->setValue(QString("method"), _method);
 
  return 1;
}
