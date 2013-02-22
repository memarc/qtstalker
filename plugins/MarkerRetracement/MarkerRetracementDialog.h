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

#ifndef PLUGIN_MARKER_RETRACEMENT_DIALOG_HPP
#define PLUGIN_MARKER_RETRACEMENT_DIALOG_HPP

#include <QtGui>

#include "ColorButton.h"
#include "Dialog.h"

class MarkerRetracementDialog : public Dialog
{
  Q_OBJECT
  
  public:
    MarkerRetracementDialog (QHash<QString, void *>, QString name);
    ~MarkerRetracementDialog ();
    void createTab (QHash<QString, void *>);
    void setSettings (QColor, QDateTime sd, QDateTime ed, double hp, double lp,
		      bool extend, double l1, double l2, double l3, QString po);
    void settings (QColor &, QDateTime &sd, QDateTime &ed, double &hp, double &lp,
		   bool &extend, double &l1, double &l2, double &l3, QString &po);
    
  public slots:
    void done ();
    void saveSettings ();
    void loadSettings ();

  private:
    QDoubleSpinBox *_highPrice;
    QDoubleSpinBox *_lowPrice;
    QDoubleSpinBox *_level1;
    QDoubleSpinBox *_level2;
    QDoubleSpinBox *_level3;
    QDateTimeEdit *_startDate;
    QDateTimeEdit *_endDate;
    ColorButton *_color;
    QCheckBox *_extend;
    QComboBox *_plot;
};

#endif
