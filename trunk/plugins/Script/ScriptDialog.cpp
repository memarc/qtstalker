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

#include "ScriptDialog.h"
#include "Doc.h"
#include "Global.h"
#include "ScriptType.h"

#include <QtDebug>
#include <QVariant>
#include <QLayout>
#include <QFormLayout>


ScriptDialog::ScriptDialog (QWidget *p) : QDialog (p)
{
  _settings = 0;
  _modified = 0;
  _helpFile = "main.html";
  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

  // main vbox
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(5);
  setLayout(vbox);
  
  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(_buttonBox);

  // ok button
  _okButton = _buttonBox->addButton(QDialogButtonBox::Ok);
  _okButton->setText(tr("&OK"));

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

void ScriptDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void ScriptDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void ScriptDialog::cancel ()
{
  saveSettings();
  reject();
}

void ScriptDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("script_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("script_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void ScriptDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("script_dialog_size", size());
  settings.setValue("script_dialog_pos", pos());
}

void ScriptDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // Settings page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);
  
  // command
  QVariant *set = _settings->get(QString("command"));
  
  _command = new QLineEdit(set->toString());
  connect(_command, SIGNAL(textChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Command"), _command);
  
  // file
  set = _settings->get(QString("file"));
  
  _file = new FileButton(this);
  _file->setFiles(QStringList() << set->toString());
  connect(_file, SIGNAL(signalSelectionChanged(QStringList)), this, SLOT(modified()));
  form->addRow(tr("File"), _file);
  
  // type
  set = _settings->get(QString("scriptType"));

  ScriptType st;
  _type = new QComboBox;
  _type->addItems(st.list());
  _type->setCurrentIndex(_type->findText(set->toString(), Qt::MatchExactly));
  connect(_type, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Type"), _type);

  _tabs->addTab(w, tr("Settings"));
}

void ScriptDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("command"));
  set->setValue(_command->text());

  set = _settings->get(QString("file"));
  QStringList l = _file->files();
  QString s;
  if (l.size())
    s = l.at(0);
  set->setValue(s);

  set = _settings->get(QString("scriptType"));
  set->setValue(_type->currentText());
}

void ScriptDialog::modified ()
{
  _modified = 1;
}
