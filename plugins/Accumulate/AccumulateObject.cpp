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

#include "AccumulateObject.h"
#include "Util.h"

#include <QtDebug>


AccumulateObject::AccumulateObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("Accumulate");
  _type = QString("indicator");
  _outputKey = QString("v");
  _inputObject = QString("symbol");
  _inputKey = QString("C");
  _hasOutput = TRUE;
  
  _bars = new Bars;
  
  _commandList << QString("update");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");
}

AccumulateObject::~AccumulateObject ()
{
  delete _bars;
}

void
AccumulateObject::clear ()
{
  _bars->clear();
}

int
AccumulateObject::message (ObjectCommand *oc)
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
AccumulateObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *input = (Object *) oc->getObject(_inputObject);
  if (! input)
  {
    qDebug() << "AccumulateObject::update: invalid" << _inputObject;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! input->message(&toc))
  {
    qDebug() << "AccumulateObject::update: message error" << input->plugin() << toc.command();
    return 0;
  }

  Bars *ibars = toc.getBars(_inputKey);
  if (! ibars)
  {
    qDebug() << "AccumulateObject::update: invalid input bars" << _inputKey;
    return 0;
  }
  
  double accum = 0;
  QMapIterator<int, Bar *> it(ibars->_bars);
  while (it.hasNext())
  {
    it.next();
    Bar *d = it.value();
    accum += d->v;
    _bars->setValue(it.key(), accum);
  }
  
  return 1;
}

int
AccumulateObject::dialog (ObjectCommand *oc)
{
  AccumulateDialog *dialog = new AccumulateDialog(oc->getObjects(), _name);
  dialog->setSettings(_inputObject, _inputKey);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
AccumulateObject::dialogDone (void *dialog)
{
  AccumulateDialog *d = (AccumulateDialog *) dialog;
  d->settings(_inputObject, _inputKey);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
AccumulateObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _outputKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
AccumulateObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setValue(_outputKey, _bars);
  return 1;
}

int
AccumulateObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "AccumulateObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _inputKey = settings->value(QString("input_key")).toString();

  return 1;
}

int
AccumulateObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "AccumulateObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("input_key"), _inputKey);

  return 1;
}
