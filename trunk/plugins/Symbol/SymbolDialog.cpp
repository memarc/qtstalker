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

#include "SymbolDialog.h"
#include "Util.h"
#include "Object.h"

SymbolDialog::SymbolDialog (QString name) : Dialog (0, name)
{
  QStringList tl;
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("Symbol") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createSymbolTab();
  loadSettings();
}

SymbolDialog::~SymbolDialog ()
{
  saveSettings();
}

void
SymbolDialog::createSymbolTab ()
{
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(10);
  w->setLayout(form);
  
  // exchange
  _exchange = new QLineEdit;
  connect(_exchange, SIGNAL(textEdited(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Exchange"), _exchange);
  
  // ticker
  _ticker = new QLineEdit;
  connect(_ticker, SIGNAL(textEdited(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Ticker"), _ticker);

  _tabs->addTab(w, tr("Settings"));
}

void
SymbolDialog::done ()
{
  emit signalDone((void *) this);
  Dialog::done();
}

void
SymbolDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
SymbolDialog::saveSettings()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
SymbolDialog::setSettings (QString e, QString t)
{
  _exchange->setText(e);
  _ticker->setText(t);
}

void
SymbolDialog::settings (QString &e, QString &t)
{
  e = _exchange->text();
  t = _ticker->text();
}
