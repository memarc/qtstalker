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


#include "YahooHistoryObject.h"
#include "YahooHistoryWidget.h"
#include "Util.h"

#include <QDebug>
#include <QtNetwork>
#include <QMainWindow>


YahooHistoryObject::YahooHistoryObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("YahooHistory");
  _type = QString("app");

  _commandList << QString("app");
  _commandList << QString("download");
}

int
YahooHistoryObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0: // app
      rc = app(oc);
      break;
    case 1: // download
      rc = download(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
YahooHistoryObject::app (ObjectCommand *oc)
{
  QMainWindow *mw = (QMainWindow *) oc->getObject(QString("main_window"));
  if (! mw)
  {
    qDebug() << "YahooHistoryObject::app: invalid main_window";
    return 0;
  }

  new YahooHistoryWidget(mw, _name);
  return 1;
}

int
YahooHistoryObject::download (ObjectCommand *oc)
{
  // symbol
  QString key("symbol");
  QString symbol = oc->getString(key);
  if (symbol.isEmpty())
  {
    qDebug() << "YahooHistoryObject::download: invalid" << key;
    return 0;
  }
  
  // start date
  key = QString("start_date");
  QDateTime sd = oc->getDate(key);
  if (! sd.isValid())
  {
    qDebug() << "YahooHistoryObject::download: invalid" << key << sd;
    return 0;
  }

  // end date
  key = QString("end_date");
  QDateTime ed = oc->getDate(key);
  if (! ed.isValid())
  {
    qDebug() << "YahooHistoryObject::download: invalid" << key << ed;
    return 0;
  }
  
  // status message
  ObjectCommand tpc(QString("status"));
  QStringList mess;
  mess << tr("Downloading") << symbol;
  tpc.setValue(QString("info"), mess.join(" "));
  emit signalMessage(tpc);
  
  // get name
  QString name;
  if (! downloadName(symbol, name))
  {
    QStringList mess;
    mess << "\t" << tr("Downloading ticker") << symbol << tr("name failed");
    tpc.setValue(QString("info"), mess.join(" "));
    emit signalMessage(tpc);
  }

  // get the url
  QString url;
  getUrl(sd, ed, symbol, url);

  // download the data
  QNetworkAccessManager manager;
  QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
  QEventLoop e;
  QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), &e, SLOT(quit()));
  e.exec();

  // parse the data and save quotes
  QByteArray ba = reply->readAll();
  parseHistory(ba, symbol, name);

  // status message
  mess.clear();
  mess << tr("Download complete");
  tpc.setValue(QString("info"), mess.join(" "));
  emit signalMessage(tpc);
  
  return 1;
}

void
YahooHistoryObject::getUrl (QDateTime sd, QDateTime ed, QString symbol, QString &url)
{
//http://ichart.finance.yahoo.com/table.csv?s=AAPL&d=1&e=22&f=2011&g=d&a=8&b=7&c=1984&ignore=.csv
  url = "http://ichart.finance.yahoo.com/table.csv?s=";
  url.append(symbol);
  url.append("&d=" + QString::number(ed.date().month() - 1));
  url.append("&e=" + ed.date().toString("d"));
  url.append("&f=" + ed.date().toString("yyyy"));
  url.append("&a=" + QString::number(sd.date().month() - 1));
  url.append("&b=" + sd.date().toString("d"));
  url.append("&c=" + sd.date().toString("yyyy"));
  url.append("&ignore=.csv");
}

void
YahooHistoryObject::parseHistory (QByteArray &ba, QString &symbol, QString &name)
{
  ObjectCommand message("status");

  QTemporaryFile file;
//  file.setAutoRemove(FALSE);
  if (! file.open())
  {
    qDebug() << "YahooHistoryObject::parseHistory: error opening tmp file";
    return;
  }

  QString ts(ba);
  QStringList tl = ts.split('\n');
  
  QTextStream out(&file);
  for (int pos = 1; pos < tl.size(); pos++) // skip first line
  {
    QString s = tl.at(pos).trimmed();
    if (s.isEmpty())
      continue;
    
    out << name << "," << symbol << "," << tl.at(pos) << "\n";
  }

  file.close();
  
  Util fac;
  Object *csv = fac.object(QString("CSV"), QString(), QString());
  if (! csv)
  {
    QStringList mess;
    mess << "\t" << tr("CSV object error");
    message.setValue(QString("info"), mess.join(" "));
    emit signalMessage(message);
    return;
  }

  // relay messages from csv to yahoo log
  connect(csv, SIGNAL(signalMessage(ObjectCommand)), this, SIGNAL(signalMessage(ObjectCommand)));
  
  ObjectCommand oc(QString("import"));
  oc.setValue(QString("files"), QStringList() << file.fileName());
  oc.setValue(QString("format"), QString("N,S,D,O,H,L,C,V,G"));
  oc.setValue(QString("date_format"), QString("yyyy-MM-dd"));
  oc.setValue(QString("delimiter"), QString("Comma"));
  oc.setValue(QString("type"), QString("Stock"));
  oc.setValue(QString("exchange"), QString("YAHOO"));
  oc.setValue(QString("use_filename"), FALSE);
  
  csv->message(&oc);
  
  delete csv;
  
  return;
}

int
YahooHistoryObject::downloadName (QString symbol, QString &name)
{
  QString url = "http://download.finance.yahoo.com/d/quotes.csv?s=";
  url.append(symbol);
  url.append("&f=n");
  url.append("&e=.csv");

  QNetworkAccessManager manager;
  QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
  QEventLoop e;
  QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), &e, SLOT(quit()));
  e.exec();

  // parse the data and save quotes
  QByteArray ba = reply->readAll();
  QString s(ba);
  s = s.remove('"');
  s = s.remove(',');
  s = s.trimmed();
  if (s.isEmpty())
    return 0;

  name = s;

  return 1;
}
