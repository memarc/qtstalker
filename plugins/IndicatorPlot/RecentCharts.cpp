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

#include "RecentCharts.h"
#include "../pics/recent_chart.xpm"

#include <QDebug>
#include <QStringList>
#include <QSettings>

RecentCharts::RecentCharts (QString sp)
{
  _settingsPath = sp;
  _maxCount = 10;
  createGUI();
}

RecentCharts::~RecentCharts ()
{
  saveSettings();
}

void
RecentCharts::createGUI ()
{
  setPopupMode(QToolButton::InstantPopup);
  setToolTip(tr("Recent Charts"));
  setStatusTip(tr("Recent Charts"));
  setIcon(QIcon(recent_chart_xpm));

  _menu = new QMenu(this);
  _menu->setTitle(tr("Recent Charts"));
  connect(_menu, SIGNAL(triggered(QAction *)), this, SLOT(itemSelected(QAction *)));
  setMenu(_menu);

  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("recent_charts"));
  QStringList tl = settings.value("list").toStringList();

  for (int pos = 0; pos < tl.size(); pos++)
  {
    _symbols.insert(tl.at(pos), tl.at(pos));

    QAction *a = _menu->addAction(tl.at(pos));
    a->setCheckable(FALSE);
    _actions.append(a);
  }
}

void
RecentCharts::addSymbol (QString d)
{
  if (_symbols.contains(d))
    return;

  if (_symbols.count() == _maxCount)
  {
    QAction *a = _actions.takeLast();
    _menu->removeAction(a);
    _symbols.remove(a->text());
    delete a;
  }

  _symbols.insert(d, d);

  QAction *a = new QAction(d, _menu);
  a->setCheckable(FALSE);
  _actions.prepend(a);

  QAction *b = _actions.first();
  _menu->insertAction(b, a);
}

void
RecentCharts::itemSelected (QAction *d)
{
  emit signalSymbol(d->text());
}

void
RecentCharts::saveSettings ()
{
  QStringList tl;
  QHashIterator<QString, QString> it(_symbols);
  while (it.hasNext())
  {
    it.next();
    tl << it.key();
  }

  QSettings settings(_settingsPath, QSettings::NativeFormat);
  settings.beginGroup(QString("recent_charts"));
  settings.setValue("list", tl);
}

