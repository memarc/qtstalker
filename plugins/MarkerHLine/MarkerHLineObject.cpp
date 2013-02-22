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

#include "MarkerHLineObject.h"
#include "Util.h"
#include "MarkerHLineDialog.h"
#include "horizontal.xpm"
#include "../../pics/edit.xpm"
#include "../../pics/delete.xpm"

#include <QDebug>

MarkerHLineObject::MarkerHLineObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("MarkerHLine");
  _type = QString("marker");
  _penWidth = 1;
  _price = 0;
  _color = QColor(Qt::white);
  _selected = 0;
  _modified = FALSE;
  _status = _NONE;
  _handleWidth = 6;
  _readOnly = FALSE;
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

MarkerHLineObject::~MarkerHLineObject ()
{
  save();
}

int
MarkerHLineObject::message (ObjectCommand *oc)
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
MarkerHLineObject::draw (ObjectCommand *oc)
{
  // get painter
  QString key("painter");
  QPainter *p = (QPainter *) oc->getObject(key);
  if (! p)
  {
    qDebug() << "MarkerHLineObject::draw: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerHLineObject::draw: invalid" << key;
    return 0;
  }

  // translate price to y axis
  ObjectCommand toc(QString("convert_to_y"));
  toc.setValue(QString("value"), _price);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerHLineObject::draw: object message error" << toc.command();
    return 0;
  }
  int y = toc.getInt(QString("y"));
  
  QPen pen = p->pen();
  pen.setStyle(Qt::SolidLine);
  pen.setColor(_color);
  pen.setWidth(_penWidth);
  p->setPen(pen);

  // draw the box with value 
  Util util;
  QString ts;
  util.strip(_price, 4, ts);
  QString s = " " + ts; // prepend space so we can clearly read text
  
  QFontMetrics fm(p->font());
  
  QRect rc = p->boundingRect(0, y - (fm.height() / 2), 1, 1, 0, s);
  
  p->fillRect(rc, p->background().color()); // fill in behind text first
  p->drawText(rc, s); // draw text
  p->drawRect(rc); // draw a nice little box around text

  // draw the hline
  QRect rect = p->viewport();
  p->drawLine (rc.width(), y, rect.width(), y);

  // setup the bounding click area
  QPolygon array;
  array << QPoint(0, y - 4);
  array << QPoint(0, y + 4);
  array << QPoint(rect.width(), y + 4);
  array << QPoint(rect.width(), y - 4);
  _selectionArea.clear();
  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    int t = rect.width() / 4;
    for (int loop = 0; loop < 5; loop++)
    {
      _grabHandles.append(QRegion(t * loop,
                                  y - (_handleWidth / 2),
                                  _handleWidth,
                                  _handleWidth,
                                  QRegion::Rectangle));

      p->fillRect(t * loop,
                  y - (_handleWidth / 2),
                  _handleWidth,
                  _handleWidth,
                  _color);
    }
  }
  
  return 1;
}

int
MarkerHLineObject::info (ObjectCommand *oc)
{
  QMouseEvent *event = (QMouseEvent *) oc->getObject(QString("event"));
  if (! event)
  {
    qDebug() << "MarkerHLineObject::info: invalid QMouseEvent";
    return 0;
  }
  
  if (! isSelected(event->pos()))
    return 0;
  
  Data info;
  info.insert(tr("Item"), QVariant(tr("HLine")));
  
  Util strip;
  QString ts;
  strip.strip(_price, 4, ts);
  info.insert(tr("Price"), QVariant(ts));
  oc->setValue(QString("info"), info);

  return 1;
}

int
MarkerHLineObject::highLowRange (ObjectCommand *oc)
{
  // get plot
  QString key("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerHLineObject::highLowRange: invalid" << key;
    return 0;
  }

  // get plot size
  key = QString("size");
  ObjectCommand toc(key);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerHLineObject::highLowRange: message error" << key;
    return 0;
  }
  int size = toc.getInt(key);
  
  toc.setCommand(QString("set_high_low"));
  for (int pos = 0; pos < size; pos++)
  {
    toc.setValue(QString("index"), pos);
    toc.setValue(QString("high"), _price);
    toc.setValue(QString("low"), _price);
    plot->message(&toc);
  }
  
  return 1;
}

int
MarkerHLineObject::mouseMove (ObjectCommand *oc)
{
  // mouse event
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerHLineObject::mouseMove: invalid" << key;
    return 0;
  }

  // get plot
  key = QString("plot");
  Object *plot = (Object *) oc->getObject(key);
  if (! plot)
  {
    qDebug() << "MarkerHLineObject::mouseMove: invalid" << key;
    return 0;
  }
  
  switch (_status)
  {
    case _MOVING:
    {
      // translate y axis to price
      ObjectCommand toc(QString("convert_to_val"));
      toc.setValue(QString("y"), event->pos().y());
      if (! plot->message(&toc))
      {
//        qDebug() << "MarkerHLineObject::mouseMove: object message error" << toc.command;
        return 0;
      }
      _price = toc.getDouble(QString("value"));

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
MarkerHLineObject::mousePress (ObjectCommand *oc)
{
  if (_readOnly)
    return 0;
  
  // mouse event
  QString key("event");
  QMouseEvent *event = (QMouseEvent *) oc->getObject(key);
  if (! event)
  {
    qDebug() << "MarkerHLineObject::mousePress: invalid" << key;
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
MarkerHLineObject::create (ObjectCommand *)
{
  _status = _MOVING;
  _selected = TRUE;
  return 1;
}

int
MarkerHLineObject::isSelected (QPoint p)
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
MarkerHLineObject::isGrabSelected (QPoint p)
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
MarkerHLineObject::load ()
{
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_name, QSettings::NativeFormat);
  
  _color = settings.value(QString("color"), QColor(Qt::red)).value<QColor>();
  _price = settings.value(QString("price"), 0).toDouble();
  
  _modified = FALSE;
  _readOnly = FALSE;
  
  return 1;
}

int
MarkerHLineObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MarkerHLineObject::load: invalid QSettings";
    return 0;
  }
  
  _color = settings->value(QString("color"), QColor(Qt::red)).value<QColor>();
  _price = settings->value(QString("price"), 0).toDouble();
  _plotObject = settings->value(QString("plot"), QString("plot")).toString();
  _readOnly = settings->value(QString("read_only"), TRUE).toBool();
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerHLineObject::save ()
{
  if (! _modified)
    return 0;
  
  if (_readOnly)
    return 0;
  
  if (_name.isEmpty())
    return 0;
  
  QSettings settings(_name, QSettings::NativeFormat);
  
  settings.setValue(QString("color"), _color);
  settings.setValue(QString("price"), _price);
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerHLineObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "MarkerHLineObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("color"), _color);
  settings->setValue(QString("price"), _price);
  settings->setValue(QString("plot"), _plotObject);
  settings->setValue(QString("read_only"), _readOnly);
  
  _modified = FALSE;
  
  return 1;
}

int
MarkerHLineObject::icon (ObjectCommand *oc)
{
  oc->setValue(QString("icon"), QIcon(horizontal_xpm));
  return 1;
}

int
MarkerHLineObject::update (ObjectCommand *oc)
{
  Object *plot = (Object *) oc->getObject(_plotObject);
  if (! plot)
  {
    qDebug() << "MarkerHLineObject::update: invalid plot" << _plotObject;
    return 0;
  }
  
  // add to plot
  ObjectCommand toc(QString("add_objects"));
  toc.setValue(_name, (void *) this);
  if (! plot->message(&toc))
  {
    qDebug() << "MarkerHLineObject::update: message error" << plot->plugin() << toc.command();
    return 0;
  }
  
  return 1;
}

int
MarkerHLineObject::copy (ObjectCommand *oc)
{
  QString key("input");
  Object *input = (Object *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "MarkerHLineObject::copy: invalid" << key;
    return 0;
  }
  
  if (input->plugin() != _plugin)
  {
    qDebug() << "MarkerHLineObject::copy: invalid" << key;
    return 0;
  }

  key = QString("settings");
  ObjectCommand toc(key);
  if (! input->message(&toc))
  {
    qDebug() << "MarkerHLineObject::copy: invalid" << input->plugin() << key;
    return 0;
  }
  
  _price = toc.getDouble(QString("price"));
  _color = toc.getColor(QString("color"));
  _plotObject = toc.getString(QString("plot"));
  _readOnly = toc.getBool(QString("read_only"));

  return 1;
}

int
MarkerHLineObject::settings (ObjectCommand *oc)
{
  oc->setValue(QString("color"), _color);
  oc->setValue(QString("price"), _price);
  oc->setValue(QString("plot"), _plotObject);
  oc->setValue(QString("read_only"), _readOnly);
  return 1;
}

int
MarkerHLineObject::dialog (ObjectCommand *oc)
{
  MarkerHLineDialog *d = new MarkerHLineDialog(oc->getObjects(), _name);
  d->setSettings(_color, _price, _plotObject);
  connect(d, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  d->setModified(FALSE);
  d->show();
  return 1;
}

void
MarkerHLineObject::dialogDone (void *dialog)
{
  MarkerHLineDialog *d = (MarkerHLineDialog *) dialog;
  d->settings(_color, _price, _plotObject);
  _modified = TRUE;
  sendDrawMessage();
}

void
MarkerHLineObject::sendDrawMessage ()
{
  ObjectCommand oc(QString("draw"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerHLineObject::sendSelectedMessage ()
{
  ObjectCommand oc(QString("selected"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerHLineObject::sendUnselectedMessage ()
{
  ObjectCommand oc(QString("unselected"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerHLineObject::sendMovingMessage ()
{
  ObjectCommand oc(QString("moving"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
MarkerHLineObject::sendDeleteMessage ()
{
  QDir dir;
  dir.remove(_name);
  _modified = FALSE;

  ObjectCommand oc(QString("delete"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}
