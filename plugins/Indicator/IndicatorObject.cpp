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

#include "IndicatorObject.h"
#include "IndicatorDialog.h"
#include "Util.h"

#include <QDebug>
#include <QDir>


IndicatorObject::IndicatorObject (QString profile, QString name)
{
  _plugin = QString("Indicator");
  _profile = profile;
  _name = name;
  _type = QString("plot");
  _widget = 0;

  _commandList << QString("set_input");
  _commandList << QString("set_index");
  _commandList << QString("load_markers");
  _commandList << QString("dialog");
  _commandList << QString("load");
  _commandList << QString("plugin_steps");
  _commandList << QString("get_object");

  _plotCommands << QString("edit");
  _plotCommands << QString("delete");
  _plotCommands << QString("info");
  _plotCommands << QString("modified");
}

IndicatorObject::~IndicatorObject ()
{
  clear();
  
  if (_widget)
  {
    if (! _widget->parent())
      delete _widget;
  }
}

void
IndicatorObject::clear ()
{
  _order.clear();

  saveMarkers();

  qDeleteAll(_objects);
  _objects.clear();
}

int
IndicatorObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      return setInput(oc);
      break;
    case 1:
      return setIndex(oc);
      break;
    case 2:
      return loadMarkers(oc);
      break;
    case 3:
      return dialog(oc);
      break;
    case 4:
      return load(oc);
      break;
    case 5:
      return pluginSteps(oc);
      break;
    case 6:
      return getObject(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

void
IndicatorObject::plotMessage (ObjectCommand oc)
{
  switch (_plotCommands.indexOf(oc.command()))
  {
    case 0:
      dialog(0);
      break;
    case 1:
      remove();
      break;
    case 2:
      emit signalMessage(oc);
      break;
    case 3:
      savePlotSettings();
      break;
    default:
      break;
  }
}

QWidget *
IndicatorObject::widget ()
{
  return _widget;
}

int
IndicatorObject::setInput (ObjectCommand *oc)
{
  saveMarkers();
  
  // input
  QString key(QString("input"));
  Object *input = (Object *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "IndicatorObject::setInput: invalid" << key;
    return 0;
  }
  
  if (input->plugin() != QString("Symbol"))
  {
    qDebug() << "IndicatorObject::setInput: invalid plugin" << input->plugin() << key;
    return 0;
  }

  // copy symbol
  Object *symbol = (Object *) _objects.value(QString("symbol"));
  if (! symbol)
  {
    qDebug() << "IndicatorObject::setInput: invalid symbol";
    return 0;
  }
  
  key = QString("copy");
  ObjectCommand toc(key);
  toc.setValue(QString("input"), (void *) input);
  if (! symbol->message(&toc))
  {
    qDebug() << "IndicatorObject::setInput: message error" << symbol->plugin() << key;
    return 0;
  }
  
  _symbol = oc->getString(QString("symbol"));
  _length = oc->getString(QString("length"));
//qDebug() << "IndicatorObject::setInput:" << _symbol << _length;
  
  plot();

  return 1;
}

int
IndicatorObject::setIndex (ObjectCommand *oc)
{
  QHashIterator<QString, Object *> it(_objects);
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();
    if (o->plugin() != QString("Plot"))
      continue;

    o->message(oc);
  }
  
  return 1;
}

int
IndicatorObject::loadMarkers (ObjectCommand *oc)
{
  QHashIterator<QString, Object *> it(_objects);
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();
    if (o->plugin() != QString("Plot"))
      continue;

    o->message(oc);
  }
  
  return 1;
}

int
IndicatorObject::plot ()
{
  Util util;
  ObjectCommand toc(QString("update"));
  for (int pos = 0; pos < _order.size(); pos++)
  {
    Object *o = _objects.value(_order.at(pos));
    if (! o)
      continue;

    toc.setValue(_order.at(pos), (void *) o);
    
    if (o->plugin() != QString("Plot"))
    {
      if (! o->message(&toc))
      {
//        qDebug() << "IndicatorObject::plot: Plot message error" << o->plugin() << toc.command();
        continue;
      }
//qDebug() << "IndicatorObject::plot: OK" << o->plugin() << toc.command();
      continue;
    }
    
    // clear plot
    QString key("clear");
    ObjectCommand ttoc(key);
    if (! o->message(&ttoc))
    { 
      qDebug() << "IndicatorObject::plot: Plot message error" << o->plugin() << key;
      return 0;
    }

    key = QString("symbol");
    Object *symbol = _objects.value(key);
    if (! symbol)
    {
      qDebug() << "IndicatorObject::plot: invalid" << key;
      return 0;
    }
  
    // set dates for plot
    key = QString("set_dates");
    ttoc.setCommand(key);
    ttoc.setValue(QString("input"), (void *) symbol);
    ttoc.setValue(QString("key"), QString("D"));
    ttoc.setValue(QString("length"), _length);
    if (! o->message(&ttoc))
    {
      qDebug() << "IndicatorObject::plot: Plot message error" << o->plugin() << key;
      return 0;
    }
  }

  return 1;
}

int
IndicatorObject::dialog (ObjectCommand *)
{
  IndicatorDialog *d = new IndicatorDialog(_indicatorPath, _name);
  connect(d, SIGNAL(signalDone(void *)), this, SLOT(dialogDone()));
  connect(d, SIGNAL(signalCancel()), this, SLOT(dialogCancel()));
  d->setModified(FALSE);
  d->show();
  return 1;
}

void
IndicatorObject::dialogDone ()
{
  loadLocal();
  
  plot();

  ObjectCommand toc(QString("draw"));
  QHashIterator<QString, Object *> it(_objects);
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();
    toc.setValue(it.key(), (void *) it.value());
    
    if (o->plugin() != QString("Plot"))
      continue;

    o->message(&toc);
  }
  
  toc.setCommand(QString("dialog_done"));
  toc.setValue(QString("name"), _name);
  emit signalMessage(toc);
}

void
IndicatorObject::dialogCancel ()
{
  ObjectCommand toc(QString("dialog_cancel"));
  toc.setValue(QString("name"), _name);
  emit signalMessage(toc);
}

int
IndicatorObject::load (ObjectCommand *oc)
{
  _indicatorPath = oc->getString(QString("file"));
  if (_indicatorPath.isEmpty())
  {
    qDebug() << "IndicatorObject::load: invalid path";
    return 0;
  }

  return loadLocal();
}

int
IndicatorObject::loadLocal ()
{
  QDir dir;
  if (! dir.exists(_indicatorPath))
  {
    qDebug() << "IndicatorObject::loadLocal: empty indicator file";
    return 1;
  }
  
  // setup widget and grid
  if (! _widget)
  {
    _widget = new QWidget;
    
    _grid = new QGridLayout;
    _grid->setMargin(0);
    _grid->setSpacing(0);
    _widget->setLayout(_grid);
  }

  QSettings *settings = new QSettings(_indicatorPath, QSettings::NativeFormat);
  
  _order = settings->value(QString("order")).toStringList();
  
  ObjectCommand toc(QString("load"));
  toc.setValue(QString("QSettings"), (void *) settings);

  QStringList oldObjects = _objects.keys();
  
  Util util;
  for (int pos = 0; pos < _order.size(); pos++)
  {
    settings->beginGroup(_order.at(pos));

    QString plugin = settings->value(QString("plugin")).toString();

    // new object?
    bool newFlag = FALSE;
    Object *o = _objects.value(_order.at(pos));
    if (! o)
    {
      o = util.object(plugin, _profile, _order.at(pos));
      if (! o)
      {
        settings->endGroup();
        continue;
      }
      _objects.insert(_order.at(pos), o);
      newFlag = TRUE;
    }
    else
    {
      // plugin has same name new type?
      if (o->plugin() != plugin)
      {
        delete o;
        o = util.object(plugin, _profile, _order.at(pos));
        if (! o)
        {
          settings->endGroup();
          continue;
        }
        _objects.insert(_order.at(pos), o);
        newFlag = TRUE;
      }
    }
      
    // load settings
    if (! o->message(&toc))
    {
      qDebug() << "IndicatorObject::loadLocal: object message error" << o->name() << toc.command();
      settings->endGroup();
      continue;
    }

    // deal with any plot objects
    if (plugin == QString("Plot"))
    {
      if (newFlag)
        connect(o, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(plotMessage(ObjectCommand)));
      
      int row = settings->value(QString("row")).toInt();
      int col = settings->value(QString("col")).toInt();
      
      if (! newFlag)
	_grid->removeWidget(o->widget());
      
      _grid->addWidget(o->widget(), row, col);
    }
    
    settings->endGroup();
qDebug() << "IndicatorObject::loadLocal: OK" << _order.at(pos) << plugin;
  }

  delete settings;
  
  // remove old objects
  for (int pos = 0; pos < oldObjects.size(); pos++)
  {
    Object *o = _objects.value(oldObjects.at(pos));
    if (o)
      continue;
    delete o;
    _objects.remove(oldObjects.at(pos));
qDebug() << "IndicatorObject::loadLocal: removed old object" << o->plugin() << oldObjects.at(pos);
  }

  return 1;
}

void
IndicatorObject::savePlotSettings ()
{
  QSettings *settings = new QSettings(_indicatorPath, QSettings::NativeFormat);
  
  ObjectCommand toc(QString("save"));
  toc.setValue(QString("QSettings"), (void *) settings);
  
  QHashIterator<QString, Object *> it(_objects);
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();
    if (o->plugin() != QString("Plot"))
      continue;
    
    settings->beginGroup(it.key());
    if (! o->message(&toc))
      qDebug() << "IndicatorObject::savePlotSettings: message error" << o->plugin() << toc.command();
    settings->endGroup();
  }
  
  delete settings;
}

int
IndicatorObject::remove ()
{
  // remove markers
  QHashIterator<QString, Object *> it(_objects);
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();
    if (o->plugin() != QString("Plot"))
      continue;
    
    ObjectCommand toc(QString("remove_markers"));
    toc.setValue(QString("symbol"), _symbol);
    o->message(&toc);
  }
  
  // send parent message to delete me
  ObjectCommand toc(QString("delete"));
  toc.setValue(QString("name"), _name);
  toc.setValue(QString("file"), _indicatorPath);
  emit signalMessage(toc);
  
  return 1;
}

void
IndicatorObject::saveMarkers ()
{
  QHashIterator<QString, Object *> it(_objects);
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();
    if (o->plugin() != QString("Plot"))
      continue;
    
    ObjectCommand toc(QString("save_markers"));
    toc.setValue(QString("symbol"), _symbol);
    o->message(&toc);
  }
}

int
IndicatorObject::pluginSteps (ObjectCommand *oc)
{
  QString plugin = oc->getString(QString("plugin"));
  if (plugin.isEmpty())
  {
    qDebug() << "IndicatorObject::pluginSteps: invalid plugin";
    return 0;
  }

  QStringList tl;
  QHashIterator<QString, Object *> it(_objects);
  while (it.hasNext())
  {
    it.next();
    Object *o = it.value();
    if (o->plugin() == plugin)
      tl << it.key();
  }
  
  oc->setValue(QString("steps"), tl);
  
  return 1;
}

int
IndicatorObject::getObject (ObjectCommand *oc)
{
  QString name = oc->getString(QString("name"));
  if (name.isEmpty())
  {
    qDebug() << "IndicatorObject::getObject: invalid name";
    return 0;
  }

  Object *o = _objects.value(name);
  if (! o)
  {
    qDebug() << "IndicatorObject::getObject: invalid name";
    return 0;
  }
  
  oc->setValue(QString("object"), (void *) o);
  
  return 1;
}
