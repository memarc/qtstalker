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

#include "AddIndicator.h"
#include "Util.h"
#include "IndicatorPlotDialog.h"

#include <QtDebug>
#include <QSettings>

AddIndicator::AddIndicator (QString profile)
{
  _profile = profile;
  _row = -1;

  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("IndicatorPlot") << QString("indicator");
  _path = tl.join("/");
}

AddIndicator::~AddIndicator ()
{
}

void
AddIndicator::quit ()
{
  deleteLater();
}

void
AddIndicator::run ()
{
  QTimer::singleShot(10, this, SLOT(start()));
}
  
void
AddIndicator::start ()
{
  QStringList wt;
  wt << QString("OTA") << "-" << tr("Add Indicator");

  IndicatorPlotDialog *dialog = new IndicatorPlotDialog;
  dialog->setWindowTitle(wt.join(" "));
  dialog->setIndicator();
  dialog->setName(QString());
  dialog->setRow(1);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(start2(void *)));
  dialog->setModified(FALSE);
  dialog->show();
}

void
AddIndicator::start2 (void *dialog)
{
  IndicatorPlotDialog *d = (IndicatorPlotDialog *) dialog;
  _name = d->name();
  _row = d->row();
  QString file = d->indicator();

  // copy from master indicator dir to profile directory
  QString ofile = _path + "/" + file;

  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("IndicatorPlot") << _profile << QString("indicator");
  QString nfile = tl.join("/");
  if (! dir.mkpath(nfile))
  {
    qDebug() << "AddIndicator::start2: path create error" << nfile;
    quit();
    return;
  }

  nfile.append("/" + _name);

  if (dir.exists(nfile))
  {
    QStringList wt;
    wt << QString("OTA") << "-" << tr("Duplicate Indicator");

    QMessageBox msgBox;
    msgBox.setWindowTitle(wt.join(" "));
    msgBox.setText("Indicator already exists.");
    msgBox.setInformativeText("Overwrite existing indicator ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if (msgBox.exec() == QMessageBox::No)
    {
      quit();
      return;
    }

    dir.remove(nfile);
  }

  if (! QFile::copy(ofile, nfile))
  {
    qDebug() << "AddIndicator::indicatorObjectMessage: copy file error" << ofile << nfile;
    quit();
    return;
  }

  emit signalDone(nfile, _name, _row);
}

