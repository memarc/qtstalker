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

#include "ScannerThread.h"
#include "Util.h"

#include <QDebug>

ScannerThread::ScannerThread (QObject *p, QString pro, QStringList l, QString bl, QDateTime sd, QDateTime ed) : QThread (p)
{
  _stop = FALSE;
  _symbols = l;
  _length = bl;
  _startDate = sd;
  _endDate = ed;
  _profile = pro;
  
  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

ScannerThread::~ScannerThread ()
{
  for (int pos = 0; pos < _items.size(); pos++)
  {
    IndicatorItem ii = _items.at(pos);
    delete ii.i;
  }
}

void
ScannerThread::run ()
{
  _stop = FALSE;
  
  loadIndicators();
  
  QStringList results;
  for (int pos = 0; pos < _symbols.size(); pos++)
  {
    emit signalProgress(pos);
    
    if (_stop)
      break;
    
    Object *symbol = loadBars(_symbols.at(pos));
    if (! symbol)
      continue;
    
    int andCount = 0;
    int andTotal = 0;
    bool orFlag = FALSE;

    for (int pos2 = 0; pos2 < _items.size(); pos2++)
    {
      IndicatorItem ii = _items.at(pos2);
      
      QStringList tl = _symbols.at(pos).split(":");
      QString sym = tl.join("_");

      ObjectCommand toc(QString("set_input"));
      toc.setValue(QString("symbol"), sym);
      toc.setValue(QString("length"), _length);
      toc.setValue(QString("input"), (void *) symbol);
      if (! ii.i->message(&toc))
      {
        qDebug() << "ScannerThread::run: message error" << ii.i->plugin() << toc.command();
        continue;
      }
      
      toc.setCommand(QString("get_object"));
      toc.setValue(QString("name"), ii.step);
      if (! ii.i->message(&toc))
      {
        qDebug() << "ScannerThread::run: message error" << ii.i->plugin() << toc.command();
        continue;
      }

      Object *step = (Object *) toc.getObject(QString("object"));
      if (! step)
      {
        qDebug() << "ScannerThread::run: invalid step object";
        continue;
      }
      
      toc.setCommand(QString("start_end_index"));
      if (! step->message(&toc))
      {
        qDebug() << "ScannerThread::run: message error" << step->plugin() << toc.command();
        continue;
      }
      
      int end = toc.getInt(QString("end"));
      if (! end)
      {
        qDebug() << "ScannerThread::run: 0 size" << _symbols.at(pos);
        continue;
      }
      
      toc.setCommand(QString("value"));
      toc.setValue(QString("index"), end);
      if (! step->message(&toc))
      {
        qDebug() << "ScannerThread::run: message error" << step->plugin() << toc.command();
        continue;
      }

      if (ii.usage == QString("AND"))
        andTotal++;
      
      double v = toc.getDouble(QString("value"));
//qDebug() << "ScannerThread::run: value" << step->plugin() << v;
      if (! v)
        continue;
      
      if (ii.usage == QString("OR"))
      {
        orFlag = TRUE;
        break;
      }
      else
        andCount++;
    }
    
    if (orFlag)
      results << _symbols.at(pos);
    else
    {
      if (andTotal == andCount)
        results << _symbols.at(pos);
    }
    
    delete symbol;
  }

  ObjectCommand toc(QString("results"));
  toc.setValue(QString("symbols"), results);
  emit signalMessage(toc);
  
  emit signalDone();
}

void
ScannerThread::stop ()
{
  _stop = TRUE;
}

Object *
ScannerThread::loadBars (QString tsymbol)
{
  Util util;
  Object *symbol = util.object(QString("Symbol"), QString(), QString());
  if (! symbol)
  {
    qDebug() << "ScannerThread::loadBars: invalid Symbol object";
    return 0;
  }
  
  QStringList tl = tsymbol.split(":");
  if (! tl.size() == 2)
  {
    qDebug() << "ScannerThread::loadBars: invalid current symbol" << tsymbol;
    delete symbol;
    return 0;
  }
  
  ObjectCommand toc(QString("get"));
  toc.setValue(QString("exchange"), tl.at(0));
  toc.setValue(QString("ticker"), tl.at(1));
  toc.setValue(QString("start_date"), _startDate);
  toc.setValue(QString("end_date"), _endDate);
  toc.setValue(QString("length"), _length);
  if (! symbol->message(&toc))
  {
    qDebug() << "ScannerThread::loadBars: message error" << symbol->plugin() << toc.command();
    delete symbol;
    return 0;
  }
  
  return symbol;
}

void
ScannerThread::loadIndicators ()
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Scanner") << QString("profile") << _profile << QString("indicator");
  QString path = tl.join("/");
  
  dir.setPath(path);
  tl = dir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::NoSort);
  
  Util util;
  for (int pos = 0; pos < tl.size(); pos++)
  {
    QString file = path + "/" + tl.at(pos);
    Object *i = util.object(QString("Indicator"), _profile, tl.at(pos));
    if (! i)
      continue;
    
    ObjectCommand toc(QString("load"));
    toc.setValue(QString("file"), file);
    if (! i->message(&toc))
    {
      delete i;
      continue;
    }
    
    IndicatorItem ii;
    ii.i = i;
    
    QSettings settings(file, QSettings::NativeFormat);
    ii.usage = settings.value(QString("usage")).toString();
    ii.step = settings.value(QString("step")).toString();

    if (ii.usage == QString("OR"))
      _items.prepend(ii);
    else
      _items << ii;
  }
}
