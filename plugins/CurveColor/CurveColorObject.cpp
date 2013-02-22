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

#include "CurveColorObject.h"
#include "Util.h"

#include <QtDebug>


CurveColorObject::CurveColorObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("CurveColor");
  _type = QString("indicator");
  _op = 2;
  _inputObject = QString("symbol");
  _inputKey = QString("C");
  _value = 1.0;
  _color = QColor(Qt::red);
  
  _commandList << QString("update");
  _commandList << QString("ops");
  _commandList << QString("dialog");
  _commandList << QString("load");
  _commandList << QString("save");
  
  _opList << QString("<");
  _opList << QString("<=");
  _opList << QString("==");
  _opList << QString("!=");
  _opList << QString(">=");
  _opList << QString(">");
}

int
CurveColorObject::message (ObjectCommand *oc)
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
      rc = load(oc);
      break;
    case 4:
      rc = save(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
CurveColorObject::update (ObjectCommand *oc)
{
  // input object
  Object *input = (Object *) oc->getObject(_inputObject);
  if (! input)
  {
    qDebug() << "CurveColorObject::update: invalid" << _inputObject;
    return 0;
  }
  
  // input2 object
  Object *input2 = (Object *) oc->getObject(_input2Object);
  if (! input2)
  {
    qDebug() << "CurveColorObject::update: invalid" << _input2Object;
    return 0;
  }
 
  if (input2->type() != QString("curve"))
  {
    qDebug() << "CurveColorObject::update: invalid" << _input2Object;
    return 0;
  }
  
  // plot object
  Object *plot = (Object *) oc->getObject(_plotObject);
  if (! plot)
  {
    qDebug() << "CurveColorObject::update: invalid" << _plotObject;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! input->message(&toc))
  {
    qDebug() << "CurveColorObject::update: message error" << input->plugin() << toc.command();
    return 0;
  }
  
  toc.setCommand(QString("set_color"));
  toc.setValue(QString("color"), _color);
  
  QMapIterator<int, Data *> it(toc.map());
  while (it.hasNext())
  {
    it.next();
    Data *d = it.value();
    
    if (! d->contains(_inputKey))
      continue;
    double v = d->value(_inputKey).toDouble();
    
    bool flag = FALSE;
    switch (_op)
    {
      case 0:
	if (v < _value)
	  flag = TRUE;
	break;
      case 1:
	if (v <= _value)
	  flag = TRUE;
	break;
      case 2:
	if (v == _value)
	  flag = TRUE;
	break;
      case 3:
	if (v != _value)
	  flag = TRUE;
	break;
      case 4:
	if (v >= _value)
	  flag = TRUE;
	break;
      case 5:
	if (v > _value)
	  flag = TRUE;
	break;
      default:
	break;
    }
    
    if (! flag)
      continue;
    
    toc.setValue(QString("index"), it.key());
    input2->message(&toc);
  }

  // get label from settings
  ObjectCommand toc2(QString("settings"));
  if (! input2->message(&toc2))
  {
    qDebug() << "CurveLineObject::update: message error" << input2->plugin() << toc2.command();
    return 0;
  }
  
  QString label = toc2.getString(QString("label"));

  // add to plot
  toc2.setCommand(QString("add_objects"));
  toc2.setValue(label, (void *) input2);
  if (! plot->message(&toc2))
  {
    qDebug() << "CurveLineObject::update: message error" << plot->plugin() << toc2.command();
    return 0;
  }
  
  return 1;
}

int
CurveColorObject::dialog (ObjectCommand *oc)
{
  CurveColorDialog *dialog = new CurveColorDialog(oc->getObjects(), _opList, _name);
  dialog->setSettings(_color, _inputObject, _inputKey, _input2Object, _op, _value, _plotObject);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
CurveColorObject::dialogDone (void *dialog)
{
  CurveColorDialog *d = (CurveColorDialog *) dialog;
  d->settings(_color, _inputObject, _inputKey, _input2Object, _op, _value, _plotObject);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
CurveColorObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "CurveColorObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object"), QString("symbol")).toString();
  _input2Object = settings->value(QString("input2_object")).toString();
  _inputKey = settings->value(QString("input_key"), QString("C")).toString();
  _op = settings->value(QString("op"), 2).toInt();
  _color = settings->value(QString("color"), QColor(Qt::red)).value<QColor>();
  _value = settings->value(QString("value"), 1.0).toDouble();
  _plotObject = settings->value(QString("plot_object")).toString();

  return 1;
}

int
CurveColorObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "CurveColorObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("input2_object"), _input2Object);
  settings->setValue(QString("input_key"), _inputKey);
  settings->setValue(QString("op"), _op);
  settings->setValue(QString("color"), _color);
  settings->setValue(QString("value"), _value);
  settings->setValue(QString("plot_object"), _plotObject);

  return 1;
}
