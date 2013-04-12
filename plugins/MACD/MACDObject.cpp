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

#include "MACDObject.h"
#include "ta_libc.h"
#include "MACDDialog.h"
#include "Util.h"

#include <QtDebug>


MACDObject::MACDObject (QString profile, QString name)
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "MACDObject::MACDObject: error on TA_Initialize";
  
  _profile = profile;
  _name = name;
  _plugin = QString("MACD");
  _type = QString("indicator");
  _fastMAType = QString("EMA");
  _slowMAType = QString("EMA");
  _signalMAType = QString("EMA");
  _fastPeriod = 12;
  _slowPeriod = 26;
  _signalPeriod = 9;
  _macdKey = QString("m");
  _signalKey = QString("s");
  _histKey = QString("h");
  _hasOutput = TRUE;
  _inputObject = QString("symbol");
  _inputKey = QString("C");
  
  _mbars = new Bars;
  _sbars = new Bars;
  _hbars = new Bars;
  
  _commandList << QString("update");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");

  Util util;
  Object *o = util.object(QString("MA"), QString(), QString());
  if (o)
  {
    QString key("types");
    ObjectCommand toc(key);
    if (o->message(&toc))
      _maList = toc.getList(key);
    delete o;
  }
}

MACDObject::~MACDObject ()
{
  delete _mbars;
  delete _sbars;
  delete _hbars;
}

void
MACDObject::clear ()
{
  _mbars->clear();
  _sbars->clear();
  _hbars->clear();
}

int
MACDObject::message (ObjectCommand *oc)
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
MACDObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *io = (Object *) oc->getObject(_inputObject);
  if (! io)
  {
    qDebug() << "MACDObject::update: invalid input" << _inputObject;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! io->message(&toc))
  {
    qDebug() << "MACDObject::update: message error" << io->plugin() << toc.command();
    return 0;
  }

  Bars *ibars = toc.getBars(_inputKey);
  if (! ibars)
  {
    qDebug() << "MACDObject::update: invalid input bars" << _inputKey;
    return 0;
  }
  
  int size = ibars->_bars.size();

  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];
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
  
  TA_RetCode rc = TA_MACDEXT(0,
			     dpos - 1,
			     &input[0],
			     _fastPeriod,
			     (TA_MAType) _maList.indexOf(_fastMAType),
			     _slowPeriod,
			     (TA_MAType) _maList.indexOf(_slowMAType),
			     _signalPeriod,
			     (TA_MAType) _maList.indexOf(_signalMAType),
			     &outBeg,
			     &outNb,
    			     &out[0],
                             &out2[0],
                             &out3[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MACDObject::update: TA-Lib error" << rc;
    return 0;
  }
  
  int outLoop = outNb - 1;
  it.toBack();
  while (it.hasPrevious() && outLoop > -1)
  {
    it.previous();
    _mbars->setValue(it.key(), (double) out[outLoop]);
    _sbars->setValue(it.key(), (double) out2[outLoop]);
    _hbars->setValue(it.key(), (double) out3[outLoop--]);
  }

  return 1;
}

int
MACDObject::dialog (ObjectCommand *oc)
{
  MACDDialog *dialog = new MACDDialog(oc->getObjects(), _name);
  dialog->setSettings(_inputObject, _inputKey, _fastPeriod, _fastMAType, _slowPeriod, _slowMAType, _signalPeriod, _signalMAType);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
MACDObject::dialogDone (void *dialog)
{
  MACDDialog *d = (MACDDialog *) dialog;
  d->settings(_inputObject, _inputKey, _fastPeriod, _fastMAType, _slowPeriod, _slowMAType, _signalPeriod, _signalMAType);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
MACDObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _macdKey << _signalKey << _histKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
MACDObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setValue(_macdKey, _mbars);
  oc->setValue(_signalKey, _sbars);
  oc->setValue(_histKey, _hbars);
  return 1;
}

int
MACDObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MACDObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _inputKey = settings->value(QString("input_key")).toString();
  _fastMAType = settings->value(QString("fast_ma_type")).toString();
  _fastPeriod = settings->value(QString("fast_period")).toInt();
  _slowMAType = settings->value(QString("slow_ma_type")).toString();
  _slowPeriod = settings->value(QString("slow_period")).toInt();
  _signalMAType = settings->value(QString("signal_ma_type")).toString();
  _signalPeriod = settings->value(QString("signal_period")).toInt();

  return 1;
}

int
MACDObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MACDObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("input_key"), _inputKey);
  settings->setValue(QString("fast_ma_type"), _fastMAType);
  settings->setValue(QString("fast_period"), _fastPeriod);
  settings->setValue(QString("slow_ma_type"), _slowMAType);
  settings->setValue(QString("slow_period"), _slowPeriod);
  settings->setValue(QString("signal_ma_type"), _signalMAType);
  settings->setValue(QString("signal_period"), _signalPeriod);

  return 1;
}
