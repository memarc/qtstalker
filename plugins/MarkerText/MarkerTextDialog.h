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

#ifndef PLUGIN_MARKER_TEXT_DIALOG_HPP
#define PLUGIN_MARKER_TEXT_DIALOG_HPP

#include <QtGui>

#include "Object.h"
#include "Dialog.h"

class MarkerTextDialog : public Dialog
{
  Q_OBJECT
  
  public:
    MarkerTextDialog (QHash<QString, void *>, QString name);
    ~MarkerTextDialog ();
    void createTab (QHash<QString, void *>);
    void setSettings (QColor color, QDateTime date, double price, QString text, QFont font, QString po);
    void settings (QColor &color, QDateTime &date, double &price, QString &text, QFont &font, QString &po);

  public slots:
    void done ();
    void saveSettings ();
    void loadSettings ();

  private:
    QDoubleSpinBox *_price;
    QDateTimeEdit *_date;
    Object *_color;
    Object *_font;
    QLineEdit *_text;
    QComboBox *_plot;
};

#endif
