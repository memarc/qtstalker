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

#include "IndicatorAddStepDialog.h"

#include <QtDebug>

IndicatorAddStepDialog::IndicatorAddStepDialog (QStringList objects) : Dialog (0, QString())
{
  _objects = objects;
  
//  setWindowTitle(tr("OTA - Edit Indicator Add Step"));
  
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Indicator");
  tl << QString("settings") << QString("add_step_dialog");
  _settingsPath = tl.join("/");
  
  createTab();
  loadSettings();
}

IndicatorAddStepDialog::~IndicatorAddStepDialog ()
{
  saveSettings();
}

void
IndicatorAddStepDialog::createTab ()
{
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(10);
  w->setLayout(form);
  
  _name = new QLineEdit;
  connect(_name, SIGNAL(textEdited(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Name"), _name);

  _indicator = new QComboBox;
  connect(_indicator, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Indicator"), _indicator);
  
  _tabs->addTab(w, tr("Settings"));
}

void
IndicatorAddStepDialog::done ()
{
  if (_objects.indexOf(_name->text()) != -1)
  {
    QMessageBox msg;
    msg.setText(tr("Duplicate step name"));
    msg.setInformativeText("Replace step?");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg.setDefaultButton(QMessageBox::No);
    int ret = msg.exec();
    if (ret == QMessageBox::No)
      return;
  }
  
  emit signalDone((void *) this);
  Dialog::done();
}

void
IndicatorAddStepDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
IndicatorAddStepDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.setValue(QString("indicator"), _indicator->currentText());
  Dialog::saveSettings(settings);
}

void
IndicatorAddStepDialog::modified ()
{
  int count = 0;
  if (! _name->text().isEmpty())
    count++;
  
  if (! _indicator->currentText().isEmpty())
    count++;
  
  switch (count)
  {
    case 2:
      _okButton->setEnabled(TRUE);
      break;
    default:
      _okButton->setEnabled(FALSE);
      break;
  }
}

void
IndicatorAddStepDialog::setSettings (QStringList plugins)
{
  _indicator->addItems(plugins);

  QSettings settings(_settingsPath, QSettings::NativeFormat);
  QString ts = settings.value(QString("indicator")).toString();
  if (! ts.isEmpty())
  {
    int index = _indicator->findText(ts);
    if (index != -1)
      _indicator->setCurrentIndex(index);
  }
}

void
IndicatorAddStepDialog::settings (QString &name, QString &plugin)
{
  name = _name->text();
  plugin = _indicator->currentText();
}
