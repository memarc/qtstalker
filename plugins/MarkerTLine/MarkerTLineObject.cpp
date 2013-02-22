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

#include "MarkerTLineObject.h"
#include "Util.h"
#include "MarkerTLineDialog.h"
#include "trend.xpm"
#include "../../pics/edit.xpm"
#include "../../pics/delete.xpm"

#include <QDebug>

MarkerTLineObject::MarkerTLineObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("MarkerTLine");
  _type = QString("marker");
  _penWidth = 1;
  _color = QColor(Qt::red);
  _selected = 0;
  _modified = FALSE;
  _status = _NONE;
  _handleWidth = 6;
  _readOnly = FALSE;
  _startDate = QDateTime::currentDateTime();
  _endDate = _startDate;
  _startPrice = 0;
  _endPrice = 0;
  _extend = FALSE;
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

MarkerTLineObject::~MarkerTLineObject ()
{
  save();
}

int
MarkerTLineObject::message (ObjectCommand *oc)
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
MarkerTLineObject::draw (ObjectCommand *oc)
{
  // get painter
  QString key("painter");
  QPainter *p = (QPainter *) oc->getObject(key);
  if (! p)
  {
    qDebug() << "MarkerTLineObject::draw: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerTLineObject::draw: invalid" << key;
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
      qDebug() << "MarkerTLineObject::draw: object message error" << toc.command();
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
      qDebug() << "MarkerTLineObject::draw: object message error" << toc.command();
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
    qDebug() << "MarkerTLineObject::draw: object message error" << toc.command();
    return 0;
  }
  int x = toc.getInt(QString("x"));
  
  // get x2
  toc.setValue(QString("index"), _index2);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerTLineObject::draw: object message error" << toc.command();
    return 0;
  }
  int x2 = toc.getInt(QString("x"));

  // get y
  toc.setCommand(QString("convert_to_y"));
  toc.setValue(QString("value"), _startPrice);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerTLineObject::draw: object message error" << toc.command();
    return 0;
  }
  int y = toc.getInt(QString("y"));

  // get y2
  toc.setValue(QString("value"), _endPrice);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerTLineObject::draw: object message error" << toc.command();
    return 0;
  }
  int y2 = toc.getInt(QString("y"));
  
  QPen pen = p->pen();
  pen.setColor(_color);
  pen.setStyle(Qt::SolidLine);
  p->setPen(pen);

  p->drawLine (x, y, x2, y2);

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  QRect rect = p->viewport();
  
  if (_extend)
  {
    int ydiff = y - y2;
    int xdiff = x2 - x;
    if (xdiff > 0)
    {
      while (x2 < rect.width())
      {
        x = x2;
        x2 = x2 + xdiff;
        
        y = y2;
        y2 = y2 - ydiff;
        
        p->drawLine (x, y, x2, y2);
      }
    }
  }

  // store the selectable area the line occupies
  _selectionArea.clear();

  QPolygon array;
  array << QPoint(tx, ty - 4);
  array << QPoint(tx, ty + 4);
  array << QPoint(x2, y2 + 4);
  array << QPoint(x2, y2 - 4);

  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(tx,
                                ty - (_handleWidth / 2),
                                _handleWidth,
                                _handleWidth,
                                QRegion::Rectangle));

    p->fillRect(tx,
                ty - (_handleWidth / 2),
                _handleWidth,
                _handleWidth,
                _color);

    _grabHandles.append(QRegion(tx2,
                                ty2 - (_handleWidth / 2),
                                _handleWidth,
                                _handleWidth,
                                QRegion::Rectangle));

    p->fillRect(tx2,
                ty2 - (_handleWidth / 2),
                _handleWidth,
                _handleWidth,
                _color);
  }
  
  return 1;
}

int
MarkerTLineObject::info (ObjectCommand *oc)
{
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerTLineObject::info: invalid" << key;
    return 0;
  }
  
  if (! isSelected(event->pos()))
    return 0;
  
  Data info;
  info.insert(tr("Item"), QVariant(tr("TLine")));
  
  info.insert(tr("SD"), QVariant(_startDate.toString("yyyy-MM-dd")));
  info.insert(tr("ST"), QVariant(_startDate.toString("HH:mm:ss")));
  
  Util strip;
  QString ts;
  strip.strip(_startPrice, 4, ts);
  info.insert(tr("Start Price"), QVariant(ts));
  
  info.insert(tr("ED"), QVariant(_endDate.toString("yyyy-MM-dd")));
  info.insert(tr("ET"), QVariant(_endDate.toString("HH:mm:ss")));
  
  strip.strip(_endPrice, 4, ts);
  info.insert(tr("End Price"), QVariant(ts));
  
  oc->setValue(QString("info"), info);

  return 1;
}

int
MarkerTLineObject::highLowRange (ObjectCommand *oc)
{
  // get plot
  QString key("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerTLineObject::highLowRange: invalid" << key;
    return 0;
  }

  ObjectCommand toc(QString("date_to_index"));
  toc.setValue(QString("date"), _startDate);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerTLineObject::highLowRange: object message error" << toc.command();
    return 0;
  }
  int index = toc.getInt(QString("index"));

  int index2 = index;
  if (_extend)
  {
    toc.setCommand(QString("size"));
    if (! plot->message(&toc))
    {
      qDebug() << "MarkerTLineObject::highLowRange: message error" << toc.command();
      return 0;
    }
    index2 = toc.getInt(QString("size"));
    index2--;
  }
  else
  {
    toc.setCommand(QString("date_to_index"));
    toc.setValue(QString("date"), _endDate);
    if (! plot->message(&toc))
    {
      qDebug() << "MarkerTLineObject::highLowRange: message error" << toc.command();
      return 0;
    }
    index2 = toc.getInt(QString("index"));
  }

  double high = _startPrice;
  if (_endPrice > high)
    high = _endPrice;
  
  double low = _startPrice;
  if (_endPrice < low)
    low = _endPrice;
  
  toc.setCommand(QString("set_high_low"));
  for (int pos = index; pos <= index2; pos++)
  {
    toc.setValue(QString("index"), pos);
    toc.setValue(QString("high"), high);
    toc.setValue(QString("low"), low);
    plot->message(&toc);
  }
  
  return 1;
}

int
MarkerTLineObject::mouseMove (ObjectCommand *oc)
{
  // mouse event
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerTLineObject::mouseMove: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerTLineObject::mouseMove: invalid" << key;
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
      
      _index = index;
      _startPrice = v;
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
      
      _index2 = index;
      _endPrice = v;
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
MarkerTLineObject::mousePress (ObjectCommand *oc)
{
  if (_readOnly)
    return 0;
  
  // mouse event
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerTLineObject::mousePress: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerTLineObject::mousePress: invalid" << key;
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
      
      // start price
      toc.setCommand(QString("convert_to_val"));
      toc.setValue(QString("y"), event->pos().y());
      if (! plot->message(&toc))
        return 0;
      double v = _startPrice = toc.getDouble(QString("value"));
      
      _index = index;
      _startDate = dt;
      _startPrice = v;

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
      
      // start price
      toc.setCommand(QString("convert_to_val"));
      toc.setValue(QString("y"), event->pos().y());
      if (! plot->message(&toc))
        return 0;
      double v = toc.getDouble(QString("value"));
      
      _index = index;
      _startDate = dt;
      _startPrice = v;

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
      
      // end price
      toc.setCommand(QString("convert_to_val"));
      toc.setValue(QString("y"), event->pos().y());
      if (! plot->message(&toc))
        return 0;
      double v = toc.getDouble(QString("value"));
      
      _index2 = index;
      _endDate = dt;
      _endPrice = v;

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
MarkerTLineObject::create (ObjectCommand *)
{
  _status = _CREATE;
  _selected = TRUE;
  return 1;
}

int
MarkerTLineObject::icon (ObjectCommand *oc)
{
  oc->setValue(QString("icon"), QIcon(trend_xpm));
  return 1;
}

int
MarkerTLineObject::isSelected (QPoint p)
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
MarkerTLineObject::isGrabSelected (QPoint p)
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
MarkerTLineObject::load ()
{
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_name, QSettings::NativeFormat);
  
  _color = settings.value(QString("color"), QColor(Qt::red)).value<QColor>();
  _startDate = settings.value(QString("start_date"), QDateTime::currentDateTime()).toDateTime();
  _endDate = settings.value(QString("end_date"), QDateTime::currentDateTime()).toDateTime();
  _startPrice = settings.value(QString("start_price"), 0).toDouble();
  _endPrice = settings.value(QString("end_price"), 0).toDouble();
  _extend = settings.value(QString("extend"), FALSE).toBool();

  _index = -1;
  _index2 = -1;
  _modified = FALSE;
  _readOnly = FALSE;
  
  return 1;
}

int
MarkerTLineObject::save ()
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
  settings.setValue(QString("start_price"), _startPrice);
  settings.setValue(QString("end_price"), _endPrice);
  settings.setValue(QString("extend"), _extend);
  
  _modified = FALSE;
  
  return 1;
}

void
MarkerTLineObject::dialogDone (void *dialog)
{
  MarkerTLineDialog *d = (MarkerTLineDialog *) dialog;
  d->settings(_color, _startDate, _endDate, _startPrice, _endPrice, _extend, _plotObject);

  _index = -1;
  _index2 = -1;
  _modified = TRUE;

  sendDrawMessage();
}

void
MarkerTLineObject::sendDrawMessage ()
{
  ObjectCommand oc(QString("draw"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerTLineObject::sendSelectedMessage ()
{
  ObjectCommand oc(QString("selected"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerTLineObject::sendUnselectedMessage ()
{
  ObjectCommand oc(QString("unselected"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerTLineObject::sendMovingMessage ()
{
  ObjectCommand oc(QString("moving"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerTLineObject::sendDeleteMessage ()
{
  QDir dir;
  dir.remove(_name);
  _modified = FALSE;
  
  ObjectCommand oc(QString("delete"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

int
MarkerTLineObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MarkerTLineObject::load: invalid QSettings";
    return 0;
  }
  
  _color = settings->value(QString("color"), QColor(Qt::red)).value<QColor>();
  _startDate = settings->value(QString("start_date"), QDateTime::currentDateTime()).toDateTime();
  _endDate = settings->value(QString("end_date"), QDateTime::currentDateTime()).toDateTime();
  _startPrice = settings->value(QString("start_price"), 0).toDouble();
  _endPrice = settings->value(QString("end_price"), 0).toDouble();
  _extend = settings->value(QString("extend"), FALSE).toBool();
  _plotObject = settings->value(QString("plot"), QString("plot")).toString();
  _readOnly = settings->value(QString("read_only"), TRUE).toBool();
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerTLineObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MarkerTLineObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("color"), _color);
  settings->setValue(QString("start_date"), _startDate);
  settings->setValue(QString("end_date"), _endDate);
  settings->setValue(QString("start_price"), _startPrice);
  settings->setValue(QString("end_price"), _endPrice);
  settings->setValue(QString("extend"), _extend);
  settings->setValue(QString("plot"), _plotObject);
  settings->setValue(QString("read_only"), _readOnly);
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerTLineObject::update (ObjectCommand *oc)
{
  Object *plot = (Object *) oc->getObject(_plotObject);
  if (! plot)
  {
    qDebug() << "MarkerTLineObject::update: invalid plot" << _plotObject;
    return 0;
  }
  
  // add to plot
  ObjectCommand toc(QString("add_objects"));
  toc.setValue(_name, (void *) this);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerTLineObject::update: message error" << plot->plugin() << toc.command();
    return 0;
  }
  
  return 1;
}

int
MarkerTLineObject::copy (ObjectCommand *oc)
{
  QString key("input");
  Object *input = (Object *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "MarkerTLineObject::copy: invalid" << key;
    return 0;
  }
  
  if (input->plugin() != _plugin)
  {
    qDebug() << "MarkerTLineObject::copy: invalid" << key;
    return 0;
  }

  key = QString("settings");
  ObjectCommand toc(key);
  if (! input->message(&toc))
  {
    qDebug() << "MarkerTLineObject::copy: invalid" << input->plugin() << key;
    return 0;
  }
  
  _color = toc.getColor(QString("color"));
  _startDate = toc.getDate(QString("start_date"));
  _endDate = toc.getDate(QString("end_date"));
  _startPrice = toc.getDouble(QString("start_price"));
  _endPrice = toc.getDouble(QString("end_price"));
  _extend = toc.getBool(QString("extend"));
  _plotObject = toc.getString(QString("plot"));
  _readOnly = toc.getBool(QString("read_only"));

  return 1;
}

int
MarkerTLineObject::settings (ObjectCommand *oc)
{
  oc->setValue(QString("color"), _color);
  oc->setValue(QString("start_date"), _startDate);
  oc->setValue(QString("end_date"), _endDate);
  oc->setValue(QString("start_price"), _startPrice);
  oc->setValue(QString("end_price"), _endPrice);
  oc->setValue(QString("extend"), _extend);
  oc->setValue(QString("plot"), _plotObject);
  oc->setValue(QString("read_only"), _readOnly);
  return 1;
}

int
MarkerTLineObject::dialog (ObjectCommand *oc)
{
  MarkerTLineDialog *d = new MarkerTLineDialog(oc->getObjects(), _name);
  d->setSettings(_color, _startDate, _endDate, _startPrice, _endPrice, _extend, _plotObject);
  connect(d, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  d->setModified(FALSE);
  d->show();
  return 1;
}

