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

#include "SymbolObject.h"
#include "SymbolSelectDialog.h"
#include "Util.h"


SymbolObject::SymbolObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("Symbol");
  _type = QString("system");
  _hasOutput = TRUE;

  _commands << QString("get");
  _commands << QString("list");
  _commands << QString("set");
  _commands << QString("output");
  _commands << QString("search");
  _commands << QString("size");
  _commands << QString("dialog");
  _commands << QString("symbol");
  _commands << QString("load");
  _commands << QString("save");
  _commands << QString("copy");
  _commands << QString("output_keys");
  _commands << QString("info");
  _commands << QString("exchanges");
  _commands << QString("search_dialog");
  
  _lengths << "1M";
  _lengths << "5M";
  _lengths << "10M";
  _lengths << "15M";
  _lengths << "30M";
  _lengths << "60M";
  _lengths << "D";
  _lengths << "W";
  _lengths << "M";
  
  _interval = 1;

  // db path
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Symbol") << QString("db");
  _path = tl.join("/");
  _file = QString("db");
  
  // get db object
  Util util;
  _db = util.object(QString("DataBase"), QString(), QString());
  if (_db)
  {
    // open db
    ObjectCommand oc(QString("open"));
    oc.setValue(QString("path"), _path);
    oc.setValue(QString("file"), _file);
    if (! _db->message(&oc))
      qDebug() << "SymbolObject::SymbolObject: message error" << _db->plugin() << oc.command();
  }
  else
    qDebug() << "SymbolObject::SymbolObject: invalid db object";
}

SymbolObject::~SymbolObject ()
{
  qDeleteAll(_bars);
  if (_db)
    delete _db;
}

int
SymbolObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commands.indexOf(oc->command()))
  {
    case 0:
      rc = loadBars(oc);
      break;
    case 1:
      oc->setValue(QString("list"), _lengths);
      rc = 1;
      break;
    case 2:
      rc = saveBars(oc);
      break;
    case 3:
      rc = values(oc);
      break;
    case 4:
      rc = search(oc);
      break;
    case 5:
      rc = size(oc);
      break;
    case 6:
      rc = dialog(oc);
      break;
    case 7:
      rc = symbol(oc);
      break;
    case 8:
      rc = load(oc);
      break;
    case 9:
      rc = save(oc);
      break;
    case 10:
      rc = copy(oc);
      break;
    case 11:
      rc = outputKeys(oc);
      break;
    case 12:
      rc = info(oc);
      break;
    case 13:
      rc = exchanges(oc);
      break;
    case 14:
      rc = searchDialog(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
SymbolObject::loadBars (ObjectCommand *oc)
{
  qDeleteAll(_bars);
  _bars.clear();
  _info.clear();
  
  if (! _db)
  {
    qDebug() << "SymbolObject::load: invalid db object";
    return 0;
  }

  QString key("exchange");
  QString exchange = oc->getString(key);
  if (exchange.isEmpty())
  {
    qDebug() << "SymbolObject::load: invalid" << key;
    return 0;
  }
  
  key = QString("ticker");
  QString ticker = oc->getString(key);
  if (ticker.isEmpty())
  {
    qDebug() << "SymbolObject::load: invalid" << key;
    return 0;
  }
  
  key = QString("length");
  QString length = oc->getString(key);
  if (length.isEmpty())
  {
    qDebug() << "SymbolObject::load: invalid" << key;
    return 0;
  }
  
  key = QString("start_date");
  QDateTime sd = oc->getDate(key);
  if (! sd.isValid())
  {
    qDebug() << "SymbolObject::load: invalid" << key << sd;
    return 0;
  }

  key = QString("end_date");
  QDateTime ed = oc->getDate(key);
  if (! ed.isValid())
  {
    qDebug() << "SymbolObject::load: invalid" << key << ed;
    return 0;
  }
  
  QString symbol = exchange + ":" + ticker;

  Data info;
  if (! getInfo(symbol, info))
  {
    qDebug() << "SymbolObject::load: getInfo error";
    return 0;
  }
  else
    _info = info;

  QString table = _info.value(QString("table")).toString();
  
  if (! getBars(sd, ed, table, length))
  {
    qDebug() << "SymbolObject::load: getBars error";
    return 0;
  }
  
//qDebug() << "SymbolObject::load" << _bars.size();  

  return 1;
}

int
SymbolObject::saveBars (ObjectCommand *oc)
{
  if (! _db)
  {
    qDebug() << "SymbolObject::save: invalid db object";
    return 0;
  }

  QString key("exchange");
  QString exchange = oc->getString(key);
  if (exchange.isEmpty())
  {
    qDebug() << "SymbolObject::save: invalid" << key;
    return 0;
  }
  
  key = QString("ticker");
  QString ticker = oc->getString(key);
  if (ticker.isEmpty())
  {
    qDebug() << "SymbolObject::save: invalid" << key;
    return 0;
  }
  
  Data info;
  QString symbol = exchange + ":" + ticker;
  if (! getInfo(symbol, info))
  {
    qDebug() << "SymbolObject::save: getInfo error";
    return 0;
  }
  
  // are we adding a new symbol?
  QString table = info.value("table").toString();
  if (table.isEmpty())
  {
    info.insert(QString("exchange"), QVariant(exchange));
    info.insert(QString("ticker"), QVariant(ticker));
    
    key = QString("type");
    QString type = oc->getString(key);
    if (type.isEmpty())
    {
      qDebug() << "SymbolObject::save: invalid" << key;
      return 0;
    }
    info.insert(key, QVariant(type));
    
    key = QString("name");
    QString name = oc->getString(key);
    if (name.isEmpty())
    {
      qDebug() << "SymbolObject::save: invalid" << key;
      return 0;
    }
    info.insert(key, QVariant(name));
    
    table = "Q" + QUuid::createUuid().toString();
    table.remove("{");
    table.remove("}");
    table.remove("-");
    info.insert(QString("table"), QVariant(table));
    
    if (! setInfo(info))
    {
      qDebug() << "SymbolObject::save: DataBase object error";
      return 0;
    }
  }
  
  ObjectCommand toc(QString("transaction"));
  if (! _db->message(&toc))
  {
    qDebug() << "SymbolObject::save: message error" << toc.command();
    return 0;
  }

  toc.setCommand(QString("set"));
  toc.setValue(QString("table"), table);
  toc.setDatas(oc->getDatas());
  if (! _db->message(&toc))
  {
    qDebug() << "SymbolObject::save: message error" << toc.command();
    return 0;
  }
  
  toc.setCommand(QString("commit"));
  if (! _db->message(&toc))
  {
    qDebug() << "SymbolObject::save: message error" << toc.command();
    return 0;
  }
  
  return 1;
}

int
SymbolObject::search (ObjectCommand *oc)
{
  if (! _db)
  {
    qDebug() << "SymbolObject::search: invalid db object";
    return 0;
  }
  
  ObjectCommand toc(QString("search"));
  toc.setValue(QString("table"), QString("symbolIndex"));
  
  Data search;
  search.insert(QString("exchange"), oc->getString(QString("exchange")));
  search.insert(QString("ticker"), oc->getString(QString("ticker")));
  search.insert(QString("type"), oc->getString(QString("type")));
  search.insert(QString("name"), oc->getString(QString("name")));
  toc.setValue(QString("search"), search);
  if (! _db->message(&toc))
  {
    qDebug() << "SymbolObject::search: message error" << toc.command();
    return 0;
  }

  oc->setValue(QString("list"), toc.getList(QString("list")));

  return 1;
}

int
SymbolObject::exchanges (ObjectCommand *oc)
{
  if (! _db)
  {
    qDebug() << "SymbolObject::exchanges: invalid db object";
    return 0;
  }
  
  ObjectCommand toc(QString("search"));
  toc.setValue(QString("table"), QString("symbolIndex"));
  
  Data search;
  search.insert(QString("exchange"), oc->getString(QString("%")));
  toc.setValue(QString("search"), search);
  if (! _db->message(&toc))
  {
    qDebug() << "SymbolObject::search: message error" << toc.command();
    return 0;
  }

  oc->setValue(QString("list"), toc.getList(QString("list")));

  return 1;
}

int
SymbolObject::values (ObjectCommand *oc)
{
  oc->setMap(_bars);
  oc->setValue(QString("exchange"), _exchange);
  oc->setValue(QString("ticker"), _ticker);
  return 1;
}

int
SymbolObject::size (ObjectCommand *oc)
{
  oc->setValue(QString("size"), _bars.size());
  return 1;
}

int
SymbolObject::dialog (ObjectCommand *)
{
  SymbolDialog *dialog = new SymbolDialog(_name);
  dialog->setSettings(_exchange, _ticker);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->show();
  return 1;
}

void
SymbolObject::dialogDone (void *dialog)
{
  SymbolDialog *d = (SymbolDialog *) dialog;
  d->settings(_exchange, _ticker);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
SymbolObject::symbol (ObjectCommand *oc)
{
  QString ts = _exchange + ":" + _ticker;
  oc->setValue(QString("symbol"), ts);
  return 1;
}

int
SymbolObject::load (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "SymbolObject::load: invalid" << key;
    return 0;
  }

  _exchange = settings->value(QString("exchange")).toString();
  _ticker = settings->value(QString("ticker")).toString();
  
  return 1;
}

int
SymbolObject::save (ObjectCommand *oc)
{
  QString key("QSettings");
  QSettings *settings = (QSettings *) oc->getObject(key);
  if (! settings)
  {
    qDebug() << "SymbolObject::save: invalid" << key;
    return 0;
  }

  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("exchange"), _exchange);
  settings->setValue(QString("ticker"), _ticker);

  return 1;
}

int
SymbolObject::copy (ObjectCommand *oc)
{
  QString key("input");
  Object *input = (Object *) oc->getObject(key);
  if (! input)
  {
    qDebug() << "SymbolObject::copy: invalid" << key;
    return 0;
  }
  
  if (input->plugin() != _plugin)
  {
    qDebug() << "SymbolObject::copy: invalid" << input->plugin() << _plugin << key;
    return 0;
  }
  
  ObjectCommand toc(QString("output"));
  if (! input->message(&toc))
  {
    qDebug() << "SymbolObject::copy: message error" << input->plugin() << toc.command();
    return 0;
  }
  
  qDeleteAll(_bars);
  _bars.clear();
  
  _exchange = toc.getString(QString("exchange"));
  _ticker = toc.getString(QString("ticker"));
  
  QMapIterator<int, Data *> it(toc.map());
  while (it.hasNext())
  {
    it.next();
    Data *b = it.value();
    
    QStringList keys = b->keys();
    
    Data *nb = new Data;
    for (int pos = 0; pos < keys.size(); pos++)
      nb->insert(keys.at(pos), b->value(keys.at(pos)));
    _bars.insert(it.key(), nb);
  }
//qDebug() << "SymbolObject::copy:" << _bars.size();

  return 1;
}

int
SymbolObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << "D" << "O" << "H" << "L" << "C" << "V";
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
SymbolObject::info (ObjectCommand *oc)
{
  QString key("names");
  QStringList names = oc->getList(key);

  for (int pos = 0; pos < names.size(); pos++)
  {
    Data i;
    if (! getInfo(names.at(pos), i))
      continue;
    
    oc->setValue(names.at(pos), i);
  }
  
  return 1;
}

int
SymbolObject::searchDialog (ObjectCommand *oc)
{
  SymbolSelectDialog *dialog = new SymbolSelectDialog(_name);
  dialog->setSettings(oc->getString(QString("exchange")),
		      oc->getString(QString("ticker")),
		      oc->getString(QString("type")),
		      oc->getString(QString("name")));
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(searchDialogDone(void *)));
  dialog->show();
  return 1;
}

void
SymbolObject::searchDialogDone (void *dialog)
{
  SymbolSelectDialog *d = (SymbolSelectDialog *) dialog;
  QHash<QString, Data> symbols;
  QString exchange, ticker, type, name;
  d->settings(symbols, exchange, ticker, type, name);
  
  ObjectCommand oc(QString("search"));
  oc.setValue(QString("exchange"), exchange);
  oc.setValue(QString("ticker"), ticker);
  oc.setValue(QString("type"), type);
  oc.setValue(QString("name"), name);
  oc.setDatas(symbols);
  emit signalMessage(oc);
}

int
SymbolObject::getInfo (QString symbol, Data &info)
{
  info.clear();

  if (! _db)
  {
    qDebug() << "SymbolObject::getInfo: invalid db object";
    return 0;
  }
  
  ObjectCommand oc(QString("get"));
  oc.setValue(QString("table"), QString("symbolIndex"));
  
  QStringList tl;
  tl << symbol;
  oc.setValue(QString("names"), tl);

  if (! _db->message(&oc))
  {
    qDebug() << "SymbolObject::getInfo: message error" << oc.command();
    return 0;
  }
  
  info = oc.getData(symbol);
  
  return 1;
}

int
SymbolObject::setInfo (Data &info)
{
  if (! _db)
  {
    qDebug() << "SymbolObject::setInfo: invalid db object";
    return 0;
  }
  
  ObjectCommand oc(QString("transaction"));
  if (! _db->message(&oc))
  {
    qDebug() << "SymbolObject::setInfo: message error" << oc.command();
    return 0;
  }
  
  QString symbol = info.value("exchange").toString() + ":" + info.value("ticker").toString();
  
  oc.setCommand(QString("set"));
  oc.setValue(QString("table"), QString("symbolIndex"));
  oc.setValue(symbol, info);
  if (! _db->message(&oc))
  {
    qDebug() << "SymbolObject::setInfo: message error" << oc.command();
    return 0;
  }
  
  oc.setCommand(QString("commit"));
  if (! _db->message(&oc))
  {
    qDebug() << "SymbolObject::setInfo: message error" << oc.command();
    return 0;
  }
  
  return 1;
}

int
SymbolObject::getBars (QDateTime &startDate, QDateTime &endDate, QString table, QString length)
{
  if (! _db)
  {
    qDebug() << "SymbolObject::getBars: invalid db object";
    return 0;
  }

  // load all bars within date range
  ObjectCommand oc(QString("get_range"));
  oc.setValue(QString("table"), table);
  oc.setValue(QString("range_start"), startDate.toString("yyyyMMddHHmmss"));
  oc.setValue(QString("range_end"), endDate.toString("yyyyMMddHHmmss"));
  if (! _db->message(&oc))
  {
    qDebug() << "SymbolObject::getBars: message error" << oc.command();
    return 0;
  }
//qDebug() << "SymbolObject::getBars: records" << oc.outputs.size();

  QHash<QString, Data> output = oc.getDatas();
  QList<QString> keys = output.keys();
  qSort(keys);

  QHash<QString, Data *> tbars;
  for (int pos = 0; pos < keys.size(); pos++)
  {
    Data d = output.value(keys.at(pos));
    
    QDateTime td = QDateTime::fromString(keys.at(pos), "yyyyMMddHHmmss");
    if (! td.isValid())
      continue;

    QDateTime sd = td;
    QDateTime ed;
    if (! getInterval(length, 1, sd, ed))
      continue;
    
    QString key = ed.toString("yyyyMMddHHmmss");
    
    Data *bar = tbars.value(key);
    if (! bar)
    {
      bar = new Data;
      bar->insert(QString("D"), QVariant(td));
      bar->insert(QString("O"), d.value(QString("O")));
      bar->insert(QString("H"), d.value(QString("H")));
      bar->insert(QString("L"), d.value(QString("L")));
      bar->insert(QString("C"), d.value(QString("C")));
      bar->insert(QString("V"), d.value(QString("V")));
      tbars.insert(key, bar);
    }
    else
    {
      // update to latest date
      bar->insert(QString("D"), QVariant(td));
      
      // test high
      double t = d.value(QString("H")).toDouble();
      double t2 = bar->value(QString("H")).toDouble();
      if (t > t2)
	bar->insert(QString("H"), QVariant(t));

      // test low
      t = d.value(QString("L")).toDouble();
      t2 = bar->value(QString("L")).toDouble();
      if (t < t2)
	bar->insert(QString("L"), QVariant(t));

      // accumulate volume
      t = bar->value(QString("V")).toDouble();
      t += d.value(QString("V")).toDouble();
      bar->insert(QString("V"), QVariant(t));
      
      // latest close
      bar->insert(QString("C"), d.value(QString("C")));
    }
  }
  
  keys = tbars.keys();
  qSort(keys);
  for (int pos = 0; pos < keys.size(); pos++)
    _bars.insert(pos, tbars.value(keys.at(pos)));

  return 1;
}

int
SymbolObject::getInterval (QString &length, int interval, QDateTime &sd, QDateTime &ed)
{
  switch (_lengths.indexOf(length))
  {
    case 0: // 1M
      sd.setTime(QTime(sd.time().hour(), sd.time().minute(), 0, 0));
      ed = sd;
      ed = ed.addSecs(60);
//      ed = ed.addSecs(-1);
      break;
    case 1: // 5M
    {
      int tint = sd.time().minute() / 5;
      sd.setTime(QTime(sd.time().hour(), tint * 5, 0, 0));
      ed = sd;
      ed = ed.addSecs(300);
 //     ed = ed.addSecs(-1);
      break;
    }
    case 2: // 10M
    {
      int tint = sd.time().minute() / 10;
      sd.setTime(QTime(sd.time().hour(), tint * 10, 0, 0));
      ed = sd;
      ed = ed.addSecs(600);
//      ed = ed.addSecs(-1);
      break;
    }
    case 3: // 15M
    {
      int tint = sd.time().minute() / 15;
      sd.setTime(QTime(sd.time().hour(), tint * 15, 0, 0));
      ed = sd;
      ed = ed.addSecs(900);
//      ed = ed.addSecs(-1);
      break;
    }
    case 4: // 30M
    {
      int tint = sd.time().minute() / 30;
      sd.setTime(QTime(sd.time().hour(), tint * 30, 0, 0));
      ed = sd;
      ed = ed.addSecs(1800);
//      ed = ed.addSecs(-1);
      break;
    }
    case 5: // 60M
      sd.setTime(QTime(sd.time().hour(), 0, 0, 0));
      ed = sd;
      ed = ed.addSecs(3600);
//      ed = ed.addSecs(-1);
      break;
    case 6: // D
      sd.setTime(QTime(0, 0, 0, 0));
      ed = sd;
      ed = ed.addDays(1);
//      ed = ed.addSecs(-1);
      break;
    case 7: // W
      sd.setTime(QTime(0, 0, 0, 0));
      sd = sd.addDays(- (sd.date().dayOfWeek() - 1));
      ed = sd;
      ed = ed.addDays(7);
//      ed = ed.addSecs(-1);
      break;
    case 8: // M
      sd.setTime(QTime(0, 0, 0, 0));
      sd = sd.addDays(- (sd.date().day() - 1));
      ed = sd;
      for (int loop = 0; loop < interval; loop++)
        ed = ed.addDays(ed.date().daysInMonth());
//      ed = ed.addSecs(-1);
      break;
    default:
      return 0;
      break;
  }
  
  return 1;
}
