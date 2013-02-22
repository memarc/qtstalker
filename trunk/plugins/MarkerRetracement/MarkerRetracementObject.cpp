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

#include "MarkerRetracementObject.h"
#include "Util.h"
#include "MarkerRetracementDialog.h"
#include "fib.xpm"
#include "../../pics/edit.xpm"
#include "../../pics/delete.xpm"

#include <QDebug>

MarkerRetracementObject::MarkerRetracementObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("MarkerRetracement");
  _type = QString("marker");
  _penWidth = 1;
  _selected = 0;
  _modified = FALSE;
  _status = _NONE;
  _handleWidth = 6;
  _readOnly = FALSE;
  _startDate = QDateTime::currentDateTime();
  _endDate = _startDate;
  _highPrice = 0;
  _lowPrice = 0;
  _color = QColor(Qt::red);
  _extend = FALSE;
  _level1 = 0.382;
  _level2 = 0.5;
  _level3 = 0.618;
  _index = -1;
  _index2 = -1;
  _plotObject = QString("plot");
  
  _commandList << QString("draw");
  _commandList << QString("info");
  _commandList << QString("high_low_range");
  _commandList << QString("mouse_move");
  _commandList << QString("mouse_press");
  _commandList << QString("create");
  _commandList << QString("icon");
  _commandList << QString("set_read_only");
  _commandList << QString("dialog");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("update");
  _commandList << QString("copy");
  _commandList << QString("settings");
  
  load();
}

MarkerRetracementObject::~MarkerRetracementObject ()
{
  save();
}

int
MarkerRetracementObject::message (ObjectCommand *oc)
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
      rc = mouseMove(oc);
      break;
    case 4:
      rc = mousePress(oc);
      break;
    case 5:
      rc = create(oc);
      break;
    case 6:
      rc = icon(oc);
      break;
    case 7:
      _readOnly = oc->getBool(QString("read_only"));
      rc = 1;
      break;
    case 8:
      rc = dialog(oc);
      break;
    case 9:
      rc = load(oc);
      break;
    case 10:
      rc = save(oc);
      break;
    case 11:
      rc = update(oc);
      break;
    case 12:
      rc = copy(oc);
      break;
    case 13:
      rc = settings(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
MarkerRetracementObject::draw (ObjectCommand *oc)
{
  // get painter
  QString key("painter");
  QPainter *p = (QPainter *) oc->getObject(key);
  if (! p)
  {
    qDebug() << "MarkerRetracementObject::draw: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerRetracementObject::draw: invalid" << key;
    return 0;
  }

  int start = oc->getInt(QString("index"));
  int end = oc->getInt(QString("index_end"));
  
  // update index
  if (_index == -1)
  {
    ObjectCommand toc(QString("date_to_index"));
    toc.setValue(QString("date"), _startDate);
    if (! plot->message(&toc))
    {
      qDebug() << "MarkerRetracementObject::draw: object message error" << toc.command();
      return 0;
    }
    _index = toc.getInt(QString("index"));
  }
  
  // update index2
  if (_index2 == -1)
  {
    ObjectCommand toc(QString("date_to_index"));
    toc.setValue(QString("date"), _endDate);
    if (! plot->message(&toc))
    {
      qDebug() << "MarkerRetracementObject::draw: object message error" << toc.command();
      return 0;
    }
    _index2 = toc.getInt(QString("index"));
  }
  
  // are we visible?
  if (_index2 < start)
    return 0;
  if (_index > end)
    return 0;
  
  // get x
  ObjectCommand toc(QString("index_to_x"));
  toc.setValue(QString("index"), _index);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerRetracementObject::draw: object message error" << toc.command();
    return 0;
  }
  int x = toc.getInt(QString("x"));
  
  // get x2
  toc.setValue(QString("index"), _index2);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerRetracementObject::draw: object message error" << toc.command();
    return 0;
  }
  int x2 = toc.getInt(QString("x"));
  
  QRect rect = p->viewport();
  
  if (_extend)
  {
    // get end page index
    toc.setCommand(QString("end_page_index"));
    toc.setValue(QString("width"), rect.width());
    if (! plot->message(&toc))
    {
      qDebug() << "MarkerRetracementObject::draw: object message error" << toc.command();
      return 0;
    }
    x2 = toc.getInt(QString("index"));

    // translate index to x2 coord
    toc.setCommand(QString("index_to_x"));
    toc.setValue(QString("index"), x2);
    if (! plot->message(&toc))
    {
      qDebug() << "MarkerRetracementObject::draw: object message error" << toc.command();
      return 0;
    }
    x2 = toc.getInt(QString("x"));
  }
  
  if (x2 > rect.width())
    x2 = rect.width();

  QPen pen = p->pen();
  pen.setColor(_color);
  pen.setStyle(Qt::SolidLine);
  p->setPen(pen);

  _selectionArea.clear();

  QList<double> levels;
  levels << _level1 << _level2 << _level3;
  
  for (int pos = 0; pos < levels.size(); pos++)
  {
    if (levels.at(pos) == 0)
      continue;
    
    double td = levels.at(pos);
    double range = _highPrice - _lowPrice;
    double r = 0;
    if (td < 0)
      r = _lowPrice + (range * td);
    else
    {
      if (td > 0)
        r = _lowPrice + (range * td);
      else
      {
        if (td < 0)
          r = _highPrice;
        else
          r = _lowPrice;
      }
    }

    toc.setCommand(QString("convert_to_y"));
    toc.setValue(QString("value"), r);
    if (! plot->message(&toc))
    {
      qDebug() << "MarkerTextObject::draw: object message error" << toc.command();
      continue;
    }
    int y = toc.getInt(QString("y"));
    
    p->drawLine (x, y, x2, y);
    p->drawText(x, y - 1, QString::number(td * 100) + "% - " + QString::number(r));

    QPolygon array;
    array << QPoint(x, y - 4);
    array << QPoint(x, y + 4);
    array << QPoint(x2, y + 4);
    array << QPoint(x2, y - 4);
    _selectionArea.append(QRegion(array));
  }

  // draw the low line
  toc.setCommand(QString("convert_to_y"));
  toc.setValue(QString("value"), _lowPrice);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerTextObject::draw: object message error" << toc.command();
    return 0;
  }
  int y = toc.getInt(QString("y"));
  p->drawLine (x, y, x2, y);

  Util util;
  QString ts;
  util.strip(_lowPrice, 4, ts);
  p->drawText(x, y - 1, "0% - " + ts);

  // store the selectable area the low line occupies
  QPolygon array;
  array << QPoint(x, y - 4);
  array << QPoint(x, y + 4);
  array << QPoint(x2, y + 4);
  array << QPoint(x2, y - 4);
  _selectionArea.append(QRegion(array));

  // draw the high line
  toc.setValue(QString("value"), _highPrice);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerTextObject::draw: object message error" << toc.command();
    return 0;
  }
  int y2 = toc.getInt(QString("y"));
  p->drawLine (x, y2, x2, y2);
  
  util.strip(_highPrice, 4, ts);
  p->drawText(x, y2 - 1, "100% - " + ts);

  // store the selectable area the high line occupies
  array.clear();
  array << QPoint(x, y2 - 4);
  array << QPoint(x, y2 + 4);
  array << QPoint(x2, y2 + 4);
  array << QPoint(x2, y2 - 4);
  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    //top left corner
    _grabHandles.append(QRegion(x, y2 - (_handleWidth / 2),
                        _handleWidth,
                        _handleWidth,
                        QRegion::Rectangle));

    p->fillRect(x,
                y2 - (_handleWidth / 2),
                _handleWidth,
                _handleWidth,
                _color);

    //bottom right corner
    _grabHandles.append(QRegion(x2, y - (_handleWidth / 2),
                        _handleWidth,
                        _handleWidth,
                        QRegion::Rectangle));

    p->fillRect(x2,
                y - (_handleWidth / 2),
                _handleWidth,
                _handleWidth,
                _color);
  }
  
  return 1;
}

int
MarkerRetracementObject::info (ObjectCommand *oc)
{
  QMouseEvent *event = (QMouseEvent *) oc->getObject(QString("event"));
  if (! event)
  {
    qDebug() << "MarkerRetracementObject::info: invalid QMouseEvent";
    return 0;
  }
  
  if (! isSelected(event->pos()))
    return 0;
  
  Data info;
  info.insert(tr("Item"), QVariant(tr("Retracement")));
  
  info.insert(tr("SD"), QVariant(_startDate.toString("yyyy-MM-dd")));
  info.insert(tr("ST"), QVariant(_startDate.toString("HH:mm:ss")));
  
  Util strip;
  QString ts;
  strip.strip(_highPrice, 4, ts);
  info.insert(tr("High"), QVariant(ts));
  
  info.insert(tr("ED"), QVariant(_endDate.toString("yyyy-MM-dd")));
  info.insert(tr("ET"), QVariant(_endDate.toString("HH:mm:ss")));

  strip.strip(_lowPrice, 4, ts);
  info.insert(tr("Low"), QVariant(ts));

  QList<double> levels;
  levels << _level1 << _level2 << _level3;
  
  for (int pos = 0; pos < levels.size(); pos++)
  {
    QString key = tr("Level ") + QString::number(pos + 1);
    strip.strip(levels.at(pos), 4, ts);
    info.insert(key, QVariant(ts));
  }
  
  oc->setValue(QString("info"), info);
  
  return 1;
}

int
MarkerRetracementObject::highLowRange (ObjectCommand *oc)
{
  // get plot
  QString key("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerRetracementObject::highLowRange: invalid" << key;
    return 0;
  }

  ObjectCommand toc(QString("date_to_index"));
  toc.setValue(QString("date"), _startDate);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerRetracementObject::highLowRange: message error" << toc.command();
    return 0;
  }
  int index = toc.getInt(QString("index"));

  int index2 = index;
  if (_extend)
  {
    key = QString("size");
    toc.setCommand(key);
    if (! plot->message(&toc))
    {
      qDebug() << "MarkerRetracementObject::highLowRange: message error" << key;
      return 0;
    }
    index2 = toc.getInt(key);
    index2--;
  }
  else
  {
    toc.setCommand(QString("date_to_index"));
    toc.setValue(QString("date"), _endDate);
    if (! plot->message(&toc))
    {
      qDebug() << "MarkerRetracementObject::highLowRange: object message error" << toc.command();
      return 0;
    }
    index2 = toc.getInt(QString("index"));
  }

  toc.setCommand(QString("set_high_low"));
  for (int pos = index; pos <= index2; pos++)
  {
    toc.setValue(QString("index"), pos);
    toc.setValue(QString("high"), _highPrice);
    toc.setValue(QString("low"), _lowPrice);
    plot->message(&toc);
  }
  
  return 1;
}

int
MarkerRetracementObject::mouseMove (ObjectCommand *oc)
{
  // mouse event
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerRetracementObject::mouseMove: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerRetracementObject::mouseMove: invalid" << key;
    return 0;
  }
  
  switch (_status)
  {
    case _MOVING:
    {
      // translate x to index
      ObjectCommand toc(QString("x_to_index"));
      toc.setValue(QString("x"), event->pos().x());
      if (! plot->message(&toc))
        return 0;
      int index = toc.getInt(QString("index"));
      if (index == -1 || index > _index2)
	return 0;
      
      // translate y axis to price
      toc.setCommand(QString("convert_to_val"));
      toc.setValue(QString("y"), event->pos().y());
      if (! plot->message(&toc))
        return 0;
      double v = toc.getDouble(QString("value"));
      if (v <= _lowPrice)
        return 0;
      
      _index = index;
      _highPrice = v;
      _modified = TRUE;
      sendDrawMessage();
      break;
    }
    case _MOVING_2:
    {
      // translate x to index
      ObjectCommand toc(QString("x_to_index"));
      toc.setValue(QString("x"), event->pos().x());
      if (! plot->message(&toc))
        return 0;
      int index = toc.getInt(QString("index"));
      if (index == -1 || index < _index)
	return 0;
      
      // translate y axis to price
      toc.setCommand(QString("convert_to_val"));
      toc.setValue(QString("y"), event->pos().y());
      if (! plot->message(&toc))
        return 0;
      double v = toc.getDouble(QString("value"));
      if (v >= _highPrice)
        return 0;
      
      _index2 = index;
      _lowPrice = v;
      _modified = TRUE;
      sendDrawMessage();
      break;
    }
    default:
      break;
  }

  return 1;
}

int
MarkerRetracementObject::mousePress (ObjectCommand *oc)
{
  if (_readOnly)
    return 0;
  
  // mouse event
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerRetracementObject::mousePress: invalid" << key;
    return 0;
  }

  // context menu
  if (event->button() == Qt::RightButton && _selected == TRUE)
  {
    QMenu menu;
    QAction *edit = menu.addAction(QIcon(edit_xpm), tr("Edit"));
    menu.addAction(QIcon(delete_xpm), tr("Delete"));
    QAction *selected = menu.exec(QCursor::pos());
    
    if (! selected)
      return 0;
    
    if (selected == edit)
    {
      ObjectCommand toc;
      dialog(&toc);
    }
    else
      sendDeleteMessage();
    return 1;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerRetracementObject::mousePress: invalid" << key;
    return 0;
  }
  
  switch (_status)
  {
    case _CREATE:
    {
      // index
      ObjectCommand toc(QString("x_to_index"));
      toc.setValue(QString("x"), event->pos().x());
      if (! plot->message(&toc))
        return 0;
      int index = toc.getInt(QString("index"));
      if (index == -1)
	return 0;

      // startDate
      toc.setCommand(QString("index_to_date"));
      toc.setValue(QString("index"), index);
      if (! plot->message(&toc))
        return 0;
      QDateTime dt = toc.getDate(QString("date"));
      
      // high price
      toc.setCommand(QString("convert_to_val"));
      toc.setValue(QString("y"), event->pos().y());
      if (! plot->message(&toc))
        return 0;
      double v = toc.getDouble(QString("value"));
      
      _index = index;
      _startDate = dt;
      _highPrice = v;

      _status = _MOVING_2;
      sendDrawMessage();
      break;
    }
    case _SELECTED:
    {
      int grab = isGrabSelected(event->pos());
      if (grab)
      {
        _status = _MOVING;
        if (grab == 2)
          _status = _MOVING_2;
	sendMovingMessage();
        return 1;
      }
      
      if (! isSelected(event->pos()))
      {
        // we have deselected marker
        _status = _NONE;
	_selected = FALSE;
	sendUnselectedMessage();
      }
      break;
    }
    case _MOVING:
    {
      // index
      ObjectCommand toc(QString("x_to_index"));
      toc.setValue(QString("x"), event->pos().x());
      if (! plot->message(&toc))
        return 0;
      int index = toc.getInt(QString("index"));

      // startDate
      toc.setCommand(QString("index_to_date"));
      toc.setValue(QString("index"), index);
      if (! plot->message(&toc))
        return 0;
      QDateTime dt = toc.getDate(QString("date"));
      
      // high price
      toc.setCommand(QString("convert_to_val"));
      toc.setValue(QString("y"), event->pos().y());
      if (! plot->message(&toc))
        return 0;
      double v = toc.getDouble(QString("value"));
      
      _index = index;
      _startDate = dt;
      _highPrice = v;

      _status = _SELECTED;
      sendDrawMessage();
      break;
    }
    case _MOVING_2:
    {
      // index2
      ObjectCommand toc(QString("x_to_index"));
      toc.setValue(QString("x"), event->pos().x());
      if (! plot->message(&toc))
        return 0;
      int index = toc.getInt(QString("index"));

      // endDate
      toc.setCommand(QString("index_to_date"));
      toc.setValue(QString("index"), index);
      if (! plot->message(&toc))
        return 0;
      QDateTime dt = toc.getDate(QString("date"));
      
      // low price
      toc.setCommand(QString("convert_to_val"));
      toc.setValue(QString("y"), event->pos().y());
      if (! plot->message(&toc))
        return 0;
      double v = toc.getDouble(QString("value"));
      
      _index2 = index;
      _endDate = dt;
      _lowPrice = v;

      _status = _SELECTED;
      sendDrawMessage();
      break;
    }
    default:
    {
      if (! isSelected(event->pos()))
	break;
      
      _status = _SELECTED;
      _selected = TRUE;
      sendSelectedMessage();
      break;
    }
  }
  
  return 1;
}

int
MarkerRetracementObject::create (ObjectCommand *)
{
  _status = _CREATE;
  _selected = TRUE;
  return 1;
}

int
MarkerRetracementObject::icon (ObjectCommand *oc)
{
  oc->setValue(QString("icon"), QIcon(fib_xpm));
  return 1;
}

int
MarkerRetracementObject::isSelected (QPoint p)
{
  for (int pos = 0; pos < _selectionArea.size(); pos++)
  {
    QRegion r = _selectionArea.at(pos);
    if (r.contains(p))
      return 1;
  }

  return 0;
}

int
MarkerRetracementObject::isGrabSelected (QPoint p)
{
  for (int pos = 0; pos < _grabHandles.size(); pos++)
  {
    QRegion r = _grabHandles.at(pos);
    if (r.contains(p))
      return pos + 1;
  }

  return 0;
}

int
MarkerRetracementObject::load ()
{
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_name, QSettings::NativeFormat);
  
  _color = settings.value(QString("color"), QColor(Qt::red)).value<QColor>();
  _startDate = settings.value(QString("start_date"), QDateTime::currentDateTime()).toDateTime();
  _endDate = settings.value(QString("end_date"), QDateTime::currentDateTime()).toDateTime();
  _highPrice = settings.value(QString("high_price"), 0).toDouble();
  _lowPrice = settings.value(QString("low_price"), 0).toDouble();
  _extend = settings.value(QString("extend"), FALSE).toBool();
  _level1 = settings.value(QString("level_1"), 0.382).toDouble();
  _level2 = settings.value(QString("level_2"), 0.5).toDouble();
  _level3 = settings.value(QString("level_3"), 0.618).toDouble();
  
  _index = -1;
  _index2 = -1;
  _modified = FALSE;
  _readOnly = FALSE;
  
  return 1;
}

int
MarkerRetracementObject::save ()
{
  if (! _modified)
    return 0;
  
  if (_readOnly)
    return 0;
  
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_name, QSettings::NativeFormat);

  settings.setValue(QString("color"), _color);
  settings.setValue(QString("start_date"), _startDate);
  settings.setValue(QString("end_date"), _endDate);
  settings.setValue(QString("high_price"), _highPrice);
  settings.setValue(QString("low_price"), _lowPrice);
  settings.setValue(QString("extend"), _extend);
  settings.setValue(QString("level_1"), _level1);
  settings.setValue(QString("level_2"), _level2);
  settings.setValue(QString("level_3"), _level3);
  
  _modified = FALSE;
  
  return 1;
}

void
MarkerRetracementObject::dialogDone (void *dialog)
{
  MarkerRetracementDialog *d = (MarkerRetracementDialog *) dialog;
  d->settings(_color, _startDate, _endDate, _highPrice, _lowPrice, _extend, _level1, _level2, _level3, _plotObject);

  _index = -1;
  _index2 = -1;
  _modified = TRUE;

  sendDrawMessage();
}

void
MarkerRetracementObject::sendDrawMessage ()
{
  ObjectCommand oc(QString("draw"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerRetracementObject::sendSelectedMessage ()
{
  ObjectCommand oc(QString("selected"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerRetracementObject::sendUnselectedMessage ()
{
  ObjectCommand oc(QString("unselected"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerRetracementObject::sendMovingMessage ()
{
  ObjectCommand oc(QString("moving"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerRetracementObject::sendDeleteMessage ()
{
  QDir dir;
  dir.remove(_name);
  _modified = FALSE;

  ObjectCommand oc(QString("delete"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

int
MarkerRetracementObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MarkerRetracementObject::load: invalid QSettings";
    return 0;
  }

  _color = settings->value(QString("color"), QColor(Qt::red)).value<QColor>();
  _startDate = settings->value(QString("start_date"), QDateTime::currentDateTime()).toDateTime();
  _endDate = settings->value(QString("end_date"), QDateTime::currentDateTime()).toDateTime();
  _highPrice = settings->value(QString("high_price"), 0).toDouble();
  _lowPrice = settings->value(QString("low_price"), 0).toDouble();
  _level1 = settings->value(QString("level_1"), 0.382).toDouble();
  _level2 = settings->value(QString("level_2"), 0.5).toDouble();
  _level3 = settings->value(QString("level_3"), 0.618).toDouble();
  _extend = settings->value(QString("extend"), FALSE).toBool();
  _plotObject = settings->value(QString("plot"), QString("plot")).toString();
  _readOnly = settings->value(QString("read_only"), TRUE).toBool();
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerRetracementObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MarkerRetracementObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("color"), _color);
  settings->setValue(QString("start_date"), _startDate);
  settings->setValue(QString("end_date"), _endDate);
  settings->setValue(QString("high_price"), _highPrice);
  settings->setValue(QString("low_price"), _lowPrice);
  settings->setValue(QString("extend"), _extend);
  settings->setValue(QString("level_1"), _level1);
  settings->setValue(QString("level_2"), _level2);
  settings->setValue(QString("level_3"), _level3);
  settings->setValue(QString("plot"), _plotObject);
  settings->setValue(QString("read_only"), _readOnly);
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerRetracementObject::update (ObjectCommand *oc)
{
  Object *plot = (Object *) oc->getObject(_plotObject);
  if (! plot)
  {
    qDebug() << "MarkerRetracementObject::update: invalid plot" << _plotObject;
    return 0;
  }
  
  // add to plot
  ObjectCommand toc(QString("add_objects"));
  toc.setValue(_name, (void *) this);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerRetracementObject::update: message error" << plot->plugin() << toc.command();
    return 0;
  }
  
  return 1;
}

int
MarkerRetracementObject::copy (ObjectCommand *oc)
{
  QString key("input");
  Object *input = (Object *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "MarkerRetracementObject::copy: invalid" << key;
    return 0;
  }
  
  if (input->plugin() != _plugin)
  {
    qDebug() << "MarkerRetracementObject::copy: invalid" << key;
    return 0;
  }

  key = QString("settings");
  ObjectCommand toc(key);
  if (! input->message(&toc))
  {
    qDebug() << "MarkerRetracementObject::copy: invalid" << input->plugin() << key;
    return 0;
  }
  
  _color = toc.getColor(QString("color"));
  _startDate = toc.getDate(QString("start_date"));
  _endDate = toc.getDate(QString("end_date"));
  _highPrice = toc.getDouble(QString("high_price"));
  _lowPrice = toc.getDouble(QString("low_price"));
  _extend = toc.getBool(QString("extend"));
  _level1 = toc.getDouble(QString("level_1"));
  _level2 = toc.getDouble(QString("level_2"));
  _level3 = toc.getDouble(QString("level_3"));
  _plotObject = toc.getString(QString("plot"));
  _readOnly = toc.getBool(QString("read_only"));

  return 1;
}

int
MarkerRetracementObject::settings (ObjectCommand *oc)
{
  oc->setValue(QString("color"), _color);
  oc->setValue(QString("start_date"), _startDate);
  oc->setValue(QString("end_date"), _endDate);
  oc->setValue(QString("high_price"), _highPrice);
  oc->setValue(QString("low_price"), _lowPrice);
  oc->setValue(QString("extend"), _extend);
  oc->setValue(QString("level_1"), _level1);
  oc->setValue(QString("level_2"), _level2);
  oc->setValue(QString("level_3"), _level3);
  oc->setValue(QString("plot"), _plotObject);
  oc->setValue(QString("read_only"), _readOnly);
  return 1;
}

int
MarkerRetracementObject::dialog (ObjectCommand *oc)
{
  MarkerRetracementDialog *d = new MarkerRetracementDialog(oc->getObjects(), _name);
  d->setSettings(_color, _startDate, _endDate, _highPrice, _lowPrice, _extend, _level1, _level2, _level3, _plotObject);
  connect(d, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  d->setModified(FALSE);
  d->show();
  return 1;
}

