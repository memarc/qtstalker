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

#include <QtDebug>

#include "PlotObject.h"
#include "Util.h"
#include "PlotDialog.h"

PlotObject::PlotObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("Plot");
  _type = QString("widget");
  _row = 1;
  _col = 1;
  
  _widget = new PlotWidget(0, this);
  _widget->setName(_name);
  connect(_widget->plotArea(), SIGNAL(signalSettingsChanged()), this, SLOT(widgetSettingsChanged()));
  connect(_widget->plotArea(), SIGNAL(signalMarkerNew(QString, QString)), this, SLOT(markerNew(QString, QString)));
  connect(_widget->plotArea(), SIGNAL(signalMarkerDelete(QStringList)), this, SLOT(markerDelete(QStringList)));
  connect(_widget->plotArea()->cursorInfo(), SIGNAL(signalInfo(Data)), this, SLOT(plotInfo(Data)));
  connect(_widget->plotArea(), SIGNAL(signalDataWindow()), this, SLOT(dataWindow()));
  
  _commandList << QString("set_dates");
  _commandList << QString("add_objects");
  _commandList << QString("set_index");
  _commandList << QString("start_position");
  _commandList << QString("set_grid");
  _commandList << QString("set_spacing");
  _commandList << QString("clear");
  _commandList << QString("draw");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("objects");
  _commandList << QString("set_high_low");
  _commandList << QString("convert_to_y");
  _commandList << QString("date_to_index");
  _commandList << QString("index_to_x");
  _commandList << QString("x_to_index");
  _commandList << QString("index_to_date");
  _commandList << QString("convert_to_val");
  _commandList << QString("size");
  _commandList << QString("x_to_date");
  _commandList << QString("end_page_index");
  _commandList << QString("load_markers");
  _commandList << QString("remove_markers");
  _commandList << QString("dialog");
}

PlotObject::~PlotObject ()
{
//qDebug() << "PlotObject::~PlotObject: deleted" << _name;
//  delete _widget;
}

int
PlotObject::message (ObjectCommand *oc)
{
  int rc = 0;
  
  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = setDates(oc);
      break;
    case 1:
      rc = addObjects(oc);
      break;
    case 2:
      rc = setIndex(oc);
      break;
    case 3:
      rc = startPosition(oc);
      break;
    case 4:
      rc = setGrid(oc);
      break;
    case 5:
      rc = setSpacing(oc);
      break;
    case 6:
      rc = clear(oc);
      break;
    case 7:
      rc = draw(oc);
      break;
    case 8:
      rc = load(oc);
      break;
    case 9:
      rc = save(oc);
      break;
    case 10:
      rc = objects(oc);
      break;
    case 11:
      rc = setHighLow(oc);
      break;
    case 12:
      rc = convertToY(oc);
      break;
    case 13:
      rc = dateToIndex(oc);
      break;
    case 14:
      rc = indexToX(oc);
      break;
    case 15:
      rc = xToIndex(oc);
      break;
    case 16:
      rc = indexToDate(oc);
      break;
    case 17:
      rc = convertToVal(oc);
      break;
    case 18:
      rc = size(oc);
      break;
    case 19:
      rc = xToDate(oc);
      break;
    case 20:
      rc = endPageIndex(oc);
      break;
    case 21:
      rc = loadMarkers(oc);
      break;
    case 22:
      rc = removeMarkers(oc);
      break;
    case 23:
      rc = dialog(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

void
PlotObject::widgetSettingsChanged ()
{
  ObjectCommand oc(QString("modified"));
  oc.setValue(QString("name"), _name);
  emit signalMessage(oc);
}

void
PlotObject::markerNew (QString plugin, QString)
{
  Util util;
  Object *marker = util.object(plugin, _profile, QString());
  if (! marker)
  {
    qDebug() << "PlotObject::markerNew: object error" << plugin;
    return;
  }
  
  QString id = QUuid::createUuid().toString();
  id.remove("{");
  id.remove("}");
  id.remove("-");
  
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Plot") << QString("profile") << _profile;
  tl << QString("markers") << _name << _symbol << plugin << id;
  marker->setName(tl.join("/"));
  _widget->plotArea()->newItem(marker);
}

void
PlotObject::markerDelete (QStringList l)
{
  ObjectCommand oc(QString("remove_markers"));
  oc.setValue(QString("names"), l);
  removeMarkers(&oc);
}

void
PlotObject::plotInfo (Data d)
{
  ObjectCommand oc(QString("info"));
  oc.setValue(QString("info"), d);
  emit signalMessage(oc);
}

QWidget *
PlotObject::widget ()
{
  return _widget;
}

int
PlotObject::setDates (ObjectCommand *oc)
{
  QString key("input");
  Object *input = (Object *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "PlotObject::setDates: invalid" << key;
    return 0;
  }

  key = QString("key");
  QString dateKey = oc->getString(key);
  if (dateKey.isEmpty())
  {
    qDebug() << "PlotObject::setDates: invalid" << key;
    return 0;
  }
  
  key = QString("length");
  QString length = oc->getString(key);
  if (length.isEmpty())
  {
    qDebug() << "PlotObject::setDates: invalid" << key;
    return 0;
  }

  _widget->setDates(input, dateKey, length);
  
  return 1;
}

int
PlotObject::addObjects (ObjectCommand *oc)
{
  Util util;
  QHashIterator<QString, void *> it(oc->getObjects());
  while (it.hasNext())
  {
    it.next();
    Object *o = (Object *) it.value();
    
    if (o->type() == QString("marker") || o->type() == QString("curve"))
    {
      Object *co = util.object(o->plugin(), QString(), it.key());
      if (! co)
	continue;
      
      ObjectCommand toc(QString("copy"));
      toc.setValue(QString("input"), (void *) o);
      if (! co->message(&toc))
      {
        qDebug() << "PlotObject::addObjects: message error" << co->plugin() << toc.command();
	delete co;
	continue;
      }
      
      if (! _widget->plotArea()->addObject(it.key(), co))
        qDebug() << "PlotObject::addObjects: error adding object";
    }
  }

  return 1;
}

int
PlotObject::setIndex (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  _widget->setIndex(index);
  _widget->draw();
  
  return 1;
}

int
PlotObject::startPosition (ObjectCommand *oc)
{
  oc->setValue(QString("index"), _widget->plotArea()->startPosition());
  return 1;
}

int
PlotObject::setGrid (ObjectCommand *oc)
{
  bool tb = oc->getBool(QString("xgrid"));
  _widget->plotArea()->setXGrid(tb);

  tb = oc->getBool(QString("ygrid"));
  _widget->plotArea()->setYGrid(tb);
  
  _widget->draw();
  
  return 1;
}

int
PlotObject::setSpacing (ObjectCommand *oc)
{
  int spacing = oc->getInt(QString("spacing"));
  _widget->setSpacing(spacing);
  _widget->draw();
  return 1;
}

int
PlotObject::clear (ObjectCommand *)
{
  _widget->clear();
  return 1;
}

int
PlotObject::draw (ObjectCommand *)
{
  _widget->draw();
  return 1;
}

int
PlotObject::load (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "PlotObject::load: invalid" << key;
    return 0;
  }
  
  _row = settings->value(QString("row"), 1).toInt();
  _col = settings->value(QString("col"), 1).toInt();
  _widget->plotArea()->setXGrid(settings->value(QString("grid_x"), TRUE).toBool());
  _widget->plotArea()->setYGrid(settings->value(QString("grid_y"), TRUE).toBool());
  _widget->plotArea()->setInfo(settings->value(QString("info"), TRUE).toBool());
  
  bool tb = settings->value(QString("date"), TRUE).toBool();
  _widget->plotDate()->setVisible(tb);
  _widget->plotArea()->setDate(tb);
  
  return 1;
}

int
PlotObject::save (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "PlotObject::save: invalid" << key;
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("grid_x"), _widget->plotArea()->xGrid());
  settings->setValue(QString("grid_y"), _widget->plotArea()->yGrid());
  settings->setValue(QString("info"), _widget->plotArea()->info());
  settings->setValue(QString("date"), _widget->plotArea()->date());
  settings->setValue(QString("row"), _row);
  settings->setValue(QString("col"), _col);

  return 1;
}

int
PlotObject::objects (ObjectCommand *oc)
{
  QHash<QString, Object *> l = _widget->plotArea()->items();
  
  oc->clearValues();
  
  QHashIterator<QString, Object *> it(l);
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();
    oc->setValue(it.key(), (void *) o);
  }
  
  return 1;
}

int
PlotObject::setHighLow (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  double high = oc->getDouble(QString("high"));
  double low = oc->getDouble(QString("low"));
  _widget->getYMap()->setHighLow(index, high, low);
  return 1;
}

int
PlotObject::convertToY (ObjectCommand *oc)
{
  double v = oc->getDouble(QString("value"));
  int y = _widget->getYMap()->convertToY(v);
  oc->setValue(QString("y"), y);
  return 1;
}

int
PlotObject::dateToIndex (ObjectCommand *oc)
{
  QDateTime dt = oc->getDate(QString("date"));
  int index = _widget->getXMap()->dateToIndex(dt);
  oc->setValue(QString("index"), index);
  return 1;
}

int
PlotObject::indexToX (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  int x = _widget->getXMap()->indexToX(index);
  oc->setValue(QString("x"), x);
  return 1;
}

int
PlotObject::xToIndex (ObjectCommand *oc)
{
  int x = oc->getInt(QString("x"));
  int index = _widget->getXMap()->xToIndex(x);
  oc->setValue(QString("index"), index);
  return 1;
}

int
PlotObject::indexToDate (ObjectCommand *oc)
{
  int index = oc->getInt(QString("index"));
  QDateTime dt = _widget->getXMap()->indexToDate(index);
  oc->setValue(QString("date"), dt);
  return 1;
}

int
PlotObject::convertToVal (ObjectCommand *oc)
{
  int y = oc->getInt(QString("y"));
  double v = _widget->getYMap()->convertToVal(y);
  oc->setValue(QString("value"), v);
  return 1;
}

int
PlotObject::size (ObjectCommand *oc)
{
  oc->setValue(QString("size"), _widget->getXMap()->size());
  return 1;
}

int
PlotObject::xToDate (ObjectCommand *oc)
{
  int x = oc->getInt(QString("x"));
  QDateTime dt = _widget->getXMap()->indexToDate(_widget->getXMap()->xToIndex(x));
  oc->setValue(QString("date"), dt);
  return 1;
}

int
PlotObject::endPageIndex (ObjectCommand *oc)
{
  int width = oc->getInt(QString("width"));
  oc->setValue(QString("index"), _widget->getXMap()->endPageIndex(width));
  return 1;
}

int
PlotObject::loadMarkers (ObjectCommand *oc)
{
  if (_profile.isEmpty())
  {
    qDebug() << "PlotObject::loadMarkers: invalid profile";
    return 0;
  }
  
  if (_name.isEmpty())
  {
    qDebug() << "PlotObject::loadMarkers: invalid name";
    return 0;
  }
  
  QString key("symbol");
  _symbol = oc->getString(key);
  if (_symbol.isEmpty())
  {
    qDebug() << "PlotObject::loadMarkers: invalid" << key;
    return 0;
  }
  
  Util util;
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Plot") << QString("profile");
  tl << _profile << QString("markers") << _name << _symbol;
  dir.setPath(tl.join("/"));

  QStringList plugins = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Unsorted);

  for (int pluginPos = 0; pluginPos < plugins.size(); pluginPos++)
  {
    dir.cd(plugins.at(pluginPos));
    QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Unsorted);

    for (int filePos = 0; filePos < files.size(); filePos++)
    {
      QString fn = dir.absolutePath() + "/" + files.at(filePos);

      Object *marker = util.object(plugins.at(pluginPos), _profile, fn);
      if (! marker)
	continue;
      
      if (! _widget->plotArea()->addObject(fn, marker))
      {
        qDebug() << "PlotObject::loadMarkers: error adding marker";
        delete marker;
      }
    }
    
    dir.cd(QString(".."));
  }

  return 1;
}

int
PlotObject::removeMarkers (ObjectCommand *)
{
  if (_profile.isEmpty())
  {
    qDebug() << "PlotObject::removeMarkers: invalid profile";
    return 0;
  }
  
  if (_name.isEmpty())
  {
    qDebug() << "PlotObject::removeMarkers: invalid name";
    return 0;
  }
  
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Plot") << QString("profile") << _profile;
  tl << QString("markers") << _name << _symbol;
  dir.rmpath(tl.join("/"));
  
  return 1;
}

int
PlotObject::dialog (ObjectCommand *)
{
  PlotDialog *d = new PlotDialog(_name);
  d->setSettings(_row, _col);
  connect(d, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  d->setModified(FALSE);
  d->show();
  return 1;
}

void
PlotObject::dialogDone (void *dialog)
{
  PlotDialog *d = (PlotDialog *) dialog;
  d->settings(_row, _col);

  ObjectCommand toc(QString("modified"));
  toc.setValue(QString("name"), _name);
  emit signalMessage(toc);
}

void
PlotObject::dataWindow ()
{
  Util util;
  Object *dw = util.object(QString("ValuesDialog"), _profile, _name + " " + _symbol);
  if (! dw)
  {
    qDebug() << "PlotObject::dataWindow: invalid ValuesDialog object";
    return;
  }

  ObjectCommand toc(QString("dialog"));
  QHashIterator<QString, Object *> it(_widget->plotArea()->items());
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();
    if (o->type() != QString("curve"))
      continue;

    toc.setValue(it.key(), (void *) o);
  }

  dw->message(&toc);

  delete dw;
}
