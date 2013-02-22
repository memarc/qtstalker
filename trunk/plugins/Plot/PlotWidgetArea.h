/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef PLUGIN_PLOT_WIDGET_AREA_HPP
#define PLUGIN_PLOT_WIDGET_AREA_HPP

#include <QtGui>

#include "PlotWidgetGrid.h"
#include "PlotWidgetCursorInfo.h"
#include "Object.h"
#include "PlotWidgetScale.h"
#include "XMap.h"
#include "YMap.h"

class PlotWidgetArea : public QWidget
{
  Q_OBJECT
  
  signals:
    void signalDateChanged (bool);
    void signalSettingsChanged ();
    void signalMarkerNew (QString, QString);
    void signalSendMessage (ObjectCommand *);
    void signalMarkerDelete (QStringList);
    void signalDataWindow ();

  public:
    PlotWidgetArea (QWidget *, XMap *, YMap *, PlotWidgetScale *, Object *plotObject);
    ~PlotWidgetArea ();
    QColor backgroundColor ();
    QString name ();
    QString symbol ();
    bool info ();
    bool xGrid ();
    bool yGrid ();
    bool date ();
    QMenu * menu ();
    int startPosition ();
    void setYPoints ();
    void setDate (bool);
    int newItem (Object *);
    PlotWidgetCursorInfo * cursorInfo ();

  public slots:
    void clear ();
    void draw ();
    void setBackgroundColor (QColor);
    void setName (QString);
    void setSymbol (QString);
    void setInfo (bool);
    void setXGrid (bool);
    void setYGrid (bool);
    int addObject (QString name, Object *);
    int removeItem (QStringList);
    QHash<QString, Object *> items ();
    void deleteAllMarkers ();
    void objectMessage (ObjectCommand);

  protected:
    virtual void resizeEvent (QResizeEvent *);
    virtual void paintEvent (QPaintEvent *);
    virtual void mousePressEvent (QMouseEvent *);
    virtual void mouseMoveEvent (QMouseEvent *);
//    virtual void keyPressEvent (QKeyEvent *);
//    virtual void mouseDoubleClickEvent (QMouseEvent *);
    
    void drawItems (QPainter *);
    void updateHighLowRange ();
    void createMenu ();

  protected slots:
    void setScalerFlag ();
    void itemUnselected ();
    void itemSelected (QString name);
    void contextMenu ();
    void menuSelected (QAction *d);
    void xGridToggled (bool);
    void yGridToggled (bool);
    void infoToggled (bool);
    void dateToggled (bool);
    
  private:
    PlotWidgetGrid *_plotGrid;
    PlotWidgetCursorInfo *_plotCursorInfo;
    QPixmap _pic;
    XMap *_xmap;
    YMap *_ymap;
    bool _scalerFlag;
    QString _name;
    QString _symbol;
    QColor _backgroundColor;
    QHash<QString, Object *> _items;
    QMap<int, Object *> _plotOrder;
    QHash<QString, int> _orderLookup;
    int _orderPos;
    Object *_itemSelected;
    QMenu *_menu;
    QMenu *_markerMenu;
    PlotWidgetScale *_plotScale;
    QAction *_actionXGrid;
    QAction *_actionYGrid;
    QAction *_actionInfo;
    QAction *_actionDate;
    Object *_plotObject;
    QPainter *_painter;
};

#endif
