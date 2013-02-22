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

#include "CSVObject.h"
#include "CSVFormat.h"
#include "CSVWidget.h"
#include "Util.h"
#include "Delimiter.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMainWindow>


CSVObject::CSVObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("CSV");
  _type = QString("system");
  
  _commandList << QString("app");
  _commandList << QString("import");
}

CSVObject::~CSVObject ()
{
}

int
CSVObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0: // app
      rc = app(oc);
      break;
    case 1: // import
      rc = import(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
CSVObject::app (ObjectCommand *oc)
{
  QMainWindow *mw = (QMainWindow *) oc->getObject(QString("main_window"));
  if (! mw)
  {
    qDebug() << "CSVObject::app: invalid main_window";
    return 0;
  }

  new CSVWidget(mw, _name);
  
  return 1;
}

int
CSVObject::import (ObjectCommand *oc)
{
  // files
  QString key("files");
  QStringList files = oc->getList(key);

  // format
  key = QString("format");
  QString format = oc->getString(key);
  if (format.isEmpty())
  {
    qDebug() << "CSVObject::import: invalid" << key;
    return 0;
  }
  
  key = QString("date_format");
  QString dateFormat = oc->getString(key);
  if (dateFormat.isEmpty())
  {
    qDebug() << "CSVObject::import: invalid" << key;
    return 0;
  }
  
  key = QString("delimiter");
  QString delimiter = oc->getString(key);
  if (delimiter.isEmpty())
  {
    qDebug() << "CSVObject::import: invalid" << key;
    return 0;
  }
  
  key = QString("type");
  QString type = oc->getString(key);
  if (type.isEmpty())
  {
    qDebug() << "CSVObject::import: invalid" << key;
    return 0;
  }
  
  key = QString("exchange");
  QString exchange = oc->getString(key);
  if (exchange.isEmpty())
  {
    qDebug() << "CSVObject::import: invalid" << key;
    return 0;
  }
  
  bool useFilename = oc->getBool(QString("use_filename"));
  
  return import2(files, format, dateFormat, delimiter, type, exchange, useFilename);
}

int
CSVObject::import2 (QStringList files, QString tformat, QString dateFormat, QString tdelimiter,
                     QString type, QString exchange, bool useFilename)
{
  QStringList format = tformat.split(",", QString::SkipEmptyParts);
  
  Delimiter delimit;
  QString delimiter = delimit.stringToActual(tdelimiter);
  
  ObjectCommand moc(QString("status"));

  // away we go
  QHash<QString, ObjectCommand *> commands;
  CSVFormat typeFormat;
  int loop = 0;
  for (; loop < files.size(); loop++)
  {
    QFile f(files.at(loop));
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QStringList mess;
      mess << "\t" << tr("File error") << files.at(loop) << tr("skipped");
      moc.setValue(QString("info"), mess.join(" "));
      emit signalMessage(moc);
      continue;
    }

    QFileInfo fi(files.at(loop));
    QString fileNameSymbol;
    if (useFilename)
      fileNameSymbol = fi.baseName();

    int lineNum = 0;
    while (! f.atEnd())
    {
      QString s = f.readLine();
      lineNum++;
      s = s.trimmed();
      if (s.isEmpty())
        continue;

      QStringList data = s.split(delimiter, QString::SkipEmptyParts);

      // number of fields match format ?
      if (format.size() != data.size())
      {
        QStringList mess;
        mess << tr("invalid # of fields") << QString::number(data.size()) << tr("expected") << QString::number(format.size());
	errorMessage(fi.baseName(), lineNum, mess.join(" "));
        continue;
      }

      // put data into a bar
      Data bar;
      for (int pos = 0; pos < format.size(); pos++)
	bar.insert(format.at(pos), QVariant(data.at(pos).trimmed()));
      
      // overide symbol with fielname
      if (useFilename)
      {
	bar.insert(typeFormat.typeToString(CSVFormat::_TICKER), QVariant(fileNameSymbol));
	bar.insert(typeFormat.typeToString(CSVFormat::_NAME), QVariant(fileNameSymbol));
      }

      // set exchange
      if (! exchange.isEmpty())
	bar.insert(typeFormat.typeToString(CSVFormat::_EXCHANGE), QVariant(exchange));

      bool error = FALSE;
      QHashIterator<QString, QVariant> it(bar);
      while (it.hasNext())
      {
	it.next();
	QVariant v = it.value();
	
        switch ((CSVFormat::Key) typeFormat.stringToIndex(it.key()))
        {
          case CSVFormat::_DATE:
	  {
            QDateTime dt = QDateTime::fromString(v.toString(), dateFormat);
            if (! dt.isValid())
            {
              QStringList mess;
              mess << tr("invalid date") << v.toString();
	      errorMessage(fi.baseName(), lineNum, mess.join(" "));
	      error = TRUE;
            }
            break;
	  }
          case CSVFormat::_OPEN:
          case CSVFormat::_HIGH:
          case CSVFormat::_LOW:
          case CSVFormat::_CLOSE:
          case CSVFormat::_VOLUME:
          case CSVFormat::_OI:
	  {
            if (! v.canConvert(QVariant::Double))
            {
              QStringList mess;
              mess << tr("invalid") << it.key() << v.toString();
	      errorMessage(fi.baseName(), lineNum, mess.join(" "));
	      error = TRUE;
            }
            break;
	  }
          case CSVFormat::_TYPE:
          case CSVFormat::_EXCHANGE:
          case CSVFormat::_TICKER:
          case CSVFormat::_NAME:
          case CSVFormat::_IGNORE:
	    // ignore these as strings
	    break;
          default:
          {
            QStringList mess;
            mess << tr("invalid format parm") << it.key();
            errorMessage(fi.baseName(), lineNum, mess.join(" "));
            error = TRUE;
            break;
          }
        }

        if (error)
          break;
      }

      if (error)
	continue;
      
      // verify date
      if (! bar.contains(typeFormat.typeToString(CSVFormat::_DATE)))
      {
        QStringList mess;
        mess << tr("date missing");
        errorMessage(fi.baseName(), lineNum, mess.join(" "));
        continue;
      }
      
      // verify exchange
      if (! bar.contains(typeFormat.typeToString(CSVFormat::_EXCHANGE)))
      {
        QStringList mess;
        mess << tr("exchange missing");
        errorMessage(fi.baseName(), lineNum, mess.join(" "));
        continue;
      }

      // verify ticker
      if (! bar.contains(typeFormat.typeToString(CSVFormat::_TICKER)))
      {
        QStringList mess;
        mess << tr("ticker missing");
        errorMessage(fi.baseName(), lineNum, mess.join(" "));
        continue;
      }

      // put bar into command struct
      QString key = bar.value(typeFormat.typeToString(CSVFormat::_EXCHANGE)).toString() +
                    QString(":") +
		    bar.value(typeFormat.typeToString(CSVFormat::_TICKER)).toString();
      ObjectCommand *oc = commands.value(key);
      if (! oc)
      {
	oc = new ObjectCommand(QString("set"));
	oc->setValue(QString("exchange"), bar.value(typeFormat.typeToString(CSVFormat::_EXCHANGE)).toString());
	oc->setValue(QString("ticker"), bar.value(typeFormat.typeToString(CSVFormat::_TICKER)).toString());
	oc->setValue(QString("type"), type);
	oc->setValue(QString("name"), bar.value(typeFormat.typeToString(CSVFormat::_NAME)).toString());
	commands.insert(key, oc);
      }

      // remove data from bar
      bar.remove(typeFormat.typeToString(CSVFormat::_EXCHANGE));
      bar.remove(typeFormat.typeToString(CSVFormat::_TICKER));
      bar.remove(typeFormat.typeToString(CSVFormat::_NAME));
      bar.remove(typeFormat.typeToString(CSVFormat::_IGNORE));
      QDateTime dt = bar.value(typeFormat.typeToString(CSVFormat::_DATE)).toDateTime();
      bar.remove(typeFormat.typeToString(CSVFormat::_DATE));
      oc->setValue(dt.toString("yyyyMMddHHmmss"), bar);
    }

    f.close();
  }

  Util fac;
  Object *db = fac.object(QString("Symbol"), QString(), QString());
  if (! db)
  {
    QStringList mess;
    mess << "\t" << tr("invalid Symbol object");
    moc.setValue(QString("info"), mess.join(" "));
    emit signalMessage(moc);
    qDeleteAll(commands);
    return 0;
  }
  
  QHashIterator<QString, ObjectCommand *> it(commands);
  while (it.hasNext())
  {
    it.next();
    ObjectCommand *oc = it.value();
    if (! db->message(oc))
    {
      QStringList mess;
      mess << "\t" << tr("error saving quotes");
      moc.setValue(QString("info"), mess.join(" "));
      emit signalMessage(moc);
    }
  }
  
  delete db;
  qDeleteAll(commands);
  
  return 1;
}

void
CSVObject::errorMessage (QString file, int line, QString mess)
{
  ObjectCommand moc(QString("status"));
  
  QStringList tl;
  tl << "\t";
  tl << tr("Error");
  tl << file;
  tl << tr("line");
  tl << QString::number(line);
  tl << mess;
        
  moc.setValue(QString("info"), tl.join(" "));
  
  emit signalMessage(moc);
}
