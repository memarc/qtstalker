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

#include "AROONObject.h"
#include "ta_libc.h"
#include "AROONDialog.h"
#include "Util.h"

#include <QtDebug>


AROONObject::AROONObject (QString profile, QString name)
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "AROONObject::AROONObject: error on TA_Initialize";
  
  _profile = profile;
  _name = name;
  _plugin = QString("AROON");
  _type = QString("indicator");
  _period = 14;
  _outputUpKey = QString("u");
  _outputDownKey = QString("d");
  _hasOutput = TRUE;
  _inputObject = QString("symbol");
  _highKey = QString("H");
  _lowKey = QString("L");
  
  _ubars = new Bars;
  _dbars = new Bars;
  
  _commandList << QString("update");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");
}

AROONObject::~AROONObject ()
{
  delete _ubars;
  delete _dbars;
}

void
AROONObject::clear ()
{
  _ubars->clear();
  _dbars->clear();
}

int
AROONObject::message (ObjectCommand *oc)
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
AROONObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *io = (Object *) oc->getObject(_inputObject);
  if (! io)
  {
    qDebug() << "AROONObject::update: invalid input" << _inputObject;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! io->message(&toc))
  {
    qDebug() << "AROONObject::update: message error" << io->plugin() << toc.command();
    return 0;
  }

  Bars *hbars = toc.getBars(_highKey);
  if (! hbars)
  {
    qDebug() << "AROONObject::update: invalid high bars" << _highKey;
    return 0;
  }

  Bars *lbars = toc.getBars(_lowKey);
  if (! lbars)
  {
    qDebug() << "AROONObject::update: invalid low bars" << _lowKey;
    return 0;
  }
  
  QList<int> keys = hbars->_bars.keys();
  int size = keys.size();

  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;
  int dpos = 0;
  for (int pos = 0; pos < keys.size(); pos++)
  {
    Bar *hbar = hbars->value(keys.at(pos));
    if (! hbar)
      continue;

    Bar *lbar = lbars->value(keys.at(pos));
    if (! lbar)
      continue;

    high[dpos] = (TA_Real) hbar->v;
    low[dpos++] = (TA_Real) lbar->v;
  }
  
  TA_RetCode rc = TA_AROON(0,
			   dpos - 1,
 			   &high[0],
			   &low[0],
			   _period,
			   &outBeg,
			   &outNb,
			   &out[0],
                           &out2[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "AROONObject::update: TA-Lib error" << rc;
    return 0;
  }

  int outLoop = outNb - 1;
  int pos = keys.size() - 1;
  while (pos > -1 && outLoop > -1)
  {
    _dbars->setValue(keys.at(pos), (double) out[outLoop]);
    _ubars->setValue(keys.at(pos), (double) out2[outLoop]);
    pos--;
    outLoop--;
  }
  
  return 1;
}

int
AROONObject::dialog (ObjectCommand *oc)
{
  AROONDialog *dialog = new AROONDialog(oc->getObjects(), _name);
  dialog->setSettings(_inputObject, _highKey, _lowKey, _period);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
AROONObject::dialogDone (void *dialog)
{
  AROONDialog *d = (AROONDialog *) dialog;
  d->settings(_inputObject, _highKey, _lowKey, _period);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
AROONObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _outputUpKey << _outputDownKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
AROONObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setValue(_outputUpKey, _ubars);
  oc->setValue(_outputDownKey, _dbars);
  return 1;
}

int
AROONObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "AROONObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _highKey = settings->value(QString("high_key")).toString();
  _lowKey = settings->value(QString("low_key")).toString();
  _period = settings->value(QString("period")).toInt();

  return 1;
}

int
AROONObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "AROONObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("high_key"), _highKey);
  settings->setValue(QString("low_key"), _lowKey);
  settings->setValue(QString("period"), _period);

  return 1;
}
