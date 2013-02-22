/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "PlotWidgetArea.h"
#include "Util.h"
#include "ObjectCommand.h"
#include "PlotWidgetStatus.h"
#include "../../pics/delete.xpm"
#include "../../pics/edit.xpm"
#include "../../pics/grid.xpm"
#include "../../pics/date.xpm"
#include "../../pics/about.xpm"
#include "../../pics/datawindow.xpm"

#include <QDebug>


PlotWidgetArea::PlotWidgetArea (QWidget *w, XMap *xmap, YMap *ymap, PlotWidgetScale *ps,
				Object *plotObject) : QWidget(w)
{
  _xmap = xmap;
  _ymap = ymap;
  _itemSelected = 0;
  _backgroundColor = QColor(Qt::black);
  _scalerFlag = FALSE;
  _plotScale = ps;
  _plotObject = plotObject;
  _orderPos = 0;

  QFont font;
  font.setFamily("Helvetica");
  font.setPointSize(10);
  font.setWeight(50);
  setFont(font);
  
  _plotGrid = new PlotWidgetGrid(this, _xmap, _ymap);
  
  _plotCursorInfo = new PlotWidgetCursorInfo(this, _xmap);
  connect(_plotCursorInfo, SIGNAL(signalDraw()), this, SLOT(update()));

  createMenu();
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu()));
  
  setMouseTracking(TRUE);
  
  setAttribute(Qt::WA_NoSystemBackground, TRUE); // disable background fill
  
  setMinimumHeight(1);
  setMinimumWidth(1);
}

PlotWidgetArea::~PlotWidgetArea ()
{
  clear();
}

void
PlotWidgetArea::clear ()
{
  qDeleteAll(_items);
  _items.clear();

  _itemSelected = 0;
  
  _plotCursorInfo->clear();
  
  _orderPos = 0;
  _plotOrder.clear();
  _orderLookup.clear();
}

PlotWidgetCursorInfo *
PlotWidgetArea::cursorInfo ()
{
  return _plotCursorInfo;
}

void
PlotWidgetArea::draw ()
{
  _pic.fill(_backgroundColor);
  if (! isVisible())
    return;

  if (_scalerFlag)
  {
    updateHighLowRange();
    _scalerFlag = FALSE;
  }

  _ymap->update(_xmap, _pic.rect());

  QPainter p(&_pic);
  p.setFont(font());  
//  p.setRenderHint(QPainter::TextAntialiasing, TRUE);
  p.setBackground(QBrush(_backgroundColor));

  _plotGrid->draw(&p);
  
  drawItems(&p);
  
  setYPoints();

//  drawCrossHair();
  
  update();
}

void
PlotWidgetArea::setBackgroundColor (QColor d)
{
  _backgroundColor = d;
}

QColor
PlotWidgetArea::backgroundColor ()
{
  return _backgroundColor;
}

void
PlotWidgetArea::setName (QString d)
{
  _name = d;
}

QString
PlotWidgetArea::name ()
{
  return _name;
}

void
PlotWidgetArea::setSymbol (QString d)
{
  _symbol = d;
}

QString
PlotWidgetArea::symbol ()
{
  return _symbol;
}

void
PlotWidgetArea::setScalerFlag ()
{
  _scalerFlag = TRUE;
}

void
PlotWidgetArea::updateHighLowRange ()
{
  _ymap->clear();

  ObjectCommand oc(QString("high_low_range"));
  oc.setValue(QString("plot"), (void *) _plotObject);

  QHashIterator<QString, Object *> it(_items);
  while (it.hasNext())
  {
    it.next();
    Object *item = it.value();
    item->message(&oc);
  }
}

int
PlotWidgetArea::startPosition ()
{
  return _xmap->size() - (width() / _xmap->spacing());
}

void
PlotWidgetArea::setYPoints ()
{
  _plotScale->clearPoints();

  Util util;
  QString err;
  
  ObjectCommand oc(QString("start_end_index"));
  QHashIterator<QString, Object *> it(_items);
  while (it.hasNext())
  {
    it.next();
    Object *item = it.value();

    if (! item->message(&oc))
      continue; // not a curve
    int max = oc.getInt(QString("end"));

    int index = _xmap->endPageIndex(_pic.width());
    if (index > max)
      index = max;

    oc.setCommand(QString("scale_point"));
    oc.setValue(QString("index"), index);
    if (! item->message(&oc))
      continue;
    
    _plotScale->addPoint(oc.getColor(QString("color")), oc.getDouble(QString("value")));
  }
}

// ******************************************
//            EVENT STUFF
// ******************************************

void
PlotWidgetArea::paintEvent (QPaintEvent *event)
{
  QPainter *p = new QPainter(this);
  p->setFont(font());  
//  p->setRenderHint(QPainter::TextAntialiasing, TRUE);
  p->setBackground(QBrush(_backgroundColor));
  
  QRect rect = event->rect();
  p->drawPixmap(rect, _pic, rect);
  
  // draw selected marker after pixmap update
  if (_itemSelected)
  {
    int index = _xmap->index();
    int spacing = _xmap->spacing();
    int epi = _xmap->endPageIndex(_pic.width());
    
    ObjectCommand oc(QString("draw"));
    oc.setValue(QString("painter"), (void *) p);
    oc.setValue(QString("plot"), (void *) _plotObject);
    oc.setValue(QString("index"), index);
    oc.setValue(QString("index_end"), epi);
    oc.setValue(QString("spacing"), spacing);
    if (! _itemSelected->message(&oc))
    {
//      qDebug() << "PlotWidgetArea::paintEvent: Scaler object message error" << oc.command;
    }
  }

  // draw cursor info
  _plotCursorInfo->draw(p);
  
  delete p;
}

void
PlotWidgetArea::resizeEvent (QResizeEvent *event)
{
  _pic = QPixmap(event->size());
  draw();
  QWidget::resizeEvent(event);
}

void
PlotWidgetArea::mousePressEvent (QMouseEvent *event)
{
  ObjectCommand toc(QString("mouse_press"));
  toc.setValue(QString("event"), (void *) event);
  toc.setValue(QString("plot"), (void *) _plotObject);
  
  if (_itemSelected)
  {
    _itemSelected->message(&toc);
    return;
  }

  emit signalSendMessage(&toc);
}

void
PlotWidgetArea::mouseMoveEvent (QMouseEvent *event)
{
  if (_itemSelected)
  {
    ObjectCommand toc(QString("mouse_move"));
    toc.setValue(QString("event"), (void *) event);
    toc.setValue(QString("plot"), (void *) _plotObject);
    _itemSelected->message(&toc);
  }
  
  _plotCursorInfo->setInfo(event, _items, _name);
}

/*
void
PlotWidgetArea::mouseDoubleClickEvent (QMouseEvent *)
{
}
*/

// ******************************************
//            INFO STUFF
// ******************************************

void
PlotWidgetArea::setInfo (bool d)
{
  _plotCursorInfo->setEnabled(d);
  
  _actionInfo->blockSignals(TRUE);
  _actionInfo->setChecked(d);
  _actionInfo->blockSignals(FALSE);
}

bool
PlotWidgetArea::info ()
{
  return _plotCursorInfo->enabled();
}

void
PlotWidgetArea::infoToggled (bool d)
{
  _plotCursorInfo->setEnabled(d);
  draw();
  emit signalSettingsChanged();
}

// ******************************************
//            GRID STUFF
// ******************************************

void
PlotWidgetArea::setXGrid (bool d)
{
  _plotGrid->enableXGrid(d);
  
  _actionXGrid->blockSignals(TRUE);
  _actionXGrid->setChecked(d);
  _actionXGrid->blockSignals(FALSE);
}

bool
PlotWidgetArea::xGrid ()
{
  return _plotGrid->xGrid();
}

void
PlotWidgetArea::xGridToggled (bool d)
{
  _plotGrid->enableXGrid(d);
  draw();
  emit signalSettingsChanged();
}

void
PlotWidgetArea::setYGrid (bool d)
{
  _plotGrid->enableYGrid(d);
  
  _actionYGrid->blockSignals(TRUE);
  _actionYGrid->setChecked(d);
  _actionYGrid->blockSignals(FALSE);
}

bool
PlotWidgetArea::yGrid ()
{
  return _plotGrid->yGrid();
}

void
PlotWidgetArea::yGridToggled (bool d)
{
  _plotGrid->enableYGrid(d);
  draw();
  emit signalSettingsChanged();
}

// ******************************************
//           DATE STUFF
// ******************************************

bool
PlotWidgetArea::date ()
{
  return _actionDate->isChecked();
}

void
PlotWidgetArea::dateToggled (bool d)
{
  emit signalDateChanged(d);
  emit signalSettingsChanged();
}

void
PlotWidgetArea::setDate (bool d)
{
  _actionDate->blockSignals(TRUE);
  _actionDate->setChecked(d);
  _actionDate->blockSignals(FALSE);
}

// ***********************************************************
//                ITEM STUFF
// ***********************************************************

int
PlotWidgetArea::addObject (QString name, Object *item)
{
  Object *o = _items.value(name);
  if (o)
  {
    delete o;
    _items.insert(name, item);
    _plotOrder.insert(_orderLookup.value(name), item);
  }
  else
  {
    _items.insert(name, item);
    _orderLookup.insert(name, _orderPos);
    _plotOrder.insert(_orderPos++, item);
  }

  connect(item, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(objectMessage(ObjectCommand)));
  connect(this, SIGNAL(signalSendMessage(ObjectCommand *)), item, SLOT(message(ObjectCommand *)));

  ObjectCommand oc(QString("high_low_range"));
  oc.setValue(QString("plot"), (void *) _plotObject);
  item->message(&oc);
  
  setScalerFlag();
  
  return 1;
}

QHash<QString, Object *>
PlotWidgetArea::items ()
{
  return _items;
}

int
PlotWidgetArea::removeItem (QStringList l)
{
  for (int pos = 0; pos < l.size(); pos++)
  {
    Object *item = _items.value(l.at(pos));
    if (! item)
      continue;
  
    if (item == _itemSelected)
      _itemSelected = 0;

    delete item;
    _items.remove(l.at(pos));

    int tpos = _orderLookup.value(l.at(pos));
    _plotOrder.remove(tpos);
    _orderLookup.remove(l.at(pos));
  
    setScalerFlag();
  }
  
  emit signalMarkerDelete(l);
  
  draw();
  
  return 1;
}

void
PlotWidgetArea::drawItems (QPainter *p)
{
  int index = _xmap->index();
  int spacing = _xmap->spacing();
  int epi = _xmap->endPageIndex(_pic.width());
  
  ObjectCommand oc(QString("draw"));
  oc.setValue(QString("painter"), (void *) p);
  oc.setValue(QString("plot"), (void *) _plotObject);
  oc.setValue(QString("index"), index);
  oc.setValue(QString("index_end"), epi);
  oc.setValue(QString("spacing"), spacing);
  
  QMapIterator<int, Object *> it(_plotOrder);
  while (it.hasNext())
  {
    it.next();
    Object *item = it.value();
    
    if (item == _itemSelected)
      continue;

    if (! item->message(&oc))
    {
//      qDebug() << "PlotWidgetArea::drawItems: object message error" << it.key() << oc.command;
    }
  }
}

int
PlotWidgetArea::newItem (Object *d)
{
  if (_itemSelected)
  {
    ObjectCommand toc(QString("set_selected"));
    toc.setValue(QString("selected"), FALSE);
    if (! _itemSelected->message(&toc))
    {
      qDebug() << "PlotWidgetArea::newItem: object message error" << toc.command();
      return 0;
    }
    _itemSelected = 0;
  }

  _itemSelected = d;
  connect(_itemSelected, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(objectMessage(ObjectCommand)));
  connect(this, SIGNAL(signalSendMessage(ObjectCommand *)), _itemSelected, SLOT(message(ObjectCommand *)));

  // object creation function if any
  ObjectCommand toc(QString("create"));
  if (! _itemSelected->message(&toc))
  {
    delete _itemSelected;
    _itemSelected = 0;
    return 0;
  }
  
  setScalerFlag();

  _items.insert(_itemSelected->name(), _itemSelected);
  _orderLookup.insert(_itemSelected->name(), _orderPos);
  _plotOrder.insert(_orderPos++, _itemSelected);
  
  return 1;
}

void
PlotWidgetArea::itemSelected (QString name)
{
  if (! _items.contains(name))
    return;
  
  _itemSelected = _items.value(name);
  
  draw();
}

void
PlotWidgetArea::itemUnselected ()
{
  if (! _itemSelected)
    return;
  _itemSelected = 0;
  
  draw();
}

void
PlotWidgetArea::deleteAllMarkers ()
{
  // get list of markers before we delete them
  QStringList tl;
  QHashIterator<QString, Object *> it(_items);
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();
    if (o->type() == QString("marker"))
      tl << it.key();
  }
  
  if (! tl.size())
    return;
  
  removeItem(tl);
}

void
PlotWidgetArea::objectMessage (ObjectCommand oc)
{
  QStringList cl;
  cl << QString("selected") << QString("unselected") << QString("moving") << QString("draw") << QString("delete");
  
  switch (cl.indexOf(oc.command()))
  {
    case 0:
      itemSelected(oc.getString(QString("name")));
      break;
    case 1:
      itemUnselected();
      break;
    case 2:
      setScalerFlag();
      break;
    case 3:
      update();
      break;
    case 4:
      removeItem(QStringList() << oc.getString(QString("name")));
      break;
    default:
      break;
  }
}

// *****************************************************************
//                      MENU STUFF
// *****************************************************************

void
PlotWidgetArea::createMenu ()
{
  _menu = new QMenu(this);

  _markerMenu = new QMenu(_menu);
  _markerMenu->setTitle(tr("New Marker"));
  connect(_markerMenu, SIGNAL(triggered(QAction *)), this, SLOT(menuSelected(QAction *)));
  _menu->addMenu(_markerMenu);
  
  Util util;
  QStringList tl = util.plugins(QString("marker"));
  
  for (int pos = 0; pos < tl.size(); pos++)
  {
    Object *o = util.object(tl.at(pos), QString(), QString());
    if (! o)
      continue;

    QAction *a = _markerMenu->addAction(tl.at(pos));
    QString ts = tr("Create") + " " + tl.at(pos);
    a->setToolTip(ts);
    a->setStatusTip(ts);
    a->setData(QVariant(tl.at(pos)));

    QString key("icon");
    ObjectCommand toc(key);
    if (o->message(&toc))
      a->setIcon(toc.getIcon(key));

    delete o;
  }

  // delete all markers
  QAction *a = _menu->addAction(tr("&Delete All Markers"));
  a->setToolTip(tr("Delete All Markers"));
  a->setStatusTip(tr("Delete All Markers"));
  a->setIcon(QIcon(delete_xpm));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(deleteAllMarkers()));

  _menu->addSeparator();
  
  // show values
  a = _menu->addAction(tr("Show &Values"));
  a->setToolTip(tr("Show Values"));
  a->setStatusTip(tr("Show Values"));
  a->setIcon(QIcon(datawindow_xpm));
  connect(a, SIGNAL(triggered(bool)), this, SIGNAL(signalDataWindow()));
  
  _menu->addSeparator();
  
  // x grid
  _actionXGrid = _menu->addAction(tr("&X Grid"));
  _actionXGrid->setToolTip(tr("Toggle X Axis Grid"));
  _actionXGrid->setStatusTip(tr("Toggle X Axis Grid"));
  _actionXGrid->setCheckable(TRUE);
  _actionXGrid->setChecked(TRUE);
  _actionXGrid->setIcon(QIcon(grid_xpm));
  connect(_actionXGrid, SIGNAL(triggered(bool)), this, SLOT(xGridToggled(bool)));
  
  // y grid
  _actionYGrid = _menu->addAction(tr("&Y Grid"));
  _actionYGrid->setToolTip(tr("Toggle Y Axis Grid"));
  _actionYGrid->setStatusTip(tr("Toggle Y Axis Grid"));
  _actionYGrid->setCheckable(TRUE);
  _actionYGrid->setChecked(TRUE);
  _actionYGrid->setIcon(QIcon(grid_xpm));
  connect(_actionYGrid, SIGNAL(triggered(bool)), this, SLOT(yGridToggled(bool)));

  _menu->addSeparator();
  
  // info
  _actionInfo = _menu->addAction(tr("&Info"));
  _actionInfo->setToolTip(tr("Toggle Info"));
  _actionInfo->setStatusTip(tr("Toggle Info"));
  _actionInfo->setCheckable(TRUE);
  _actionInfo->setChecked(TRUE);
  _actionInfo->setIcon(QIcon(about_xpm));
  connect(_actionInfo, SIGNAL(triggered(bool)), this, SLOT(infoToggled(bool)));
  
  // date
  _actionDate = _menu->addAction(tr("&Date Axis"));
  _actionDate->setToolTip(tr("Toggle Date Axis"));
  _actionDate->setStatusTip(tr("Toggle Date Axis"));
  _actionDate->setCheckable(TRUE);
  _actionDate->setChecked(TRUE);
  _actionDate->setIcon(QIcon(date_xpm));
  connect(_actionDate, SIGNAL(triggered(bool)), this, SLOT(dateToggled(bool)));
}

void
PlotWidgetArea::menuSelected (QAction *d)
{
  if (! _items.size())
    return;
  
  emit signalMarkerNew(d->data().toString(), _symbol);
}

QMenu *
PlotWidgetArea::menu ()
{
  return _menu;
}

void
PlotWidgetArea::contextMenu ()
{
  if (_itemSelected)
    return;

  _menu->exec(QCursor::pos());
}

