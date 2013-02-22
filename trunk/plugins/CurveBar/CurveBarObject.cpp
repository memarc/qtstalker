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

#include "CurveBarObject.h"
#include "Util.h"

#include <QtDebug>

CurveBarObject::CurveBarObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("CurveBar");
  _type = QString("curve");
  _penWidth = 1;
  _color = QColor(Qt::red);
  _label = tr("Bar");
   _inputObject = QString("symbol");
  _inputKey = QString("H");
 
  _commandList << QString("draw");
  _commandList << QString("info");
  _commandList << QString("high_low_range");
  _commandList << QString("update");
  _commandList << QString("scale_point");
  _commandList << QString("start_end_index");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("dialog");
  _commandList << QString("copy");
  _commandList << QString("output");
  _commandList << QString("set_color");
  _commandList << QString("settings");
}

CurveBarObject::~CurveBarObject ()
{
  clear();
}

int
CurveBarObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = draw(oc);
      break;
    case 1:
      rc = info(oc);
      break;
    case 2:
      rc = highLowRange(oc);
      break;
    case 3:
      rc = update(oc);
      break;
    case 4:
      rc = scalePoint(oc);
      break;
    case 5:
      rc = startEndIndexMessage(oc);
      break;
    case 6:
      rc = load(oc);
      break;
    case 7:
      rc = save(oc);
      break;
    case 8:
      rc = dialog(oc);
      break;
    case 9:
      rc = copy(oc);
      break;
    case 10:
      rc = output(oc);
      break;
    case 11:
      rc = setColor(oc);
      break;
    case 12:
      rc = settings(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

void
CurveBarObject::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

int
CurveBarObject::draw (ObjectCommand *oc)
{
  // get painter
  QString key("painter");
  QPainter *p = (QPainter *) oc->getObject(key);
  if (! p)
  {
    qDebug() << "CurveBarObject::draw: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "CurveBarObject::draw: invalid" << key;
    return 0;
  }
  
  int pos = oc->getInt(QString("index"));
  int end = oc->getInt(QString("index_end"));
  int width = oc->getInt(QString("spacing"));
  
  // get end key
  int start = 0;
  int tend = 0;
  if (! startEndIndex(start, tend))
    return 0;
  if (tend < end)
    end = tend;

//  p->setRenderHint(QPainter::Antialiasing, TRUE);

  QRect rect = p->viewport();
  
  QPen pen = p->pen();
  pen.setStyle(Qt::NoPen);
  p->setPen(pen);

  // get zero value
  ObjectCommand toc(QString("convert_to_y"));
  toc.setValue(QString("value"), 0);
  if (! plot->message(&toc))
  {
    qDebug() << "CurveBarObject::drawBar: base error";
    return 0;
  }
  int zero = toc.getInt(QString("y"));
  
  QList<QPoint> bpoints;
  QPolygon hist;      
  int x = 0;
  while (x < rect.width() && pos <= end)
  {
    Data *bar = _bars.value(pos);
    if (bar)
    {
      toc.setValue(QString("value"), bar->value("v").toDouble());
      if (! plot->message(&toc))
      {
	qDebug() << "CurveBarObject::drawBar: value error";
	continue;
      }
      int v = toc.getInt(QString("y"));
      
      if (v < zero)
      {
        QRect r(x, v, width - 1, 0);
        r.setHeight(zero - v);
        p->fillRect(r, bar->value("c").value<QColor>());
      }
      else
      {
        QRect r(x, zero, width - 1, 0);
        r.setHeight(v - zero);
        p->fillRect(r, bar->value("c").value<QColor>());
      }
    }
    
    x += width;
    pos++;
  }
  
  return 1;
}

int
CurveBarObject::update (ObjectCommand *oc)
{
  clear();

  Object *input = (Object *) oc->getObject(_inputObject);
  if (! input)
  {
    qDebug() << "CurveBarObject::update: invalid input" << _inputObject;
    return 0;
  }

  Object *plot = (Object *) oc->getObject(_plotObject);
  if (! plot)
  {
    qDebug() << "CurveBarObject::update: invalid plot" << _plotObject;
    return 0;
  }
  
  ObjectCommand toc(QString("output"));
  if (! input->message(&toc))
  {
    qDebug() << "CurveLineObject::update: message error" << input->plugin() << toc.command();
    return 0;
  }
  
  QMapIterator<int, Data *> it(toc.map());
  while (it.hasNext())
  {
    it.next();
    Data *bar = it.value();

    if (! bar->contains(_inputKey))
      continue;
    
    Data *nbar = new Data;
    nbar->insert("c", _color);
    nbar->insert("v", bar->value(_inputKey));
    _bars.insert(it.key(), nbar);
  }

  // add to plot
  ObjectCommand toc2(QString("add_objects"));
  toc2.setValue(_label, (void *) this);
  if (! plot->message(&toc2))
  {
    qDebug() << "CurveBarObject::update: message error" << plot->plugin() << toc2.command();
    return 0;
  }
  
  return 1;
}

int
CurveBarObject::info (ObjectCommand *oc)
{
  // get index
  int index = oc->getInt(QString("index"));

  Data *bar = _bars.value(index);
  if (! bar)
    return 0;

  Util strip;
  QString ts;
  strip.strip(bar->value("v").toDouble(), 4, ts);
  
  Data info;
  info.insert(_label, ts);
  oc->setValue(QString("info"), info);

  return 1;
}

int
CurveBarObject::highLowRange (ObjectCommand *oc)
{
  if (! _bars.size())
    return 0;
  
  // get plot
  QString key("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "CurveBarObject::highLowRange: invalid" << key;
    return 0;
  }

  ObjectCommand toc(QString("set_high_low"));
  
  QMapIterator<int, Data *> it(_bars);
  while (it.hasNext())
  {
    it.next();
    Data *b = it.value();
    
    double h = b->value("v").toDouble();
    double l = 0;
    
    if (h < l)
    {
      l = h;
      h = 0;
    }
    
    toc.setValue(QString("index"), it.key());
    toc.setValue(QString("high"), h);
    toc.setValue(QString("low"), l);
    plot->message(&toc);
  }
  
  return 1;
}

int
CurveBarObject::scalePoint (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  
  Data *b = _bars.value(index);
  if (! b)
    return 0;

  oc->setValue(QString("color"), b->value("c").value<QColor>());
  oc->setValue(QString("value"), b->value("v").toDouble());
  
  return 1;
}

int
CurveBarObject::startEndIndexMessage (ObjectCommand *oc)
{
  int start = 0;
  int end = 0;
  if (! startEndIndex(start, end))
  {
    qDebug() << "CurveBarObject::startEndIndexMessage: error";
    return 0;
  }
  
  oc->setValue(QString("start"), start);
  oc->setValue(QString("end"), end);
  
  return 1;
}

int
CurveBarObject::startEndIndex (int &start, int &end)
{
  start = 0;
  end = 0;
  
  QMapIterator<int, Data *> it(_bars);
  it.toFront();
  if (! it.hasNext())
    return 0;
  it.next();
  start = it.key();
  
  it.toBack();
  if (! it.hasPrevious())
    return 0;
  it.previous();
  end = it.key();
  
  return 1;
}

int
CurveBarObject::load (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "CurveBarObject::load: invalid" << key;
    return 0;
  }

  _inputObject = settings->value(QString("input_object"), QString("symbol")).toString();
  _inputKey = settings->value(QString("input_key"), QString("H")).toString();
  _label = settings->value(QString("label"), QString("Line")).toString();
  _color = settings->value(QString("color"), QColor(Qt::red)).value<QColor>();
  _plotObject = settings->value(QString("plot_object")).toString();
  
  return 1;
}

int
CurveBarObject::save (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "CurveBarObject::save: invalid" << key;
    return 0;
  }

  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("input_key"), _inputKey);
  settings->setValue(QString("label"), _label);
  settings->setValue(QString("color"), _color);
  settings->setValue(QString("plot_object"), _plotObject);

  return 1;
}

int
CurveBarObject::dialog (ObjectCommand *oc)
{
  CurveBarDialog *dialog = new CurveBarDialog(oc->getObjects(), _name);
  dialog->setSettings(_color, _label, _inputObject, _inputKey, _plotObject);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
CurveBarObject::dialogDone (void *dialog)
{
  CurveBarDialog *d = (CurveBarDialog *) dialog;
  d->settings(_color, _label, _inputObject, _inputKey, _plotObject);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
CurveBarObject::copy (ObjectCommand *oc)
{
  QString key("input");
  Object *input = (Object *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "CurveBarObject::copy: invalid" << key;
    return 0;
  }
  
  if (input->plugin() != _plugin)
  {
    qDebug() << "CurveBarObject::copy: invalid" << key;
    return 0;
  }

  key = QString("output");
  ObjectCommand toc(key);
  if (! input->message(&toc))
  {
    qDebug() << "CurveBarObject::copy: invalid" << input->plugin() << key;
    return 0;
  }
  
  qDeleteAll(_bars);
  _bars.clear();

  _inputObject = toc.getString(QString("input_object"));
  _inputKey = toc.getString(QString("input_key"));
  _label = toc.getString(QString("label"));
  _color = toc.getColor(QString("color"));
  _plotObject = toc.getString(QString("plot_object"));
  
  QMapIterator<int, Data *> it(toc.map());
  while (it.hasNext())
  {
    it.next();
    Data *b = it.value();
    
    Data *nb = new Data;
    nb->insert("v", b->value("v"));
    nb->insert("c", b->value("c"));
    _bars.insert(it.key(), nb);
  }
  
  return 1;
}

int
CurveBarObject::output (ObjectCommand *oc)
{
  oc->setMap(_bars);
  settings(oc);  
  return 1;
}

int
CurveBarObject::setColor (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  QColor color = oc->getColor(QString("color"));
  
  Data *bar = _bars.value(index);
  if (! bar)
    return 0;
  
  bar->insert("c", color);
  
  return 1;
}

int
CurveBarObject::settings (ObjectCommand *oc)
{
  oc->setValue(QString("input_object"), _inputObject);
  oc->setValue(QString("input_key"), _inputKey);
  oc->setValue(QString("label"), _label);
  oc->setValue(QString("color"), _color);
  oc->setValue(QString("plot_object"), _plotObject);

  QStringList tl;
  tl << "v";
  oc->setValue(QString("output_keys"), tl);

  tl.clear();
  tl << _label;
  oc->setValue(QString("output_labels"), tl);

  return 1;
}

