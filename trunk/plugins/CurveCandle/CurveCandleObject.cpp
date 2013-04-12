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

#include "CurveCandleObject.h"
#include "Util.h"

#include <QtDebug>

CurveCandleObject::CurveCandleObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("CurveCandle");
  _type = QString("curve");
  _penWidth = 1;
  _label = QString("Candle");
  _color = QColor(Qt::red);
  _inputObject = QString("symbol");
  _openKey = QString("O");
  _highKey = QString("H");
  _lowKey = QString("L");
  _closeKey = QString("C");

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

CurveCandleObject::~CurveCandleObject ()
{
  clear();
}

void
CurveCandleObject::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

int
CurveCandleObject::message (ObjectCommand *oc)
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

int
CurveCandleObject::draw (ObjectCommand *oc)
{
  QString key("painter");
  QPainter *p = (QPainter *) oc->getObject(key);
  if (! p)
  {
    qDebug() << "CurveCandleObject::draw: invalid" << key;
    return 0;
  }
  
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "CurveCandleObject::draw: invalid" << key;
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

  QRect rect = p->viewport();
  int x = 0;
  
  QPen pen = p->pen();
  pen.setWidth(_penWidth);
  pen.setStyle(Qt::SolidLine);

  ObjectCommand toc(QString("convert_to_y"));
  QString valueKey("value");
  QString yKey("y");
  int yo = 0;
  int yh = 0;
  int yl = 0;
  int yc = 0;
  while (x < rect.width() && pos <= end)
  {
    CandleBar *b = _bars.value(pos);
    if (b)
    {
      pen.setColor(b->color);
      p->setPen(pen);

      toc.setValue(valueKey, b->open);
      if (! plot->message(&toc))
	continue;
      yo = toc.getInt(yKey);

      toc.setValue(valueKey, b->high);
      if (! plot->message(&toc))
	continue;
      yh = toc.getInt(yKey);

      toc.setValue(valueKey, b->low);
      if (! plot->message(&toc))
	continue;
      yl = toc.getInt(yKey);
      
      toc.setValue(valueKey, b->close);
      if (! plot->message(&toc))
	continue;
      yc = toc.getInt(yKey);
      
      if (b->fill)
      {
        // filled candle c < o
        QRect r(QPoint(x + 2, yo), QPoint(x + width - 2, yc));
        p->drawLine (r.center().x(), yh, r.center().x(), yl);
        p->setBrush(b->color);
        p->drawRect(r);
      }
      else
      {
        // empty candle c > o
        QRect r(QPoint(x + 2, yc), QPoint(x + width - 2, yo));
        p->drawLine (r.center().x(), yh, r.center().x(), yl);
        p->setBrush(p->background().color());
        p->drawRect(r);
      }
    }
    
    x += width;
    pos++;
  }
  
//  p->setRenderHint(QPainter::Antialiasing, FALSE);
  return 1;
}

int
CurveCandleObject::info (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  
  CandleBar *bar = _bars.value(index);
  if (! bar)
    return 0;

  Data info;
  Util util;
  QString ts;
  util.strip(bar->open, 4, ts);
  info.insert(QString("O"), QVariant(ts));

  util.strip(bar->high, 4, ts);
  info.insert(QString("H"), QVariant(ts));

  util.strip(bar->low, 4, ts);
  info.insert(QString("L"), QVariant(ts));

  util.strip(bar->close, 4, ts);
  info.insert(QString("C"), QVariant(ts));

  oc->setValue(QString("info"), info);
  
  return 1;
}

int
CurveCandleObject::highLowRange (ObjectCommand *oc)
{
  if (! _bars.size())
    return 0;

  Object *plot = (Object *) oc->getObject(QString("plot"));
  if (! plot)
  {
    qDebug() << "CurveCandleObject::highLowRange: invalid Plot object";
    return 0;
  }

  ObjectCommand toc(QString("set_high_low"));
  
  QMapIterator<int, CandleBar *> it(_bars);
  while (it.hasNext())
  {
    it.next();
    CandleBar *b = it.value();

    toc.setValue(QString("index"), it.key());
    toc.setValue(QString("high"), b->high);
    toc.setValue(QString("low"), b->low);
    plot->message(&toc);
  }
  
  return 1;
}

int
CurveCandleObject::update (ObjectCommand *oc)
{
  clear();

  Object *input = (Object *) oc->getObject(_inputObject);
  if (! input)
  {
    qDebug() << "CurveCandleObject::setInput: invalid" << _inputObject;
    return 0;
  }

  Object *plot = (Object *) oc->getObject(_plotObject);
  if (! plot)
  {
    qDebug() << "CurveCandleObject::update: invalid plot" << _plotObject;
    return 0;
  }
  
  ObjectCommand toc(QString("output"));
  if (! input->message(&toc))
  {
    qDebug() << "CurveCandleObject::setInput: message error" << input->plugin() << toc.command();
    return 0;
  }
  
  Bars *obars = toc.getBars(_openKey);
  if (! obars)
  {
    qDebug() << "CurveCandleObject::setInput: invalid open bars" << _openKey;
    return 0;
  }
  
  Bars *hbars = toc.getBars(_highKey);
  if (! hbars)
  {
    qDebug() << "CurveCandleObject::setInput: invalid high bars" << _highKey;
    return 0;
  }
  
  Bars *lbars = toc.getBars(_lowKey);
  if (! lbars)
  {
    qDebug() << "CurveCandleObject::setInput: invalid low bars" << _lowKey;
    return 0;
  }
  
  Bars *cbars = toc.getBars(_closeKey);
  if (! cbars)
  {
    qDebug() << "CurveCandleObject::setInput: invalid close bars" << _closeKey;
    return 0;
  }

  QList<int> keys = cbars->_bars.keys();
  
  for (int pos = 0; pos < keys.size(); pos++)
  {
    Bar *obar = obars->value(keys.at(pos));
    if (! obar)
      continue;

    Bar *hbar = hbars->value(keys.at(pos));
    if (! hbar)
      continue;

    Bar *lbar = lbars->value(keys.at(pos));
    if (! lbar)
      continue;

    Bar *cbar = cbars->value(keys.at(pos));
    if (! cbar)
      continue;

    CandleBar *nbar = new CandleBar;
    nbar->fill = FALSE;
    nbar->color = _color;
    nbar->open = obar->v;
    nbar->high = hbar->v;
    nbar->low = lbar->v;
    nbar->close = cbar->v;
    if (nbar->close < nbar->open)
      nbar->fill = TRUE;
    _bars.insert(keys.at(pos), nbar);
  }

  // add to plot
  ObjectCommand toc2(QString("add_objects"));
  toc2.setValue(_label, (void *) this);
  if (! plot->message(&toc2))
  {
    qDebug() << "CurveCandleObject::update: message error" << plot->plugin() << toc2.command();
    return 0;
  }
  
  return 1;
}

int
CurveCandleObject::scalePoint (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  
  CandleBar *b = _bars.value(index);
  if (! b)
    return 0;

  oc->setValue(QString("color"), b->color);
  oc->setValue(QString("value"), b->close);
  
  return 1;
}

int
CurveCandleObject::startEndIndexMessage (ObjectCommand *oc)
{
  int start = 0;
  int end = 0;
  if (! startEndIndex(start, end))
  {
    qDebug() << "CurveCandleObject::startEndIndexMessage: error";
    return 0;
  }
  
  oc->setValue(QString("start"), start);
  oc->setValue(QString("end"), end);
  
  return 1;
}

int
CurveCandleObject::load (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "CurveCandleObject::load: invalid" << key;
    return 0;
  }

  _inputObject = settings->value(QString("input_object"), QString("symbol")).toString();
  _openKey = settings->value(QString("open_key"), QString("O")).toString();
  _highKey = settings->value(QString("high_key"), QString("H")).toString();
  _lowKey = settings->value(QString("low_key"), QString("L")).toString();
  _closeKey = settings->value(QString("close_key"), QString("C")).toString();
  _label = settings->value(QString("label"), QString("Candle")).toString();
  _color = settings->value(QString("color"), QColor(Qt::red)).value<QColor>();
  _plotObject = settings->value(QString("plot_object")).toString();
  
  return 1;
}

int
CurveCandleObject::save (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "CurveCandleObject::save: invalid" << key;
    return 0;
  }

  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("open_key"), _openKey);
  settings->setValue(QString("high_key"), _highKey);
  settings->setValue(QString("low_key"), _lowKey);
  settings->setValue(QString("close_key"), _closeKey);
  settings->setValue(QString("label"), _label);
  settings->setValue(QString("color"), _color);
  settings->setValue(QString("plot_object"), _plotObject);

  return 1;
}

int
CurveCandleObject::dialog (ObjectCommand *oc)
{
  CurveCandleDialog *dialog = new CurveCandleDialog(oc->getObjects(), _name);
  dialog->setSettings(_color, _label, _inputObject, _openKey, _highKey, _lowKey, _closeKey, _plotObject);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
CurveCandleObject::dialogDone (void *dialog)
{
  CurveCandleDialog *d = (CurveCandleDialog *) dialog;
  d->settings(_color, _label, _inputObject, _openKey, _highKey, _lowKey, _closeKey, _plotObject);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
CurveCandleObject::copy (ObjectCommand *oc)
{
  QString key("input");
  CurveCandleObject *input = (CurveCandleObject *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "CurveCandleObject::copy: invalid" << key;
    return 0;
  }
  
  if (input->plugin() != _plugin)
  {
    qDebug() << "CurveCandleObject::copy: invalid" << key;
    return 0;
  }

  key = QString("settings");
  ObjectCommand toc(key);
  if (! input->message(&toc))
  {
    qDebug() << "CurveCandleObject::copy: invalid" << input->plugin() << key;
    return 0;
  }
  
  qDeleteAll(_bars);
  _bars.clear();

  _inputObject = toc.getString(QString("input_object"));
  _openKey = toc.getString(QString("open_key"));
  _highKey = toc.getString(QString("high_key"));
  _lowKey = toc.getString(QString("low_key"));
  _closeKey = toc.getString(QString("close_key"));
  _label = toc.getString(QString("label"));
  _color = toc.getColor(QString("color"));
  _plotObject = toc.getString(QString("plot_object"));
  
  QMapIterator<int, CandleBar *> it(input->_bars);
  while (it.hasNext())
  {
    it.next();
    CandleBar *b = it.value();
    
    CandleBar *nb = new CandleBar;
    nb->open = b->open;
    nb->high = b->high;
    nb->low = b->low;
    nb->close = b->close;
    nb->color = b->color;
    nb->fill = b->fill;
    _bars.insert(it.key(), nb);
  }
  
  return 1;
}

int
CurveCandleObject::setColor (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  QColor color = oc->getColor(QString("color"));
  
  CandleBar *bar = _bars.value(index);
  if (! bar)
    return 0;
  
  bar->color = color;
  
  return 1;
}

int
CurveCandleObject::startEndIndex (int &start, int &end)
{
  start = 0;
  end = 0;
  
  QMapIterator<int, CandleBar *> it(_bars);
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
CurveCandleObject::settings (ObjectCommand *oc)
{
  oc->setValue(QString("input_object"), _inputObject);
  oc->setValue(QString("open_key"), _openKey);
  oc->setValue(QString("high_key"), _highKey);
  oc->setValue(QString("low_key"), _lowKey);
  oc->setValue(QString("close_key"), _closeKey);
  oc->setValue(QString("label"), _label);
  oc->setValue(QString("color"), _color);
  oc->setValue(QString("plot_object"), _plotObject);

  QStringList tl;
  tl << "O" << "H" << "L" << "C";
  oc->setValue(QString("output_keys"), tl);

  tl.clear();
  tl << tr("Open") << tr("High") << tr("Low") << tr("Close");
  oc->setValue(QString("output_labels"), tl);

  return 1;
}
