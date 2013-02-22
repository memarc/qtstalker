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

#include "CSVWidget.h"
#include "CSVThread.h"
#include "Delimiter.h"
#include "Quote.h"
#include "Util.h"
#include "../../pics/insert.xpm"
#include "../../pics/stop.xpm"
#include "../../pics/help.xpm"
#include "../../pics/quit.xpm"

#include <QtDebug>
#include <QSettings>


CSVWidget::CSVWidget (QMainWindow *mw, QString profile)
{
  _helpFile = "main.html";
  _cancel = FALSE;
  _mw = mw;
  _profile = profile;
  
  createActions();
  createGUI();

  QStringList tl;
  tl << QString("OTA -") << QString("CSV") << QString("(") << _profile << QString(")");
  _mw->setWindowTitle(tl.join(" "));

  tl.clear();
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("CSV") << QString("profile") << _profile;
  _dbPath = tl.join("/");

  loadSettings();
}

CSVWidget::~CSVWidget ()
{
  saveSettings();
}

void
CSVWidget::createActions ()
{
  QAction *a = new QAction(QIcon(insert_xpm), tr("Import CSV Quotes"), this);
  a->setToolTip(tr("Import CSV Quotes"));
  a->setStatusTip(tr("Import CSV Quotes"));
  connect(a, SIGNAL(triggered()), this, SLOT(importThread()));
  _actions.insert(_IMPORT, a);
  
  a = new QAction(QIcon(stop_xpm), tr("Stop Import"), this);
  a->setToolTip(tr("Stop Import"));
  a->setStatusTip(tr("Stop Import"));
  a->setEnabled(FALSE);
  connect(a, SIGNAL(triggered()), this, SIGNAL(signalStop()));
  _actions.insert(_STOP, a);
  
  a = new QAction(QIcon(help_xpm), tr("Help"), this);
  a->setToolTip(tr("Help"));
  a->setStatusTip(tr("Help"));
  connect(a, SIGNAL(triggered()), this, SLOT(help()));
  _actions.insert(_HELP, a);
  
  a = new QAction(QIcon(quit_xpm), tr("Quit"), this);
  a->setToolTip(tr("Quit"));
  a->setStatusTip(tr("Quit"));
  connect(a, SIGNAL(triggered()), qApp, SLOT(quit()));
  _actions.insert(_QUIT, a);
}

void
CSVWidget::createGUI ()
{
  _toolBar = _mw->addToolBar(QString("main"));
  _toolBar->addAction(_actions.value(_IMPORT));
  _toolBar->addAction(_actions.value(_STOP));
  _toolBar->addSeparator();
  _toolBar->addAction(_actions.value(_HELP));
  _toolBar->addSeparator();
  _toolBar->addAction(_actions.value(_QUIT));
  
  _mw->setCentralWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(5);
  setLayout(vbox);
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(0);
  vbox->addLayout(form);
  
  // csv file
  _csvButton = new FileButton(0);
  connect(_csvButton, SIGNAL(valueChanged()), this, SLOT(buttonStatus()));
  form->addRow (tr("CSV Files"), _csvButton);

  // format
  _format = new QLineEdit;
  form->addRow (tr("Format"), _format);
  
  // date format
  _dateFormat = new QLineEdit;
  form->addRow (tr("Date Format"), _dateFormat);
  
  // delimiter
  Delimiter d;
  _delimiter = new QComboBox;
  _delimiter->addItems(d.list());
  _delimiter->setCurrentIndex(Delimiter::_SEMICOLON);
  form->addRow (tr("Delimiter"), _delimiter);
  
  // type
  Quote q;
  _type = new QComboBox;
  _type->addItems(q.list());
  _type->setCurrentIndex(Quote::_STOCK);
  form->addRow (tr("Quote Type"), _type);
  
  // exchange
  _exchange = new QLineEdit;
  form->addRow (tr("Exchange Override"), _exchange);
  
  // filename as symbol
  _filename = new QCheckBox;
  form->addRow (tr("Use Filename As Ticker"), _filename);

  // log
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Log"));
  vbox->addWidget(gbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  gbox->setLayout(tvbox);
  
  _log = new QTextEdit;
  _log->setReadOnly(TRUE);
  tvbox->addWidget(_log);

  _statusBar = _mw->statusBar();
  _statusBar->showMessage(tr("Ready"), 2000);

  // progress bar  
  _progBar = new QProgressBar;
  _statusBar->addPermanentWidget(_progBar, 0);
  
  _mw->setUnifiedTitleAndToolBarOnMac(TRUE);
}

void
CSVWidget::importStart ()
{
  _log->clear();
  setEnabled(FALSE);
  
  QStringList mess;
  mess << tr("Import started") << QDateTime::currentDateTime().toString(Qt::ISODate);
  _log->append(mess.join(" "));
}

void
CSVWidget::importDone ()
{
  setEnabled(TRUE);
  
  QStringList mess;
  mess << tr("Import completed") << QDateTime::currentDateTime().toString(Qt::ISODate);
  _log->append(mess.join(" "));
}

void
CSVWidget::importThread ()
{
  importStart();

  CSVThread *thread = new CSVThread(0,
				    _csvButton->files(),
                                    _format->text(),
                                    _dateFormat->text(),
                                    _delimiter->currentText(),
                                    _type->currentText(),
                                    _exchange->text(),
                                    _filename->isChecked());
  connect(thread, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(threadMessage(ObjectCommand)));
  connect(this, SIGNAL(signalStop()), thread, SLOT(stop()));
  connect(thread, SIGNAL(signalDone()), this, SLOT(importDone()));
  thread->start();  
}

void
CSVWidget::buttonStatus ()
{
  int count = 0;

  QStringList tl = _csvButton->files();
  if (tl.size())
    count++;
  
  if (! _format->text().isEmpty())
    count++;
  
  if (! _dateFormat->text().isEmpty())
    count++;
  
  switch (count)
  {
    case 3:
      _actions.value(_IMPORT)->setEnabled(TRUE);
      break;
    default:
      _actions.value(_IMPORT)->setEnabled(FALSE);
      break;
  }
}

void
CSVWidget::loadSettings ()
{
  QSettings settings(_dbPath, QSettings::NativeFormat);
  
  QSize sz = settings.value(QString("size"), QSize(500,300)).toSize();
  _mw->resize(sz);

  QPoint p = settings.value(QString("pos"), QPoint(0, 0)).toPoint();
  _mw->move(p);
  
  _csvButton->setFiles(settings.value(QString("files")).toStringList());
  _format->setText(settings.value(QString("format"), "X,S,N,D,O,H,L,C,V").toString());
  _dateFormat->setText(settings.value(QString("dateFormat"), "yyyy-MM-dd").toString());
  _delimiter->setCurrentIndex(settings.value(QString("delimiter"), 1).toInt());
  _type->setCurrentIndex(settings.value(QString("type"), 0).toInt());
  _exchange->setText(settings.value(QString("exchange")).toString());
  _filename->setChecked(settings.value(QString("useFilename")).toBool());
  
  buttonStatus();
}

void
CSVWidget::saveSettings ()
{
  QSettings settings(_dbPath, QSettings::NativeFormat);
  
  settings.setValue(QString("size"), _mw->size());
  settings.setValue(QString("pos"), _mw->pos());
  settings.setValue(QString("format"), _format->text());
  settings.setValue(QString("dateFormat"), _dateFormat->text());
  settings.setValue(QString("delimiter"), _delimiter->currentIndex());
  settings.setValue(QString("type"), _type->currentIndex());
  settings.setValue(QString("exchange"), _exchange->text());
  settings.setValue(QString("useFilename"), _filename->isChecked());
  settings.setValue(QString("files"), _csvButton->files());
}

void
CSVWidget::help ()
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("bin") << QString("OTA -a Help");
  
  if (! QProcess::startDetached(tl.join(" ")))
    qDebug() << "CSVWidget::help: error launching process" << tl;
}

void
CSVWidget::threadMessage (ObjectCommand d)
{
  _log->append(d.getString(QString("info")));
}
