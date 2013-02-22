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

#include "PlotDialog.h"
#include "Util.h"

PlotDialog::PlotDialog (QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Plot") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createTab();
  loadSettings();
}

PlotDialog::~PlotDialog ()
{
  saveSettings();
}

void
PlotDialog::createTab ()
{
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(10);
  w->setLayout(form);

  // row
  _row = new QSpinBox;
  _row->setRange(1, 10);
  connect(_row, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Row"), _row);

  // col
  _col = new QSpinBox;
  _col->setRange(1, 10);
  connect(_col, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Column"), _col);

  _tabs->addTab(w, tr("Settings"));
}

void
PlotDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
PlotDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
PlotDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
PlotDialog::setSettings (int row, int col)
{
  _row->setValue(row);
  _col->setValue(col);
}

void
PlotDialog::settings (int &row, int &col)
{
  row = _row->value();
  col = _col->value();
}
