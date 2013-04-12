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

#include "CurveHistogramObject.h"
#include "Util.h"

#include <QtDebug>

CurveHistogramObject::CurveHistogramObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("CurveHistogram");
  _type = QString("curve");
  _penWidth = 1;
  _color = QColor(Qt::red);
  _label = tr("Histogram Bar");
   _inputObject = QString("symbol");
  _inputKey = QString("C");
 
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
  _commandList << QString("set_color");
  _commandList << QString("settings");
}

CurveHistogramObject::~CurveHistogramObject ()
{
  clear();
}

int
CurveHistogramObject::message (ObjectCommand *oc)
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
      rc = setColor(oc);
      break;
    case 11:
      rc = settings(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

void
CurveHistogramObject::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

int
CurveHistogramObject::draw (ObjectCommand *oc)
{
  // get painter
  QString key("painter");
  QPainter *p = (QPainter *) oc->getObject(key);
  if (! p)
  {
    qDebug() << "CurveHistogramObject::draw: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "CurveHistogramObject::draw: invalid" << key;
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
  toc.setValue(QString("value"), 0);
  if (! plot->message(&toc))
  {
    qDebug() << "CurveHistogramObject::draw: base error";
    return 0;
  }
  int zero = toc.getInt(QString("y"));

  QList<QPoint> bpoints;
  QPolygon hist;      
  int x = 0;
  int x2 = width;
  HistogramBar *pbar = 0;
  while (x < rect.width() && pos <= end)
  {
    HistogramBar *bar = _bars.value(pos);
    if (bar)
    {
      if (! pbar)
      {
        pbar = bar;
        continue;
      }

      toc.setValue(QString("value"), pbar->value);
      if (! plot->message(&toc))
      {
	qDebug() << "CurveHistogramObject::draw: value error";
	continue;
      }
      int y = toc.getInt(QString("y"));

      toc.setValue(QString("value"), bar->value);
      if (! plot->message(&toc))
      {
	qDebug() << "CurveHistogramObject::draw: value error";
	continue;
      }
      int y2 = toc.getInt(QString("y"));

      QPolygon hist;      
      if (y2 <= zero) // above zero line
      {
        hist << QPoint(x, zero); // bottom left
        hist << QPoint(x, y); // top left
        hist << QPoint(x2, y2); // top right
        hist << QPoint(x2, zero); // bottom right
      }
      else
      {
        hist << QPoint(x, y); // bottom left
        hist << QPoint(x, zero); // top left
        hist << QPoint(x2, zero); // top right
        hist << QPoint(x2, y2); // bottom right
      }

      p->setBrush(bar->color);
      p->drawPolygon(hist, Qt::OddEvenFill);

      pbar = bar;
    }

    x = x2;
    x2 += width;
    pos++;
  }
  
  return 1;
}

int
CurveHistogramObject::update (ObjectCommand *oc)
{
  clear();

  Object *input = (Object *) oc->getObject(_inputObject);
  if (! input)
  {
    qDebug() << "CurveHistogramObject::update: invalid" << _inputObject;
    return 0;
  }

  Object *plot = (Object *) oc->getObject(_plotObject);
  if (! plot)
  {
    qDebug() << "CurveHistogramObject::update: invalid plot" << _plotObject;
    return 0;
  }
  
  ObjectCommand toc(QString("output"));
  if (! input->message(&toc))
  {
    qDebug() << "CurveHistogramObject::update: message error" << input->plugin() << toc.command();
    return 0;
  }
  
  Bars *ibars = toc.getBars(_inputKey);
  if (! ibars)
  {
    qDebug() << "CurveHistogramObject::setInput: invalid input bars" << _inputKey;
    return 0;
  }
  
  QMapIterator<int, Bar *> it(ibars->_bars);
  while (it.hasNext())
  {
    it.next();
    Bar *bar = it.value();

    HistogramBar *nbar = new HistogramBar;
    nbar->color = _color;
    nbar->value = bar->v;
    _bars.insert(it.key(), nbar);
  }

  // add to plot
  ObjectCommand toc2(QString("add_objects"));
  toc2.setValue(_label, (void *) this);
  if (! plot->message(&toc2))
  {
    qDebug() << "CurveHistogramObject::update: message error" << plot->plugin() << toc2.command();
    return 0;
  }
  
  return 1;
}

int
CurveHistogramObject::info (ObjectCommand *oc)
{
  // get index
  int index = oc->getInt(QString("index"));

  HistogramBar *bar = _bars.value(index);
  if (! bar)
    return 0;

  Util strip;
  QString ts;
  strip.strip(bar->value, 4, ts);
  
  Data info;
  info.insert(_label, ts);
  oc->setValue(QString("info"), info);

  return 1;
}

int
CurveHistogramObject::highLowRange (ObjectCommand *oc)
{
  if (! _bars.size())
    return 0;
  
  // get plot
  QString key("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "CurveHistogramObject::highLowRange: invalid" << key;
    return 0;
  }

  ObjectCommand toc(QString("set_high_low"));
  
  QMapIterator<int, HistogramBar *> it(_bars);
  while (it.hasNext())
  {
    it.next();
    HistogramBar *b = it.value();
    
    double h = b->value;
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
CurveHistogramObject::scalePoint (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  
  HistogramBar *b = _bars.value(index);
  if (! b)
    return 0;

  oc->setValue(QString("color"), b->color);
  oc->setValue(QString("value"), b->value);
  
  return 1;
}

int
CurveHistogramObject::startEndIndexMessage (ObjectCommand *oc)
{
  int start = 0;
  int end = 0;
  if (! startEndIndex(start, end))
  {
    qDebug() << "CurveHistogramObject::startEndIndexMessage: error";
    return 0;
  }
  
  oc->setValue(QString("start"), start);
  oc->setValue(QString("end"), end);
  
  return 1;
}

int
CurveHistogramObject::startEndIndex (int &start, int &end)
{
  start = 0;
  end = 0;
  
  QMapIterator<int, HistogramBar *> it(_bars);
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
CurveHistogramObject::load (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "CurveHistogramObject::load: invalid" << key;
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
CurveHistogramObject::save (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "CurveHistogramObject::save: invalid" << key;
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
CurveHistogramObject::dialog (ObjectCommand *oc)
{
  CurveHistogramDialog *dialog = new CurveHistogramDialog(oc->getObjects(), _name);
  dialog->setSettings(_color, _label, _inputObject, _inputKey, _plotObject);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
CurveHistogramObject::dialogDone (void *dialog)
{
  CurveHistogramDialog *d = (CurveHistogramDialog *) dialog;
  d->settings(_color, _label, _inputObject, _inputKey, _plotObject);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
CurveHistogramObject::copy (ObjectCommand *oc)
{
  QString key("input");
  CurveHistogramObject *input = (CurveHistogramObject *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "CurveHistogramObject::copy: invalid" << key;
    return 0;
  }
  
  if (input->plugin() != _plugin)
  {
    qDebug() << "CurveHistogramObject::copy: invalid" << key;
    return 0;
  }

  key = QString("settings");
  ObjectCommand toc(key);
  if (! input->message(&toc))
  {
    qDebug() << "CurveHistogramObject::copy: invalid" << input->plugin() << key;
    return 0;
  }
  
  qDeleteAll(_bars);
  _bars.clear();

  _inputObject = toc.getString(QString("input_object"));
  _inputKey = toc.getString(QString("input_key"));
  _label = toc.getString(QString("label"));
  _color = toc.getColor(QString("color"));
  _plotObject = toc.getString(QString("plot_object"));
  
  QMapIterator<int, HistogramBar *> it(input->_bars);
  while (it.hasNext())
  {
    it.next();
    HistogramBar *b = it.value();
    
    HistogramBar *nb = new HistogramBar;
    nb->color = b->color;
    nb->value = b->value;
    _bars.insert(it.key(), nb);
  }
  
  return 1;
}

int
CurveHistogramObject::setColor (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  QColor color = oc->getColor(QString("color"));
  
  HistogramBar *bar = _bars.value(index);
  if (! bar)
    return 0;
  
  bar->color = color;
  
  return 1;
}

int
CurveHistogramObject::settings (ObjectCommand *oc)
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
