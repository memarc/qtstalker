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

#include "MarkerTextObject.h"
#include "Util.h"
#include "MarkerTextDialog.h"
#include "text.xpm"
#include "../../pics/edit.xpm"
#include "../../pics/delete.xpm"

#include <QDebug>

MarkerTextObject::MarkerTextObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("MarkerText");
  _type = QString("marker");
  _penWidth = 1;
  _date = QDateTime::currentDateTime();
  _price = 0;
  _color = QColor(Qt::red);
  _text = tr("Text");
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

MarkerTextObject::~MarkerTextObject ()
{
  save();
}

int
MarkerTextObject::message (ObjectCommand *oc)
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
MarkerTextObject::draw (ObjectCommand *oc)
{
  // get painter
  QString key("painter");
  QPainter *p = (QPainter *) oc->getObject(key);
  if (! p)
  {
    qDebug() << "MarkerTextObject::draw: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerTextObject::draw: invalid" << key;
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
      qDebug() << "MarkerTextObject::draw: object message error" << toc.command();
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
    qDebug() << "MarkerTextObject::draw: object message error" << toc.command();
    return 0;
  }
  int x = toc.getInt(QString("x"));

  // translate price to y axis
  toc.setCommand(QString("convert_to_y"));
  toc.setValue(QString("value"), _price);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerTextObject::draw: object message error" << toc.command();
    return 0;
  }
  int y = toc.getInt(QString("y"));

  p->save();
  
  QPen pen = p->pen();
  pen.setColor(_color);
  p->setPen(pen);
  p->setFont(_font);

  p->drawText(x, y, _text);

  QFontMetrics fm(_font);

  _selectionArea.clear();

  _selectionArea.append(QRegion(x,
                                y - fm.height(),
                                fm.width(_text, -1),
                                fm.height(),
                                QRegion::Rectangle));

  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(x - _handleWidth - 1,
                                y - (fm.height() / 2),
                                _handleWidth,
                                _handleWidth,
                                QRegion::Rectangle));

    p->fillRect(x - _handleWidth - 1,
                y - (fm.height() / 2),
                _handleWidth,
                _handleWidth,
                _color);
  }
  
  p->restore();
  
  return 1;
}

int
MarkerTextObject::info (ObjectCommand *oc)
{
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerTextObject::info: invalid" << key;
    return 0;
  }
  
  if (! isSelected(event->pos()))
    return 0;

  Data info;
  info.insert(tr("Item"), QVariant(tr("Text")));
  info.insert(tr("D"), QVariant(_date.toString("yyyy-MM-dd")));
  info.insert(tr("T"), QVariant(_date.toString("HH:mm:ss")));
  
  Util strip;
  QString ts;
  strip.strip(_price, 4, ts);
  info.insert(tr("Price"), QVariant(ts));
  
  info.insert(tr("Text"), QVariant(_text));
  
  oc->setValue(QString("info"), info);

  return 1;
}

int
MarkerTextObject::highLowRange (ObjectCommand *oc)
{
  // get plot
  QString key("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerTextObject::highLowRange: invalid" << key;
    return 0;
  }

  ObjectCommand toc(QString("date_to_index"));
  toc.setValue(QString("date"), _date);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerTextObject::highLowRange: object message error" << toc.command();
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
MarkerTextObject::mouseMove (ObjectCommand *oc)
{
  // mouse event
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerTextObject::mouseMove: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerTextObject::mouseMove: invalid" << key;
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
MarkerTextObject::mousePress (ObjectCommand *oc)
{
  if (_readOnly)
    return 0;
  
  // mouse event
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerTextObject::mousePress: invalid" << key;
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
    qDebug() << "MarkerTextObject::mousePress: invalid" << key;
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
MarkerTextObject::create (ObjectCommand *)
{
  _status = _MOVING;
  _selected = TRUE;
  return 1;
}

int
MarkerTextObject::isSelected (QPoint p)
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
MarkerTextObject::isGrabSelected (QPoint p)
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
MarkerTextObject::load ()
{
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_name, QSettings::NativeFormat);
  
  _color = settings.value(QString("color"), QColor(Qt::red)).value<QColor>();
  _date = settings.value(QString("date"), QDateTime::currentDateTime()).toDateTime();
  _price = settings.value(QString("price"), 0).toDouble();
  _text = settings.value(QString("text"), tr("Text")).toString();
  _font = settings.value(QString("font"), QFont()).value<QFont>();
  
  _index = -1;
  _modified = FALSE;
  _readOnly = FALSE;
  
  return 1;
}

int
MarkerTextObject::icon (ObjectCommand *oc)
{
  oc->setValue(QString("icon"), QIcon(text_xpm));
  return 1;
}

int
MarkerTextObject::save ()
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
  settings.setValue(QString("text"), _text);
  settings.setValue(QString("font"), _font);
  
  _modified = FALSE;
  
  return 1;
}

void
MarkerTextObject::dialogDone (void *dialog)
{
  MarkerTextDialog *d = (MarkerTextDialog *) dialog;
  d->settings(_color, _date, _price, _text, _font, _plotObject);

  _index = -1;
  _modified = TRUE;
  
  sendDrawMessage();
}

void
MarkerTextObject::sendDrawMessage ()
{
  ObjectCommand oc(QString("draw"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerTextObject::sendSelectedMessage ()
{
  ObjectCommand oc(QString("selected"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerTextObject::sendUnselectedMessage ()
{
  ObjectCommand oc(QString("unselected"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerTextObject::sendMovingMessage ()
{
  ObjectCommand oc(QString("moving"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerTextObject::sendDeleteMessage ()
{
  QDir dir;
  dir.remove(_name);
  _modified = FALSE;
  
  ObjectCommand oc(QString("delete"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

int
MarkerTextObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MarkerTextObject::load: invalid QSettings";
    return 0;
  }
  
  _color = settings->value(QString("color"), QColor(Qt::red)).value<QColor>();
  _price = settings->value(QString("price"), 0).toDouble();
  _date = settings->value(QString("date"), QDateTime::currentDateTime()).toDateTime();
  _text = settings->value(QString("text"), QString("Text")).toString();
  _font = settings->value(QString("font"), QFont()).value<QFont>();
  _plotObject = settings->value(QString("plot"), QString("plot")).toString();
  _readOnly = settings->value(QString("read_only"), TRUE).toBool();
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerTextObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MarkerTextObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("color"), _color);
  settings->setValue(QString("price"), _price);
  settings->setValue(QString("date"), _date);
  settings->setValue(QString("text"), _text);
  settings->setValue(QString("font"), _font);
  settings->setValue(QString("plot"), _plotObject);
  settings->setValue(QString("read_only"), _readOnly);
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerTextObject::update (ObjectCommand *oc)
{
  Object *plot = (Object *) oc->getObject(_plotObject);
  if (! plot)
  {
    qDebug() << "MarkerTextObject::update: invalid plot" << _plotObject;
    return 0;
  }
  
  // add to plot
  ObjectCommand toc(QString("add_objects"));
  toc.setValue(_name, (void *) this);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerTextObject::update: message error" << plot->plugin() << toc.command();
    return 0;
  }
  
  return 1;
}

int
MarkerTextObject::copy (ObjectCommand *oc)
{
  QString key("input");
  Object *input = (Object *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "MarkerTextObject::copy: invalid" << key;
    return 0;
  }
  
  if (input->plugin() != _plugin)
  {
    qDebug() << "MarkerTextObject::copy: invalid" << key;
    return 0;
  }

  key = QString("settings");
  ObjectCommand toc(key);
  if (! input->message(&toc))
  {
    qDebug() << "MarkerTextObject::copy: invalid" << input->plugin() << key;
    return 0;
  }
  
  _color = toc.getColor(QString("color"));
  _price = toc.getDouble(QString("price"));
  _date = toc.getDate(QString("date"));
  _text = toc.getString(QString("text"));
  _font = toc.getFont(QString("font"));
  _plotObject = toc.getString(QString("plot"));
  _readOnly = toc.getBool(QString("read_only"));

  return 1;
}

int
MarkerTextObject::settings (ObjectCommand *oc)
{
  oc->setValue(QString("color"), _color);
  oc->setValue(QString("price"), _price);
  oc->setValue(QString("date"), _date);
  oc->setValue(QString("text"), _text);
  oc->setValue(QString("font"), _font);
  oc->setValue(QString("plot"), _plotObject);
  oc->setValue(QString("read_only"), _readOnly);
  return 1;
}

int
MarkerTextObject::dialog (ObjectCommand *oc)
{
  MarkerTextDialog *d = new MarkerTextDialog(oc->getObjects(), _name);
  d->setSettings(_color, _date, _price, _text, _font, _plotObject);
  connect(d, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  d->setModified(FALSE);
  d->show();
  return 1;
}

