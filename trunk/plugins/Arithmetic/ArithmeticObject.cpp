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

#include "ArithmeticObject.h"
#include "Util.h"

#include <QtDebug>


ArithmeticObject::ArithmeticObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("Arithmetic");
  _type = QString("indicator");
  _op = 0;
  _outputKey = QString("v");
  _inputObject = QString("symbol");
  _input2Object = QString("symbol");
  _inputKey = QString("C");
  _input2Key = QString("C");
  _hasOutput = TRUE;
  
  _commandList << QString("update");
  _commandList << QString("ops");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");
  
  _opList << QString("ADD");
  _opList << QString("DIV");
  _opList << QString("MULT");
  _opList << QString("SUB");
}

ArithmeticObject::~ArithmeticObject ()
{
  clear();
}

void
ArithmeticObject::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

int
ArithmeticObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = update(oc);
      break;
    case 1:
      oc->setValue(QString("ops"), _opList);
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
ArithmeticObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *input = (Object *) oc->getObject(_inputObject);
  if (! input)
  {
    qDebug() << "ArithmeticObject::update: invalid" << _inputObject;
    return 0;
  }
  
  // input2 object
  Object *input2 = (Object *) oc->getObject(_input2Object);
  if (! input2)
  {
    qDebug() << "ArithmeticObject::update: invalid" << _input2Object;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! input->message(&toc))
  {
    qDebug() << "ArithmeticObject::update: message error" << input->plugin() << toc.command();
    return 0;
  }
  QMap<int, Data *> in = toc.map();
  
  // get input2 bars
  if (! input2->message(&toc))
  {
    qDebug() << "ArithmeticObject::update: message error" << input2->plugin() << toc.command();
    return 0;
  }
  QMap<int, Data *> in2 = toc.map();

  switch (_op)
  {
    case 0:
      return add(in, in2);
      break;
    case 1:
      return div(in, in2);
      break;
    case 2:
      return mult(in, in2);
      break;
    case 3:
      return sub(in, in2);
      break;
    default:
      break;
  }

  return 0;
}

int
ArithmeticObject::add (QMap<int, Data *> &in, QMap<int, Data *> &in2)
{
  QList<int> keys = in.keys();

  for (int pos = 0; pos < keys.size(); pos++)
  {
    Data *d = in.value(keys.at(pos));
    
    if (! d->contains(_inputKey))
      continue;
    double v = d->value(_inputKey).toDouble();
    
    d = in2.value(keys.at(pos));
    if (! d)
      continue;
    
    if (! d->contains(_input2Key))
      continue;
    double v2 = d->value(_input2Key).toDouble();

    d = new Data;
    d->insert(_outputKey, QVariant(v + v2));
    _bars.insert(keys.at(pos), d);
  }
  
  return 1;
}

int
ArithmeticObject::div (QMap<int, Data *> &in, QMap<int, Data *> &in2)
{
  QList<int> keys = in.keys();

  for (int pos = 0; pos < keys.size(); pos++)
  {
    Data *d = in.value(keys.at(pos));
    
    if (! d->contains(_inputKey))
      continue;
    double v = d->value(_inputKey).toDouble();
    
    d = in2.value(keys.at(pos));
    if (! d)
      continue;
    
    if (! d->contains(_input2Key))
      continue;
    double v2 = d->value(_input2Key).toDouble();

    d = new Data;
    d->insert(_outputKey, QVariant(v / v2));
    _bars.insert(keys.at(pos), d);
  }
  
  return 1;
}

int
ArithmeticObject::mult (QMap<int, Data *> &in, QMap<int, Data *> &in2)
{
  QList<int> keys = in.keys();

  for (int pos = 0; pos < keys.size(); pos++)
  {
    Data *d = in.value(keys.at(pos));
    
    if (! d->contains(_inputKey))
      continue;
    double v = d->value(_inputKey).toDouble();
    
    d = in2.value(keys.at(pos));
    if (! d)
      continue;
    
    if (! d->contains(_input2Key))
      continue;
    double v2 = d->value(_input2Key).toDouble();

    d = new Data;
    d->insert(_outputKey, QVariant(v * v2));
    _bars.insert(keys.at(pos), d);
  }
  
  return 1;
}

int
ArithmeticObject::sub (QMap<int, Data *> &in, QMap<int, Data *> &in2)
{
  QList<int> keys = in.keys();

  for (int pos = 0; pos < keys.size(); pos++)
  {
    Data *d = in.value(keys.at(pos));
    
    if (! d->contains(_inputKey))
      continue;
    double v = d->value(_inputKey).toDouble();
    
    d = in2.value(keys.at(pos));
    if (! d)
      continue;
    
    if (! d->contains(_input2Key))
      continue;
    double v2 = d->value(_input2Key).toDouble();

    d = new Data;
    d->insert(_outputKey, QVariant(v - v2));
    _bars.insert(keys.at(pos), d);
  }
  
  return 1;
}

int
ArithmeticObject::dialog (ObjectCommand *oc)
{
  ArithmeticDialog *dialog = new ArithmeticDialog(oc->getObjects(), _opList, _name);
  dialog->setSettings(_inputObject, _inputKey, _input2Object, _input2Key, _op);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
ArithmeticObject::dialogDone (void *dialog)
{
  ArithmeticDialog *d = (ArithmeticDialog *) dialog;
  d->settings(_inputObject, _inputKey, _input2Object, _input2Key, _op);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
ArithmeticObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _outputKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
ArithmeticObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setMap(_bars);
  return 1;
}

int
ArithmeticObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "ArithmeticObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _input2Object = settings->value(QString("input2_object")).toString();
  _inputKey = settings->value(QString("input_key")).toString();
  _input2Key = settings->value(QString("input2_key")).toString();
  _op = settings->value(QString("op")).toInt();

  return 1;
}

int
ArithmeticObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "ArithmeticObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("input2_object"), _input2Object);
  settings->setValue(QString("input_key"), _inputKey);
  settings->setValue(QString("input2_key"), _input2Key);
  settings->setValue(QString("op"), _op);

  return 1;
}
