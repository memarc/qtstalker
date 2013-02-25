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

#include "FileButtonWidget.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>

FileButtonWidget::FileButtonWidget (QWidget *w) : QPushButton (w)
{
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(fileDialog()));
  _path = QDir::homePath();
  updateButtonText();
}

QStringList FileButtonWidget::files ()
{
  return _files;
}

void FileButtonWidget::setFiles (QStringList l)
{
  _files = l;
  updateButtonText();
}

void FileButtonWidget::setPath (QString d)
{
  _path = d;
  updateButtonText();
}

void FileButtonWidget::fileDialog ()
{
  if (_files.count())
  {
    QFileInfo fi(_files.at(0));
    _path = fi.absolutePath();
  }

  QFileDialog *dialog = new QFileDialog(this);
  dialog->setFileMode(QFileDialog::AnyFile);
  dialog->setDirectory(_path);
  dialog->setWindowTitle(tr("Select Files"));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  connect(dialog, SIGNAL(filesSelected(const QStringList &)), this, SLOT(setFiles(QStringList)));
  dialog->show();
}

void FileButtonWidget::updateButtonText ()
{
  QStringList l;
  
  if (_files.count() == 1)
    l << _files.at(0);
  else
  {
    l << QString::number(_files.count());
    l << tr("Files");
  }
  
  setText(l.join(" "));

  emit signalSelectionChanged();
}
