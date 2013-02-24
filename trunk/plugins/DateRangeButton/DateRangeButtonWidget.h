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

#ifndef PLUGIN_DATE_RANGE_BUTTON_WIDGET_HPP
#define PLUGIN_DATE_RANGE_BUTTON_WIDGET_HPP


#include <QtGui>
#include "PopupWidget.h"

class DateRangeButtonWidget : public QToolButton
{
  Q_OBJECT
  
  signals:
    void signalRange ();

  public:
    DateRangeButtonWidget (QString profile);
    ~DateRangeButtonWidget ();
    void createGUI ();
    QString range ();
    QDateTime startDate ();
    QDateTime endDate ();
    bool custom ();
    void setSettings (QStringList l, QString range, QDateTime sd, QDateTime ed, bool cus);

  public slots:
    void showPopup ();
    void popupChanged ();
    
  private:
    QString _profile;
    PopupWidget *_popup;
};

#endif
