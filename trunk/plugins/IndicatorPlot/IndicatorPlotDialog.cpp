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

#include "IndicatorPlotDialog.h"
#include "Util.h"

#include <QtDebug>

IndicatorPlotDialog::IndicatorPlotDialog () : Dialog (0, QString())
{
  _indicator = 0;
  _name = 0;
  _row = 0;

  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("IndicatorPlot");
  tl << QString("settings") << QString("indicator_plot_dialog");
  _settingsPath = tl.join("/");

  tl.clear();
  tl << dir.absolutePath() << QString("OTA") << QString("IndicatorPlot") << QString("indicator");
  _indicatorPath = tl.join("/");
  
  createTab();
  
  loadSettings();
}

IndicatorPlotDialog::~IndicatorPlotDialog ()
{
  saveSettings();
}

void
IndicatorPlotDialog::createTab ()
{
  QWidget *w = new QWidget;
  
  _form = new QFormLayout;
  _form->setSpacing(2);
  _form->setMargin(10);
  w->setLayout(_form);

  _tabs->addTab(w, tr("Settings"));
}

void
IndicatorPlotDialog::setIndicator ()
{
  QDir dir(_indicatorPath);
  QStringList fl = dir.entryList(QStringList(), QDir::Files | QDir::NoDotAndDotDot, QDir::NoSort);

  _indicator = new QComboBox;
  _indicator->addItems(fl);
  connect(_indicator, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  _form->addRow(tr("Indicator"), _indicator);
}

QString
IndicatorPlotDialog::indicator ()
{
  if (! _indicator)
    return QString();

  return _indicator->currentText();
}

void
IndicatorPlotDialog::setName (QString d)
{
  _name = new QLineEdit(tr("New Indicator"));
  connect(_name, SIGNAL(textChanged(const QString &)), this, SLOT(modified()));
  _form->addRow(tr("Name"), _name);

  if (d.isEmpty())
    return;

  _name->setText(d);
}

QString
IndicatorPlotDialog::name ()
{
  if (! _name)
    return QString();

  return _name->text();
}

void
IndicatorPlotDialog::setRow (int d)
{
  _row = new QSpinBox;
  _row->setRange(1, 10);
  _row->setValue(d);
  connect(_row, SIGNAL(valueChanged(int)), this, SLOT(modified()));
  _form->addRow(tr("Tab Row"), _row);
}

int
IndicatorPlotDialog::row ()
{
  if (! _row)
    return 0;

  return _row->value();
}

void
IndicatorPlotDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
IndicatorPlotDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
IndicatorPlotDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

