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

#include "TabBar.h"
#include "../../pics/delete.xpm"
#include "../../pics/insert.xpm"

#include <QtDebug>

TabBar::TabBar (QWidget *parent) : QTabBar(parent)
{
  setAcceptDrops(true);

  setStyleSheet("QTabBar::tab { height: 20px; }");

  _menu = new QMenu(this);
  _menu->addAction(QIcon(insert_xpm), tr("Edit Indicator"), this, SLOT(edit()), 0);
  _menu->addSeparator();
  _menu->addAction(QIcon(insert_xpm), tr("Move Indicator"), this, SLOT(moveRow()), 0);
  _menu->addSeparator();
  _menu->addAction(QIcon(delete_xpm), tr("Delete Indicator"), this, SLOT(remove()), 0);

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu()));
}

void
TabBar::mousePressEvent (QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    m_dragStartPos = event->pos(); // m_dragStartPos is a QPoint defined in the header

  QTabBar::mousePressEvent(event);
}

void
TabBar::mouseMoveEvent (QMouseEvent *event)
{
  // If the left button isn't pressed anymore then return
  if (! (event->buttons() & Qt::LeftButton))
    return;

  // If the distance is too small then return
  if ((event->pos() - m_dragStartPos).manhattanLength() < QApplication::startDragDistance())
    return;

  // initiate Drag
  QDrag* drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;

  // a crude way to distinguish tab-reodering drops from other ones
  mimeData->setData("action", "tab-reordering");
  drag->setMimeData(mimeData);
  drag->exec();
}

void
TabBar::dragEnterEvent (QDragEnterEvent *event)
{
  // Only accept if it's an tab-reordering request
  const QMimeData *m = event->mimeData();
  QStringList formats = m->formats();
  if (formats.contains("action") && (m->data("action") == "tab-reordering"))
    event->acceptProposedAction();
}

void
TabBar::dropEvent (QDropEvent *event)
{
  int fromIndex = tabAt(m_dragStartPos);
  int toIndex = tabAt(event->pos());
    
  // Tell interested objects that 
  if (fromIndex != toIndex)
    emit tabMoveRequested(fromIndex, toIndex);
  event->acceptProposedAction();
}

void
TabBar::remove ()
{
  int index = tabAt(_tabMenu);
  QString name = tabText(index);

  QStringList wt;
  wt << QString("OTA") << "-" << tr("Confirm Indicator Delete");

  QStringList mess;
  mess << tr("Remove indicator") << "(" << name << ")";

  QMessageBox msgBox;
  msgBox.setWindowTitle(wt.join(" "));
  msgBox.setText(mess.join(" "));
  msgBox.setInformativeText(tr("Are you sure you want to delete this ?"));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  if (msgBox.exec() == QMessageBox::No)
    return;

  emit signalRemove(name, index);
}

void
TabBar::contextMenu ()
{
  _tabMenu = mapFromGlobal(QCursor::pos());
  _menu->exec(QCursor::pos());
}

void
TabBar::moveRow ()
{
  int index = tabAt(_tabMenu);
  QString name = tabText(index);

  QStringList wt;
  wt << QString("OTA") << "-" << tr("Move Indicator") << "(" << name << ")";

  bool ok = FALSE;
  int row = QInputDialog::getInt(this,
                                 wt.join(" "),
                                 tr("New Row"),
                                 1,
                                 1,
                                 10,
                                 1,
                                 &ok,
                                 0);
  if (! ok)
    return;

  if (index == row)
    return;

  emit signalMoveRow(name, index, row);
}

void
TabBar::edit ()
{
  emit signalEdit(tabText(tabAt(_tabMenu)));
}

