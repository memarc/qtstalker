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

#include "ValuesDialogDialog.h"
#include "Util.h"

#include <QDebug>

ValuesDialogDialog::ValuesDialogDialog (QString profile, QString name) : Dialog (0, QString())
{
  QStringList tl;
  tl << QString("OTA") << "(" << profile << ")" << "-" << name << tr("Values");
  setWindowTitle(tl.join(" "));

  tl.clear();
  QDir dir(QDir::homePath());
  tl << dir.absolutePath() << QString("OTA") << QString("ValuesDialog") << QString("settings") << QString("dialog");
  _settingsPath = tl.join("/");
  
  createTab();
  loadSettings();

  _cancelButton->hide();
}

ValuesDialogDialog::~ValuesDialogDialog ()
{
  saveSettings();
}

void
ValuesDialogDialog::createTab ()
{
  QWidget *w = new QWidget;
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(10);
  w->setLayout(vbox);

  _tree = new QTreeWidget;
  _tree->setItemsExpandable(FALSE);
  _tree->setRootIsDecorated(FALSE);
  _tree->setUniformRowHeights(TRUE);
  _tree->setSortingEnabled(FALSE);
  vbox->addWidget(_tree);

  _tabs->addTab(w, tr("Values"));
}

void
ValuesDialogDialog::done ()
{
  Dialog::done();
}

void
ValuesDialogDialog::loadSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::loadSettings(settings);
}

void
ValuesDialogDialog::saveSettings ()
{
  QSettings settings(_settingsPath, QSettings::NativeFormat);
  Dialog::saveSettings(settings);
}

void
ValuesDialogDialog::setData (QStringList &header, QMap<int, Data *> &map)
{
  QStringList tl = header;
  tl.prepend(tr("#"));
  _tree->setHeaderLabels(tl);

  Util util;
  QTreeWidgetItem *i = 0;
  QMapIterator<int, Data *> it(map);
  while (it.hasNext())
  {
    it.next();
    Data *d = it.value();

    i = new QTreeWidgetItem(_tree);
    i->setText(0, QString::number(it.key() + 1));

    for (int pos = 0; pos < header.size(); pos++)
    {
      QVariant v = d->value(header.at(pos));
      if (v.isNull())
        continue;

      QString ts;
      util.strip(v.toDouble(), 4, ts);
      i->setText(pos + 1, ts);
    }
  }

  for (int pos = 0; pos < _tree->columnCount(); pos++)
    _tree->resizeColumnToContents(pos);

  if (i)
    _tree->scrollToItem(i, QAbstractItemView::EnsureVisible);
}

