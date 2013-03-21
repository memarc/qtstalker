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

#include "MAObject.h"
#include "ta_libc.h"
#include "MADialog.h"
#include "Util.h"
#include <glib.h>

#include <QtDebug>


MAObject::MAObject (QString profile, QString name)
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "MAObject::MAObject: error on TA_Initialize";
  
  _profile = profile;
  _name = name;
  _plugin = QString("MA");
  _type = QString("indicator");
  _maType = QString("EMA");
  _period = 14;
  _outputKey = QString("MA");
  _hasOutput = TRUE;
  _inputObject = QString("symbol");
  _inputKey = QString("C");
  
  _commandList << QString("update");
  _commandList << QString("types");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");
  
  _maList << QString("SMA");
  _maList << QString("EMA");
  _maList << QString("WMA");
  _maList << QString("DEMA");
  _maList << QString("TEMA");
  _maList << QString("TRIMA");
  _maList << QString("KAMA");
  _maList << QString("Wilder");
}

MAObject::~MAObject ()
{
  clear();
}

void
MAObject::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

int
MAObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = update(oc);
      break;
    case 1:
      oc->setValue(QString("types"), _maList);
      rc = 1;
      break;
    case 2:
      rc = dialog(oc);
      break;
    case 3:
      rc = output(oc);
      break;
    case 4:
      rc = load(oc);
      break;
    case 5:
      rc = save(oc);
      break;
    case 6:
      rc = outputKeys(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
MAObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *io = (Object *) oc->getObject(_inputObject);
  if (! io)
  {
    qDebug() << "MAObject::update: invalid input" << _inputObject;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! io->message(&toc))
  {
    qDebug() << "MAObject::update: message error" << io->plugin() << toc.command();
    return 0;
  }

  QMap<int, Data *> data = toc.map();


  GTimer *timer = g_timer_new();
  
  int size = data.size();
  TA_Real input[size];
  TA_Real out[size];
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
  
  TA_RetCode rc = TA_MA(0,
			dpos - 1,
			&input[0],
			_period,
			(TA_MAType) _maList.indexOf(_maType),
			&outBeg,
			&outNb,
			&out[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MAObject::getMA: TA-Lib error" << rc << size << _period << _maType;
    return 0;
  }

  int outLoop = outNb - 1;
  it.toBack();
  while (it.hasPrevious() && outLoop > -1)
  {
    it.previous();
    Data *b = new Data;
    b->insert(_outputKey, out[outLoop--]);
    _bars.insert(it.key(), b);
  }

  g_timer_stop(timer);
  gulong ms = 0;
  gdouble seconds = g_timer_elapsed(timer, &ms);
  g_print("MAObject::update: time elapsed %g\n", seconds);
  g_timer_destroy(timer);
  
  return 1;
}

int
MAObject::dialog (ObjectCommand *oc)
{
  MADialog *dialog = new MADialog(oc->getObjects(), _name);
  dialog->setSettings(_inputObject, _inputKey, _maType, _period);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
MAObject::dialogDone (void *dialog)
{
  MADialog *d = (MADialog *) dialog;
  d->settings(_inputObject, _inputKey, _maType, _period);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
MAObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _outputKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
MAObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setMap(_bars);
  return 1;
}

int
MAObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MAObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _inputKey = settings->value(QString("input_key")).toString();
  _maType = settings->value(QString("type")).toString();
  _period = settings->value(QString("period")).toInt();

  return 1;
}

int
MAObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MAObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("input_key"), _inputKey);
  settings->setValue(QString("type"), _maType);
  settings->setValue(QString("period"), _period);

  return 1;
}
