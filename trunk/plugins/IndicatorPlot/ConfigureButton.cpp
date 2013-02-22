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

#include "ConfigureButton.h"
#include "IndicatorPlotDialog.h"
#include "Util.h"
#include "EditIndicator.h"

//#include "../pics/about.xpm"
//#include "../pics/help.xpm"
#include "../pics/configure.xpm"
#include "../pics/quit.xpm"
#include "../pics/add.xpm"
#include "../pics/new.xpm"
#include "../pics/edit.xpm"

#include <QDebug>
#include <QString>
#include <QAction>

ConfigureButton::ConfigureButton ()
{
  createMenu();

  setPopupMode(QToolButton::InstantPopup);
  setIcon(QIcon(configure_xpm));
  setText(tr("Configure / Options"));
  setStatusTip(tr("Configure and options"));
  setToolTip(tr("Configure and options"));
}

ConfigureButton::~ConfigureButton ()
{
}

void
ConfigureButton::createMenu ()
{
  _menu = new QMenu(this);
  _menu->setTitle(tr("Configure / Options"));
  setMenu(_menu);

  // new global indicator
  QAction *a = _menu->addAction(tr("&New Global Indicator") + "...");
  a->setShortcut(QKeySequence(QKeySequence::New));
  a->setIcon(QIcon(new_xpm));
  a->setToolTip(tr("New Global Indicator") + "...");
  a->setStatusTip(tr("New Global Indicator") + "...");
  connect(a, SIGNAL(triggered(bool)), this, SLOT(newIndicator()));

  // edit global indicator
  a = _menu->addAction(tr("&Edit Global Indicator") + "...");
  a->setShortcut(QKeySequence(QKeySequence::New));
  a->setIcon(QIcon(edit_xpm));
  a->setToolTip(tr("Edit Global Indicator") + "...");
  a->setStatusTip(tr("Edit Global Indicator") + "...");
  connect(a, SIGNAL(triggered(bool)), this, SLOT(editIndicator()));

  // add indicator
  a = _menu->addAction(tr("&Add Indicator") + "...");
  a->setShortcut(QKeySequence(QKeySequence::New));
  a->setIcon(QIcon(add_xpm));
  a->setToolTip(tr("Add Indicator") + "...");
  a->setStatusTip(tr("Add Indicator") + "...");
  connect(a, SIGNAL(triggered(bool)), this, SLOT(addIndicator()));

  _menu->addSeparator();

/*  
  // configure
  a = _menu->addAction(tr("&Configure") + "...");
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
  a->setCheckable(FALSE);
  a->setIcon(QIcon(configure_xpm));
  a->setStatusTip(tr("Configure settings") + "...");
  a->setToolTip(tr("Configure settings") + "...");
  connect(a, SIGNAL(triggered(bool)), this, SLOT(configureDialog()));

  _menu->addSeparator();
*/

/*
  // help dialog
  a = _menu->addAction(tr("&Help") + "...");
  a->setShortcut(QKeySequence(QKeySequence::HelpContents));
  a->setCheckable(FALSE);
  a->setIcon(QIcon(help_xpm));
  a->setStatusTip(tr("QtStalker help documentation") + "...");
  a->setToolTip(tr("QtStalker help documentation") + "...");
  connect(a, SIGNAL(triggered(bool)), this, SLOT(startDocumentation()));
*/

/*
  // about dialog
  a = _menu->addAction(tr("&About") + "...");
  a->setCheckable(FALSE);
  a->setIcon(QIcon(about_xpm));
  a->setStatusTip(tr("About QtStalker") + "...");
  a->setToolTip(tr("About QtStalker") + "...");
  connect(a, SIGNAL(triggered(bool)), this, SLOT(aboutDialog()));

  _menu->addSeparator();
*/

  // quit
  a = _menu->addAction(tr("&Quit"));
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
  a->setCheckable(FALSE);
  a->setIcon(QIcon(quit_xpm));
  a->setStatusTip(tr("Quit QtStalker"));
  a->setToolTip(tr("Quit QtStalker"));
  connect(a, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
}

void
ConfigureButton::startDocumentation ()
{
//  Doc *doc = new Doc;
//  doc->showDocumentation("index.html");
}

void
ConfigureButton::aboutDialog ()
{
//  AboutDialog *dialog = new AboutDialog;
//  dialog->show();
}

void
ConfigureButton::configureDialog ()
{
//  ConfigureDialog *dialog = new ConfigureDialog(this);
//  dialog->show();
}

void
ConfigureButton::newIndicator ()
{
  EditIndicator *ei = new EditIndicator(TRUE);
  ei->run();
}

void
ConfigureButton::addIndicator ()
{
  emit signalAddIndicator();
}

void
ConfigureButton::editIndicator ()
{
  EditIndicator *ei = new EditIndicator(FALSE);
  ei->run();
}

