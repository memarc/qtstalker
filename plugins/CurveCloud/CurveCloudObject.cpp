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

#include "CurveCloudObject.h"
#include "Util.h"

#include <QtDebug>

CurveCloudObject::CurveCloudObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("CurveCloud");
  _type = QString("curve");
  _penWidth = 1;
  _color = QColor(Qt::red);
  _label = tr("Cloud");
   _baseObject = QString("symbol");
  _baseKey = QString("L");
   _valueObject = QString("symbol");
  _valueKey = QString("H");
 
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

CurveCloudObject::~CurveCloudObject ()
{
  clear();
}

int
CurveCloudObject::message (ObjectCommand *oc)
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
CurveCloudObject::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

int
CurveCloudObject::draw (ObjectCommand *oc)
{
  // get painter
  QString key("painter");
  QPainter *p = (QPainter *) oc->getObject(key);
  if (! p)
  {
    qDebug() << "CurveCloudObject::draw: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "CurveCloudObject::draw: invalid" << key;
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
  
  ObjectCommand toc(QString("convert_to_y"));
  QList<QPoint> bpoints;
  QPolygon hist;      
  int x = 0;
  while (x < rect.width() && pos <= end)
  {
    Data *bar = _bars.value(pos);
    if (bar)
    {
      toc.setValue(QString("value"), bar->value("b").toDouble());
      if (! plot->message(&toc))
      {
	qDebug() << "CurveCloudObject::drawCloud: base error";
	continue;
      }
      int b = toc.getInt(QString("y"));
      
      toc.setValue(QString("value"), bar->value("v").toDouble());
      if (! plot->message(&toc))
      {
	qDebug() << "CurveCloudObject::drawCloud: value error";
	continue;
      }
      int v = toc.getInt(QString("y"));
      
      hist << QPoint(x, v);
      
      bpoints << QPoint(x, b);
    }
    
    x += width;
    pos++;
  }
  
  for (int pos = bpoints.size() - 1; pos > -1; pos--)
    hist << bpoints.at(pos);
  
  p->setBrush(_color);
  
  p->drawPolygon(hist, Qt::OddEvenFill);
  
  return 1;
}

int
CurveCloudObject::update (ObjectCommand *oc)
{
  clear();

  Object *input = (Object *) oc->getObject(_valueObject);
  if (! input)
  {
    qDebug() << "CurveCloudObject::update: invalid" << _valueObject;
    return 0;
  }

  Object *plot = (Object *) oc->getObject(_plotObject);
  if (! plot)
  {
    qDebug() << "CurveCloudObject::update: invalid plot" << _plotObject;
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

    if (! bar->contains(_valueKey))
      continue;
    
    Data *nbar = new Data;
    nbar->insert("b", 0);
    nbar->insert("v", bar->value(_valueKey));
    _bars.insert(it.key(), nbar);
  }
  
  input = (Object *) oc->getObject(_baseObject);
  if (! input)
  {
    qDebug() << "CurveCloudObject::update: invalid" << _baseObject;
    return 0;
  }
  
  if (! input->message(&toc))
  {
    qDebug() << "CurveLineObject::update: message error" << input->plugin() << toc.command();
    return 0;
  }
  
  QMapIterator<int, Data *> it2(toc.map());
  while (it2.hasNext())
  {
    it2.next();
    Data *bar = it2.value();

    if (! bar->contains(_baseKey))
      continue;
    
    Data *nbar = _bars.value(it2.key());
    if (! nbar)
      continue;
    
    nbar->insert("b", bar->value(_baseKey));
  }

  // add to plot
  ObjectCommand toc2(QString("add_objects"));
  toc2.setValue(_label, (void *) this);
  if (! plot->message(&toc2))
  {
    qDebug() << "CurveCloudObject::update: message error" << plot->plugin() << toc2.command();
    return 0;
  }
  
  return 1;
}

int
CurveCloudObject::info (ObjectCommand *oc)
{
  // get index
  int index = oc->getInt(QString("index"));

  Data *bar = _bars.value(index);
  if (! bar)
    return 0;

  Data info;
  Util strip;
  QString ts;
  strip.strip(bar->value("v").toDouble(), 4, ts);
  info.insert(_label + QString(":H"), ts);

  strip.strip(bar->value("b").toDouble(), 4, ts);
  info.insert(_label + QString(":L"), ts);
  
  oc->setValue(QString("info"), info);

  return 1;
}

int
CurveCloudObject::highLowRange (ObjectCommand *oc)
{
  if (! _bars.size())
    return 0;
  
  // get plot
  QString key("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "CurveCloudObject::highLowRange: invalid" << key;
    return 0;
  }

  ObjectCommand toc(QString("set_high_low"));
  
  QMapIterator<int, Data *> it(_bars);
  while (it.hasNext())
  {
    it.next();
    Data *b = it.value();
    
    double v = b->value("v").toDouble();
    double h = b->value("b").toDouble();
    double l = h;
    
    if (v > h)
      h = v;
    
    if (v < l)
      v = l;
    
    toc.setValue(QString("index"), it.key());
    toc.setValue(QString("high"), h);
    toc.setValue(QString("low"), l);
    plot->message(&toc);
  }
  
  return 1;
}

int
CurveCloudObject::scalePoint (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  
  Data *b = _bars.value(index);
  if (! b)
    return 0;

  oc->setValue(QString("color"), _color);
  oc->setValue(QString("value"), b->value("v").toDouble());
  
  return 1;
}

int
CurveCloudObject::startEndIndexMessage (ObjectCommand *oc)
{
  int start = 0;
  int end = 0;
  if (! startEndIndex(start, end))
  {
    qDebug() << "CurveCloudObject::startEndIndexMessage: error";
    return 0;
  }
  
  oc->setValue(QString("start"), start);
  oc->setValue(QString("end"), end);
  
  return 1;
}

int
CurveCloudObject::startEndIndex (int &start, int &end)
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
CurveCloudObject::load (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "CurveCloudObject::load: invalid" << key;
    return 0;
  }

  _baseObject = settings->value(QString("base_object"), QString("symbol")).toString();
  _valueObject = settings->value(QString("value_object"), QString("symbol")).toString();
  _baseKey = settings->value(QString("base_key"), QString("L")).toString();
  _valueKey = settings->value(QString("value_key"), QString("H")).toString();
  _label = settings->value(QString("label"), QString("Cloud")).toString();
  _color = settings->value(QString("color"), QColor(Qt::red)).value<QColor>();
  _plotObject = settings->value(QString("plot_object")).toString();
  
  return 1;
}

int
CurveCloudObject::save (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "CurveCloudObject::save: invalid" << key;
    return 0;
  }

  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("base_object"), _baseObject);
  settings->setValue(QString("value_object"), _valueObject);
  settings->setValue(QString("base_key"), _baseKey);
  settings->setValue(QString("value_key"), _valueKey);
  settings->setValue(QString("label"), _label);
  settings->setValue(QString("color"), _color);
  settings->setValue(QString("plot_object"), _plotObject);

  return 1;
}

int
CurveCloudObject::dialog (ObjectCommand *oc)
{
  CurveCloudDialog *dialog = new CurveCloudDialog(oc->getObjects(), _name);
  dialog->setSettings(_color, _label, _baseObject, _baseKey, _valueObject, _valueKey, _plotObject);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
CurveCloudObject::dialogDone (void *dialog)
{
  CurveCloudDialog *d = (CurveCloudDialog *) dialog;
  d->settings(_color, _label, _baseObject, _baseKey, _valueObject, _valueKey, _plotObject);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
CurveCloudObject::copy (ObjectCommand *oc)
{
  QString key("input");
  Object *input = (Object *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "CurveCloudObject::copy: invalid" << key;
    return 0;
  }
  
  if (input->plugin() != _plugin)
  {
    qDebug() << "CurveCloudObject::copy: invalid" << key;
    return 0;
  }

  key = QString("output");
  ObjectCommand toc(key);
  if (! input->message(&toc))
  {
    qDebug() << "CurveCloudObject::copy: invalid" << input->plugin() << key;
    return 0;
  }
  
  qDeleteAll(_bars);
  _bars.clear();

  _baseObject = toc.getString(QString("base_object"));
  _valueObject = toc.getString(QString("value_object"));
  _baseKey = toc.getString(QString("base_key"));
  _valueKey = toc.getString(QString("value_key"));
  _label = toc.getString(QString("label"));
  _color = toc.getColor(QString("color"));
  _plotObject = toc.getString(QString("plot_object"));
  
  QMapIterator<int, Data *> it(toc.map());
  while (it.hasNext())
  {
    it.next();
    Data *b = it.value();
    
    Data *nb = new Data;
    nb->insert("b", b->value("b"));
    nb->insert("v", b->value("v"));
    _bars.insert(it.key(), nb);
  }
  
  return 1;
}

int
CurveCloudObject::output (ObjectCommand *oc)
{
  oc->setMap(_bars);
  settings(oc);
  return 1;
}

int
CurveCloudObject::setColor (ObjectCommand *oc)
{
/*
  int index = oc->getInt(QString("index"));
  QColor color = oc->getColor(QString("color"));
  
  Data *bar = _bars.value(index);
  if (! bar)
    return 0;
  
  bar->color = color;
*/

  _color = oc->getColor(QString("color"));
  return 1;
}

int
CurveCloudObject::settings (ObjectCommand *oc)
{
  oc->setValue(QString("base_object"), _baseObject);
  oc->setValue(QString("value_object"), _valueObject);
  oc->setValue(QString("base_key"), _baseKey);
  oc->setValue(QString("value_key"), _valueKey);
  oc->setValue(QString("label"), _label);
  oc->setValue(QString("color"), _color);
  oc->setValue(QString("plot_object"), _plotObject);

  QStringList tl;
  tl << "b" << "v";
  oc->setValue(QString("output_keys"), tl);

  tl.clear();
  tl << _label + "-L" << _label + "-H";
  oc->setValue(QString("output_labels"), tl);

  return 1;
}
