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
#include <QSettings>

#include "YahooHistoryWidget.h"
#include "YahooHistoryObject.h"
#include "YahooHistoryThread.h"

#include "../../pics/download.xpm"
#include "../../pics/stop.xpm"
#include "../../pics/help.xpm"
#include "../../pics/quit.xpm"


YahooHistoryWidget::YahooHistoryWidget (QMainWindow *mw, QString profile)
{
  _helpFile = "main.html";
  _mw = mw;
  _profile = profile;
  _downloading = FALSE;
  
  createActions();
  createGUI();

  QStringList tl;
  tl << QString("OTA -") << QString("Yahoo") << tr("History") << QString("(") << _profile << QString(")");
  _mw->setWindowTitle(tl.join(" "));

  tl.clear();
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("YahooHistory") << QString("profile") << _profile;
  _dbPath = tl.join("/");

  loadSettings();
  
  buttonStatus();
}

YahooHistoryWidget::~YahooHistoryWidget ()
{
  saveSettings();
}

void
YahooHistoryWidget::createActions ()
{
  QAction *a = new QAction(QIcon(download_xpm), tr("Download Quotes"), this);
  a->setToolTip(tr("Download Quotes"));
  a->setStatusTip(tr("Download Quotes"));
  connect(a, SIGNAL(triggered()), this, SLOT(downloadHistory()));
  _actions.insert(_DOWNLOAD, a);
  
  a = new QAction(QIcon(stop_xpm), tr("Stop Download"), this);
  a->setToolTip(tr("Stop Download"));
  a->setStatusTip(tr("Stop Download"));
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
YahooHistoryWidget::createGUI ()
{
  _toolBar = _mw->addToolBar(QString("main"));
  _toolBar->addAction(_actions.value(_DOWNLOAD));
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
  form->setMargin(5);
  vbox->addLayout(form);
  
  // start date
  QDateTime dt = QDateTime::currentDateTime();
  dt = dt.addYears(-1);
  
  _startDate = new QDateTimeEdit;
  _startDate->setDateTime(dt);
  connect(_startDate, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(buttonStatus()));
  form->addRow (tr("Start Date"), _startDate);

  // end date
  _endDate = new QDateTimeEdit;
  _endDate->setDateTime(QDateTime::currentDateTime());
  connect(_endDate, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(buttonStatus()));
  form->addRow (tr("End Date"), _endDate);
  
  // symbol file
  _symbolButton = new FileButton(0);
  connect(_symbolButton, SIGNAL(signalSelectionChanged()), this, SLOT(buttonStatus()));
  form->addRow (tr("Symbol File"), _symbolButton);
  
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
YahooHistoryWidget::loadSettings ()
{
  QSettings settings(_dbPath, QSettings::NativeFormat);
  
  // app size;
  QSize sz = settings.value(QString("size"), QSize(500,300)).toSize();
  _mw->resize(sz);

  // app position
  QPoint p = settings.value(QString("pos"), QPoint(0, 0)).toPoint();
  _mw->move(p);
  
  // start date
  QDateTime sd = settings.value(QString("start_date")).toDateTime();
  if (! sd.isValid())
  {
    sd = QDateTime::currentDateTime();
    sd = sd.addYears(-1);
  }
  _startDate->setDateTime(sd);

  // symbol files
  _symbolButton->setFiles(settings.value(QString("symbol_files")).toStringList());
}

void
YahooHistoryWidget::saveSettings()
{
  QSettings settings(_dbPath, QSettings::NativeFormat);
  settings.setValue(QString("size"), _mw->size());
  settings.setValue(QString("pos"), _mw->pos());
  settings.setValue(QString("start_date"), _startDate->dateTime());
  settings.setValue(QString("symbol_files"), _symbolButton->files());
}

void
YahooHistoryWidget::help ()
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("bin") << QString("OTA -a Help");
  
  if (! QProcess::startDetached(tl.join(" ")))
    qDebug() << "OTA::help: error launching process" << tl;
}

void
YahooHistoryWidget::downloadHistory ()
{
  _log->clear();
  setEnabled(FALSE);
  _downloading = TRUE;
  buttonStatus();

  // parse symbol files
  QStringList tl = _symbolButton->files();
  QStringList symbols;
  for (int pos = 0; pos < tl.size(); pos++)
  {
    QFile f(tl.at(pos));
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QStringList mess;
      mess << tr("Error opening file") << tl.at(pos) << tr("skipped");
      _log->append(mess.join(" "));
      continue;
    }

    while (! f.atEnd())
    {
      QString symbol = f.readLine().trimmed();
      if (symbol.isEmpty())
        continue;
      symbols << symbol;
    }
    
    f.close();
  }

  _progBar->setRange(0, symbols.size());
  
  YahooHistoryThread *thread = new YahooHistoryThread(0, symbols, _startDate->dateTime(), _endDate->dateTime());
  connect(thread, SIGNAL(signalMessage(ObjectCommand)), this, SLOT(threadMessage(ObjectCommand)));
  connect(this, SIGNAL(signalStop()), thread, SLOT(stop()));
  connect(thread, SIGNAL(signalDone()), this, SLOT(downloadDone()));
  connect(thread, SIGNAL(signalProgress(int)), _progBar, SLOT(setValue(int)));
  thread->start();  
}

void
YahooHistoryWidget::buttonStatus ()
{
  if (_downloading)
  {
    _actions.value(_DOWNLOAD)->setEnabled(FALSE);
    _actions.value(_STOP)->setEnabled(TRUE);
    _actions.value(_HELP)->setEnabled(FALSE);
    _actions.value(_QUIT)->setEnabled(FALSE);
    return;
  }
  else
  {
    _actions.value(_STOP)->setEnabled(FALSE);
    _actions.value(_HELP)->setEnabled(TRUE);
    _actions.value(_QUIT)->setEnabled(TRUE);
  }
  
  int count = 0;

  QStringList tl = _symbolButton->files();
  if (tl.size())
    count++;
//qDebug() << "YahooHistoryWidget::buttonStatus: files=" << tl.size();
  
  switch (count)
  {
    case 1:
      _actions.value(_DOWNLOAD)->setEnabled(TRUE);
      break;
    default:
      _actions.value(_DOWNLOAD)->setEnabled(FALSE);
      break;
  }
}

void
YahooHistoryWidget::downloadDone ()
{
  _progBar->reset();
  setEnabled(TRUE);
  _downloading = FALSE;
  buttonStatus();
}

void
YahooHistoryWidget::threadMessage (ObjectCommand d)
{
  _log->append(d.getString(QString("info")));
}
