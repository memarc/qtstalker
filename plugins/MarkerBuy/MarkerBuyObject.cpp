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

#include "MarkerBuyObject.h"
#include "Util.h"
#include "MarkerBuyDialog.h"
#include "buyarrow.xpm"
#include "../../pics/edit.xpm"
#include "../../pics/delete.xpm"

#include <QDebug>

MarkerBuyObject::MarkerBuyObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("MarkerBuy");
  _type = QString("marker");
  _penWidth = 1;
  _date = QDateTime::currentDateTime();
  _price = 0;
  _color = QColor(Qt::green);
  _selected = 0;
  _modified = FALSE;
  _status = _NONE;
  _handleWidth = 6;
  _readOnly = FALSE;
  _index = -1;
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

MarkerBuyObject::~MarkerBuyObject ()
{
  save();
}

int
MarkerBuyObject::message (ObjectCommand *oc)
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
MarkerBuyObject::draw (ObjectCommand *oc)
{
  // get painter
  QString key("painter");
  QPainter *p = (QPainter *) oc->getObject(key);
  if (! p)
  {
    qDebug() << "MarkerBuyObject::draw: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerBuyObject::draw: invalid" << key;
    return 0;
  }

  int start = oc->getInt(QString("index"));
  int end = oc->getInt(QString("index_end"));

  // update index
  if (_index == -1)
  {
    ObjectCommand toc(QString("date_to_index"));
    toc.setValue(QString("date"), _date);
    if (! plot->message(&toc))
    {
      qDebug() << "MarkerBuyObject::draw: object message error" << toc.command();
      return 0;
    }
    _index = toc.getInt(QString("index"));
  }
  
  // are we visible?
  if (_index == -1 || _index < start || _index > end)
    return 0;

  // translate index to x coord
  ObjectCommand toc(QString("index_to_x"));
  toc.setValue(QString("index"), _index);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerBuyObject::draw: object message error" << toc.command();
    return 0;
  }
  int x = toc.getInt(QString("x"));

  // translate price to y axis
  toc.setCommand(QString("convert_to_y"));
  toc.setValue(QString("value"), _price);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerBuyObject::draw: object message error" << toc.command();
    return 0;
  }
  int y = toc.getInt(QString("y"));

  QPen pen = p->pen();
  pen.setStyle(Qt::NoPen);
  p->setPen(pen);
  
  p->setBrush(_color);

  QPolygon arrow;
  arrow << QPoint(x, y);
  arrow << QPoint(x + 5, y + 5);
  arrow << QPoint(x + 2, y + 5);
  arrow << QPoint(x + 2, y + 11);
  arrow << QPoint(x - 2, y + 11);
  arrow << QPoint(x - 2, y + 5);
  arrow << QPoint(x - 5, y + 5);

  p->drawPolygon(arrow, Qt::OddEvenFill);

  _selectionArea.clear();

  _selectionArea.append(QRegion(arrow));

  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(x - (_handleWidth / 2),
                        y - _handleWidth,
                        _handleWidth,
                        _handleWidth,
                        QRegion::Rectangle));

    p->fillRect(x - (_handleWidth / 2),
                y - _handleWidth,
                _handleWidth,
                _handleWidth,
                _color);
  }
  
  return 1;
}

int
MarkerBuyObject::info (ObjectCommand *oc)
{
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerBuyObject::info: invalid" << key;
    return 0;
  }
  
  if (! isSelected(event->pos()))
    return 0;
  
  Data info;
  info.insert(tr("Item"), QVariant(tr("Buy")));
  info.insert(tr("D"), QVariant(_date.toString("yyyy-MM-dd")));
  info.insert(tr("T"), QVariant(_date.toString("HH:mm:ss")));
  
  Util strip;
  QString ts;
  strip.strip(_price, 4, ts);
  info.insert(tr("Price"), QVariant(ts));
  
  oc->setValue(QString("info"), info);

  return 1;
}

int
MarkerBuyObject::highLowRange (ObjectCommand *oc)
{
  // get plot
  QString key("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerBuyObject::highLowRange: invalid" << key;
    return 0;
  }

  ObjectCommand toc(QString("date_to_index"));
  toc.setValue(QString("date"), _date);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerBuyObject::highLowRange: object message error" << toc.command();
    return 0;
  }
  int index = toc.getInt(QString("index"));
  
  toc.setCommand(QString("set_high_low"));
  toc.setValue(QString("index"), index);
  toc.setValue(QString("high"), _price);
  toc.setValue(QString("low"), _price);
  plot->message(&toc);
  
  return 1;
}

int
MarkerBuyObject::mouseMove (ObjectCommand *oc)
{
  // mouse event
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerBuyObject::mouseMove: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerBuyObject::mouseMove: invalid" << key;
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
      if (index == -1)
	return 1;
      
      // translate y axis to price
      toc.setCommand(QString("convert_to_val"));
      toc.setValue(QString("y"), event->pos().y());
      if (! plot->message(&toc))
        return 0;
      double v = toc.getDouble(QString("value"));

      _index = index;
      _price = v;
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
MarkerBuyObject::mousePress (ObjectCommand *oc)
{
  if (_readOnly)
    return 0;
  
  // mouse event
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerBuyObject::mousePress: invalid" << key;
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
    qDebug() << "MarkerBuyObject::mousePress: invalid" << key;
    return 0;
  }
  
  switch (_status)
  {
    case _SELECTED:
    {
      if (isGrabSelected(event->pos()))
      {
        _status = _MOVING;
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

      // date
      toc.setCommand(QString("index_to_date"));
      toc.setValue(QString("index"), index);
      if (! plot->message(&toc))
        return 0;
      QDateTime dt = toc.getDate(QString("date"));
      
      // price
      toc.setCommand(QString("convert_to_val"));
      toc.setValue(QString("y"), event->pos().y());
      if (! plot->message(&toc))
        return 0;
      double v = toc.getDouble(QString("value"));
      
      _index = index;
      _date = dt;
      _price = v;
      
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
MarkerBuyObject::create (ObjectCommand *)
{
  _status = _MOVING;
  _selected = TRUE;
  _modified = TRUE;
  _readOnly = FALSE;
  return 1;
}

int
MarkerBuyObject::isSelected (QPoint p)
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
MarkerBuyObject::isGrabSelected (QPoint p)
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
MarkerBuyObject::load ()
{
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_name, QSettings::NativeFormat);
  
  _color = settings.value(QString("color"), QColor(Qt::green)).value<QColor>();
  _date = settings.value(QString("date"), QDateTime::currentDateTime()).toDateTime();
  _price = settings.value(QString("price"), 0).toDouble();
  
  _index = -1;
  _modified = FALSE;
  _readOnly = FALSE;
  
  return 1;
}

int
MarkerBuyObject::save ()
{
  if (! _modified)
    return 0;
  
  if (_readOnly)
    return 0;
  
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_name, QSettings::NativeFormat);
  
  settings.setValue(QString("color"), _color);
  settings.setValue(QString("date"), _date);
  settings.setValue(QString("price"), _price);
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerBuyObject::icon (ObjectCommand *oc)
{
  oc->setValue(QString("icon"), QIcon(buyarrow_xpm));
  return 1;
}

void
MarkerBuyObject::dialogDone (void *dialog)
{
  MarkerBuyDialog *d = (MarkerBuyDialog *) dialog;
  d->settings(_color, _date, _price, _plotObject);

  _index = -1;
  _modified = TRUE;
  
  sendDrawMessage();
}

void
MarkerBuyObject::sendDrawMessage ()
{
  ObjectCommand oc(QString("draw"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerBuyObject::sendSelectedMessage ()
{
  ObjectCommand oc(QString("selected"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerBuyObject::sendUnselectedMessage ()
{
  ObjectCommand oc(QString("unselected"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerBuyObject::sendMovingMessage ()
{
  ObjectCommand oc(QString("moving"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerBuyObject::sendDeleteMessage ()
{
  QDir dir;
  dir.remove(_name);
  _modified = FALSE;
  
  ObjectCommand oc(QString("delete"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

int
MarkerBuyObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MarkerBuyObject::load: invalid QSettings";
    return 0;
  }
  
  _color = settings->value(QString("color"), QColor(Qt::red)).value<QColor>();
  _price = settings->value(QString("price"), 0).toDouble();
  _date = settings->value(QString("date"), QDateTime::currentDateTime()).toDateTime();
  _plotObject = settings->value(QString("plot"), QString("plot")).toString();
  _readOnly = settings->value(QString("read_only"), TRUE).toBool();
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerBuyObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MarkerBuyObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("color"), _color);
  settings->setValue(QString("price"), _price);
  settings->setValue(QString("date"), _date);
  settings->setValue(QString("plot"), _plotObject);
  settings->setValue(QString("read_only"), _readOnly);
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerBuyObject::update (ObjectCommand *oc)
{
  Object *plot = (Object *) oc->getObject(_plotObject);
  if (! plot)
  {
    qDebug() << "MarkerBuyObject::update: invalid plot" << _plotObject;
    return 0;
  }
  
  // add to plot
  ObjectCommand toc(QString("add_objects"));
  toc.setValue(_name, (void *) this);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerBuyObject::update: message error" << plot->plugin() << toc.command();
    return 0;
  }
  
  return 1;
}

int
MarkerBuyObject::copy (ObjectCommand *oc)
{
  QString key("input");
  Object *input = (Object *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "MarkerBuyObject::copy: invalid" << key;
    return 0;
  }
  
  if (input->plugin() != _plugin)
  {
    qDebug() << "MarkerBuyObject::copy: invalid" << key;
    return 0;
  }

  key = QString("settings");
  ObjectCommand toc(key);
  if (! input->message(&toc))
  {
    qDebug() << "MarkerBuyObject::copy: invalid" << input->plugin() << key;
    return 0;
  }
  
  _price = toc.getDouble(QString("price"));
  _color = toc.getColor(QString("color"));
  _plotObject = toc.getString(QString("plot"));
  _date = toc.getDate(QString("date"));
  _readOnly = toc.getBool(QString("read_only"));

  return 1;
}

int
MarkerBuyObject::settings (ObjectCommand *oc)
{
  oc->setValue(QString("color"), _color);
  oc->setValue(QString("price"), _price);
  oc->setValue(QString("plot"), _plotObject);
  oc->setValue(QString("date"), _date);
  oc->setValue(QString("read_only"), _readOnly);
  return 1;
}

int
MarkerBuyObject::dialog (ObjectCommand *oc)
{
  MarkerBuyDialog *d = new MarkerBuyDialog(oc->getObjects(), _name);
  d->setSettings(_color, _date, _price, _plotObject);
  connect(d, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  d->setModified(FALSE);
  d->show();
  return 1;
}

