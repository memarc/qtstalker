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

#include "EditIndicator.h"
#include "Util.h"
#include "IndicatorPlotDialog.h"

#include <QtDebug>

EditIndicator::EditIndicator (bool flag)
{
  _newFlag = flag;
  _indicator = 0;

  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("IndicatorPlot") << QString("indicator");
  _path = tl.join("/");
}

EditIndicator::~EditIndicator ()
{
  if (_indicator)
    delete _indicator;
}

void
EditIndicator::quit ()
{
  deleteLater();
}

void
EditIndicator::run ()
{
  QTimer::singleShot(10, this, SLOT(start()));
}
  
void
EditIndicator::start ()
{
  IndicatorPlotDialog *dialog = new IndicatorPlotDialog();
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(start2(void *)));

  QStringList wt;
  wt << QString("OTA") << "-";

  if (_newFlag)
  {
    wt << tr("New Indicator");
    dialog->setName(QString());
    dialog->setModified(FALSE);
  }
  else
  {
    wt << tr("Edit Indicator");
    dialog->setIndicator();
    dialog->ignoreChanges();
  }

  dialog->setWindowTitle(wt.join(" "));
  dialog->show();
}

void
EditIndicator::start2 (void *dialog)
{
  IndicatorPlotDialog *d = (IndicatorPlotDialog *) dialog;

  QString path = _path + "/";

  if (_newFlag) // new
    path.append(d->name());
  else
    path.append(d->indicator());

  if (_newFlag)
  {
    QDir dir;
    if (dir.exists(path))
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

      dir.remove(path);
    }

    _name = d->name();
  }

  Util util;
  _indicator = util.object(QString("Indicator"), _profile, _name);
  if (! _indicator)
  {
    qDebug() << "EditIndicator::start2: invalid Indicator object";
    quit();
    return;
  }
  
  ObjectCommand toc(QString("load"));
  toc.setValue(QString("file"), path);
  if (! _indicator->message(&toc))
  {
    qDebug() << "EditIndicator::start2: message error" << _indicator->plugin() << toc.command();
    quit();
    return;
  }
  
  toc.setCommand(QString("dialog"));
  if (! _indicator->message(&toc))
  {
    qDebug() << "EditIndicator::start2: message error" << _indicator->plugin() << toc.command();
    quit();
    return;
  }
}

