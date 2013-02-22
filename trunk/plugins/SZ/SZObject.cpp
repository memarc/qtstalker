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

/* The "SafeZone Stop" _indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.173
*/


#include "SZObject.h"
#include "SZDialog.h"
#include "Util.h"

#include <QtDebug>


SZObject::SZObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("SZ");
  _type = QString("indicator");
  _period = 10;
  _outputUpKey = QString("u");
  _outputDownKey = QString("d");
  _hasOutput = TRUE;
  _inputObject = QString("symbol");
  _highKey = QString("H");
  _lowKey = QString("L");
  _method = QString(tr("Long"));
  _noDecline = 2;
  _coeff = 2.0;
  
  _commandList << QString("update");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");

  _methodList << QString(tr("Long"));
  _methodList << QString(tr("Short"));
}

SZObject::~SZObject ()
{
  clear();
}

void
SZObject::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

int
SZObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = update(oc);
      break;
    case 1:
      rc = dialog(oc);
      break;
    case 2:
      rc = output(oc);
      break;
    case 3:
      rc = load(oc);
      break;
    case 4:
      rc = save(oc);
      break;
    case 5:
      rc = outputKeys(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
SZObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *io = (Object *) oc->getObject(_inputObject);
  if (! io)
  {
    qDebug() << "SZObject::update: invalid input" << _inputObject;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! io->message(&toc))
  {
    qDebug() << "SZObject::update: message error" << io->plugin() << toc.command();
    return 0;
  }

  QMap<int, Data *> data = toc.map();
  QList<int> keys = data.keys();
  
  double uptrend_stop = 0;
  double dntrend_stop = 0;

  if (_noDecline < 0)
    _noDecline = 0;
  if (_noDecline > 365)
    _noDecline = 365;

  double old_uptrend_stops[_noDecline];
  double old_dntrend_stops[_noDecline];
  for (int pos = 0; pos < _noDecline; pos++)
  {
    old_uptrend_stops[pos] = 0;
    old_dntrend_stops[pos] = 0;
  }

  int ipos = _period + 1;
  int start = ipos;
  for (; ipos < keys.size(); ipos++)
  {
    // calculate downside/upside penetration for lookback period
    int lbloop;
    int lbstart = ipos - _period;
    if (lbstart < 2)
      lbstart = 2;
    double uptrend_noise_avg = 0;
    double uptrend_noise_cnt = 0;
    double dntrend_noise_avg = 0;
    double dntrend_noise_cnt = 0;
    for (lbloop = lbstart; lbloop < ipos; lbloop++)
    {
      Data *bar = data.value(keys.at(lbloop));
      if (! bar)
        continue;

      Data *pbar = data.value(keys.at(lbloop - 1));
      if (! pbar)
        continue;

      double lo_curr = bar->value(_lowKey).toDouble();
      double lo_last = pbar->value(_lowKey).toDouble();
      double hi_curr = bar->value(_highKey).toDouble();
      double hi_last = pbar->value(_highKey).toDouble();
      if (lo_last > lo_curr)
      {
        uptrend_noise_avg += lo_last - lo_curr;
        uptrend_noise_cnt++;
      }
      if (hi_last < hi_curr)
      {
        dntrend_noise_avg += hi_curr - hi_last;
        dntrend_noise_cnt++;
      }
    }

    // make *_avg into actual averages
    if (uptrend_noise_cnt > 0)
      uptrend_noise_avg /= uptrend_noise_cnt;
    if (dntrend_noise_cnt > 0)
      dntrend_noise_avg /= dntrend_noise_cnt;

    Data *pbar = data.value(keys.at(ipos - 1));
    if (! pbar)
      continue;

    double lo_last = pbar->value(_lowKey).toDouble();
    double hi_last = pbar->value(_highKey).toDouble();
    uptrend_stop = lo_last - _coeff * uptrend_noise_avg;
    dntrend_stop = hi_last + _coeff * dntrend_noise_avg;

    double adjusted_uptrend_stop = uptrend_stop;
    double adjusted_dntrend_stop = dntrend_stop;

    for (int backloop = _noDecline - 1; backloop >= 0; backloop--)
    {
      if (ipos - backloop > start)
      {
        if (old_uptrend_stops[backloop] > adjusted_uptrend_stop)
          adjusted_uptrend_stop = old_uptrend_stops[backloop];
        if (old_dntrend_stops[backloop] < adjusted_dntrend_stop)
          adjusted_dntrend_stop = old_dntrend_stops[backloop];
      }
      if (backloop > 0)
      {
        old_uptrend_stops[backloop] = old_uptrend_stops[backloop-1];
        old_dntrend_stops[backloop] = old_dntrend_stops[backloop-1];
      }
    }

    old_uptrend_stops[0] = uptrend_stop;
    old_dntrend_stops[0] = dntrend_stop;

    Data *b = new Data;
    b->insert(_outputUpKey, adjusted_uptrend_stop);
    b->insert(_outputDownKey, adjusted_dntrend_stop);
    _bars.insert(keys.at(ipos), b);
  }

  return 1;
}

int
SZObject::dialog (ObjectCommand *oc)
{
  SZDialog *dialog = new SZDialog(oc->getObjects(), _methodList, _name);
  dialog->setSettings(_inputObject, _highKey, _lowKey, _period, _noDecline, _coeff, _method);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
SZObject::dialogDone (void *dialog)
{
  SZDialog *d = (SZDialog *) dialog;
  d->settings(_inputObject, _highKey, _lowKey, _period, _noDecline, _coeff, _method);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
SZObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _outputUpKey << _outputDownKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
SZObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setMap(_bars);
  return 1;
}

int
SZObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "SZObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _highKey = settings->value(QString("high_key")).toString();
  _lowKey = settings->value(QString("low_key")).toString();
  _period = settings->value(QString("period")).toInt();
  _noDecline = settings->value(QString("no_decline")).toInt();
  _coeff = settings->value(QString("coeff")).toDouble();
  _method = settings->value(QString("method")).toString();

  return 1;
}

int
SZObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "SZObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("high_key"), _highKey);
  settings->setValue(QString("low_key"), _lowKey);
  settings->setValue(QString("period"), _period);
  settings->setValue(QString("no_decline"), _noDecline);
  settings->setValue(QString("coeff"), _coeff);
  settings->setValue(QString("method"), _method);

  return 1;
}
