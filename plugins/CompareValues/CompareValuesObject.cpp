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

#include "CompareValuesObject.h"
#include "Util.h"

#include <QtDebug>


CompareValuesObject::CompareValuesObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("CompareValues");
  _type = QString("indicator");
  _op = 2;
  _outputKey = QString("v");
  _inputObject = QString("symbol");
  _input2Object = QString("symbol");
  _inputKey = QString("C");
  _input2Key = QString("C");
  _offset = 0;
  _offset2 = 0;
  _constant = FALSE;
  _constantValue = 0;
  _hasOutput = TRUE;
  
  _bars = new Bars;
  
  _commandList << QString("update");
  _commandList << QString("ops");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");
  _commandList << QString("value");
  _commandList << QString("start_end_index");
  
  _opList << QString("<");
  _opList << QString("<=");
  _opList << QString("==");
  _opList << QString("!=");
  _opList << QString(">=");
  _opList << QString(">");
}

CompareValuesObject::~CompareValuesObject ()
{
  delete _bars;
}

void
CompareValuesObject::clear ()
{
  _bars->clear();
}

int
CompareValuesObject::message (ObjectCommand *oc)
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
    case 7:
      rc = value(oc);
      break;
    case 8:
      rc = startEndIndex(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
CompareValuesObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *input = (Object *) oc->getObject(_inputObject);
  if (! input)
  {
    qDebug() << "CompareValuesObject::update: invalid" << _inputObject;
    return 0;
  }
  
  // input2 object
  Object *input2 = 0;
  if (! _constant)
  {
    input2 = (Object *) oc->getObject(_input2Object);
    if (! input2)
    {
      qDebug() << "CompareValuesObject::update: invalid" << _input2Object;
      return 0;
    }
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! input->message(&toc))
  {
    qDebug() << "CompareValuesObject::update: message error" << input->plugin() << toc.command();
    return 0;
  }

  Bars *ibars = toc.getBars(_inputKey);
  if (! ibars)
  {
    qDebug() << "CompareValuesObject::update: invalid input bars" << _inputKey;
    return 0;
  }
  
  // get input2 bars
  Bars *ibars2 = 0;
  if (! _constant)
  {
    ibars2 = toc.getBars(_input2Key);
    if (! ibars2)
    {
      qDebug() << "CompareValuesObject::update: invalid input2 bars" << _input2Key;
      return 0;
    }
  }
  
  QList<int> keys = ibars->_bars.keys();
  
  for (int pos = 0; pos < keys.size(); pos++)
  {
    _bars->setValue(keys.at(pos), 0);
    
    Bar *d = ibars->_bars.value(keys.at(pos - _offset));
    if (! d)
      continue;
    double v = d->v;
    
    double v2 = _constantValue;
    if (! _constant)
    {
      d = ibars2->_bars.value(keys.at(pos - _offset2));
      if (! d)
        continue;
      v2 = d->v;
    }
    
    switch (_op)
    {
      case 0:
	if (v < v2)
          _bars->setValue(keys.at(pos), 1);
	break;
      case 1:
	if (v <= v2)
          _bars->setValue(keys.at(pos), 1);
	break;
      case 2:
	if (v == v2)
          _bars->setValue(keys.at(pos), 1);
	break;
      case 3:
	if (v != v2)
          _bars->setValue(keys.at(pos), 1);
	break;
      case 4:
	if (v >= v2)
          _bars->setValue(keys.at(pos), 1);
	break;
      case 5:
	if (v > v2)
          _bars->setValue(keys.at(pos), 1);
	break;
      default:
	break;
    }
  }
  
  return 1;
}

int
CompareValuesObject::dialog (ObjectCommand *oc)
{
  CompareValuesDialog *dialog = new CompareValuesDialog(oc->getObjects(), _opList, _name);
  dialog->setSettings(_inputObject, _inputKey, _offset, _input2Object,
                      _input2Key, _offset2, _op, _constant, _constantValue);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
CompareValuesObject::dialogDone (void *dialog)
{
  CompareValuesDialog *d = (CompareValuesDialog *) dialog;
  d->settings(_inputObject, _inputKey, _offset, _input2Object, _input2Key, _offset2, _op, _constant, _constantValue);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
CompareValuesObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _outputKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
CompareValuesObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setValue(_outputKey, _bars);
  return 1;
}

int
CompareValuesObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "CompareValuesObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _input2Object = settings->value(QString("input2_object")).toString();
  _inputKey = settings->value(QString("input_key")).toString();
  _input2Key = settings->value(QString("input2_key")).toString();
  _op = settings->value(QString("op")).toInt();
  _offset = settings->value(QString("offset")).toInt();
  _offset2 = settings->value(QString("offset2")).toInt();
  _constant = settings->value(QString("constant"), FALSE).toBool();
  _constantValue = settings->value(QString("constant_value"), 0).toDouble();

  return 1;
}

int
CompareValuesObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "CompareValuesObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("input2_object"), _input2Object);
  settings->setValue(QString("input_key"), _inputKey);
  settings->setValue(QString("input2_key"), _input2Key);
  settings->setValue(QString("op"), _op);
  settings->setValue(QString("offset"), _offset);
  settings->setValue(QString("offset2"), _offset2);
  settings->setValue(QString("constant"), _constant);
  settings->setValue(QString("constant_value"), _constantValue);

  return 1;
}

int
CompareValuesObject::value (ObjectCommand *oc)
{
  QString key("index");
  int index = oc->getInt(key);
  
  Bar *d = _bars->value(index);
  if (! d)
  {
    qDebug() << "CompareValuesObject::save: invalid" << key << index;
    return 0;
  }
  
  oc->setValue(QString("value"), d->v);
  
  return 1;
}

int
CompareValuesObject::startEndIndex (ObjectCommand *oc)
{
  int start = 0;
  int end = 0;
  _bars->startEndIndex(start, end);
  oc->setValue(QString("start"), start);
  oc->setValue(QString("end"), end);
  return 1;
}
