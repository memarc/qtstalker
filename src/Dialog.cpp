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

#include "Dialog.h"
#include "Object.h"

#include <QtDebug>
#include <QSettings>

Dialog::Dialog (QWidget *p, QString name) : QDialog (p)
{
  _helpFile = "main.html";
  _name = name;
  _ignoreChanges = FALSE;

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
  
  QStringList tl;
  tl << QString("OTA") << QString("-") << name;
  setWindowTitle(tl.join(" "));

  createGUI();
}

Dialog::~Dialog ()
{
}

void
Dialog::createGUI ()
{
  // main vbox
  _vbox = new QVBoxLayout;
  _vbox->setSpacing(5);
  _vbox->setMargin(10);
  setLayout(_vbox);

  // tabs
  _tabs = new QTabWidget;
  _vbox->addWidget(_tabs);

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  _vbox->addWidget(_buttonBox);

  // ok button
  _okButton = _buttonBox->addButton(QDialogButtonBox::Ok);
  _okButton->setText(tr("&OK"));
  _okButton->setEnabled(FALSE);

  // cancel button
  _cancelButton = _buttonBox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setText(tr("&Cancel"));
  _cancelButton->setDefault(TRUE);
  _cancelButton->setFocus();

  // help button
  QPushButton *b = _buttonBox->button(QDialogButtonBox::Help);
  b->setText(tr("&Help"));
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void
Dialog::ignoreChanges ()
{
  _ignoreChanges = TRUE;
  _okButton->setEnabled(TRUE);
}

void
Dialog::done ()
{
  accept();
}

void
Dialog::cancel ()
{
  if (! _ignoreChanges)
  {
    if (_okButton->isEnabled())
    {
      QMessageBox msgBox;
      msgBox.setWindowTitle(windowTitle());
      msgBox.setText(tr("Settings have been modified"));
      msgBox.setInformativeText(tr("Are you sure you want to discard changes ?"));
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgBox.setDefaultButton(QMessageBox::No);
      if (msgBox.exec() == QMessageBox::No)
        return;
    }
  }

  emit signalCancel();
  
  reject();
}

void
Dialog::help ()
{
}

void
Dialog::setModified (bool d)
{
  _okButton->setEnabled(d);
}

void
Dialog::loadSettings (QSettings &settings)
{
  QSize sz = settings.value(QString("size"), QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value(QString("pos")).toPoint();
  if (! p.isNull())
    move(p);
}

void
Dialog::saveSettings (QSettings &settings)
{
  settings.setValue(QString("size"), size());
  settings.setValue(QString("pos"), pos());
}

void
Dialog::modified ()
{
  _okButton->setEnabled(TRUE);
}

