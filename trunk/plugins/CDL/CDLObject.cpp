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

#include "CDLObject.h"
#include "ta_libc.h"
#include "CDLDialog.h"
#include "Util.h"
#include "TypeCandle.h"

#include <QtDebug>


CDLObject::CDLObject (QString profile, QString name)
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "CDLObject::CDLObject: error on TA_Initialize";
  
  _profile = profile;
  _name = name;
  _plugin = QString("CDL");
  _type = QString("indicator");
  _outputKey = QString("v");
  _hasOutput = TRUE;
  _inputObject = QString("symbol");
  _openKey = QString("O");
  _highKey = QString("H");
  _lowKey = QString("L");
  _closeKey = QString("C");
  _method = QString("HARAMI");
  _pen = 0.3;
  
  _commandList << QString("update");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");
}

CDLObject::~CDLObject ()
{
  clear();
}

void
CDLObject::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

int
CDLObject::message (ObjectCommand *oc)
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
CDLObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *io = (Object *) oc->getObject(_inputObject);
  if (! io)
  {
    qDebug() << "CDLObject::update: invalid input" << _inputObject;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! io->message(&toc))
  {
    qDebug() << "CDLObject::update: message error" << io->plugin() << toc.command();
    return 0;
  }

  QMap<int, Data *> data = toc.map();
  
  int size = data.size();
  TA_Real open[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer out[size];
  TA_Integer outBeg;
  TA_Integer outNb;
  int dpos = 0;
  QMapIterator<int, Data *> it(data);
  while (it.hasNext())
  {
    it.next();
    Data *d = it.value();
    
    if (! d->contains(_openKey))
      continue;
    if (! d->contains(_highKey))
      continue;
    if (! d->contains(_lowKey))
      continue;
    if (! d->contains(_closeKey))
      continue;

    open[dpos] = (TA_Real) d->value(_openKey).toDouble();
    high[dpos] = (TA_Real) d->value(_highKey).toDouble();
    low[dpos] = (TA_Real) d->value(_lowKey).toDouble();
    close[dpos++] = (TA_Real) d->value(_closeKey).toDouble();
  }

  TA_RetCode rc = TA_SUCCESS;
  TypeCandle tc;
  switch ((TypeCandle::Key) tc.stringToIndex(_method))
  {
    case TypeCandle::_2CROWS:
      rc = TA_CDL2CROWS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_3BLACKCROWS:
      rc = TA_CDL3BLACKCROWS(0,  dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_3INSIDE:
      rc = TA_CDL3INSIDE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_3LINESTRIKE:
      rc = TA_CDL3LINESTRIKE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_3OUTSIDE:
      rc = TA_CDL3OUTSIDE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_3STARSINSOUTH:
      rc = TA_CDL3STARSINSOUTH(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_3WHITESOLDIERS:
      rc = TA_CDL3WHITESOLDIERS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_ABANDONEDBABY:
      rc = TA_CDLABANDONEDBABY(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], _pen, &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_ADVANCEBLOCK:
      rc = TA_CDLADVANCEBLOCK(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_BELTHOLD:
      rc = TA_CDLBELTHOLD(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_BREAKAWAY:
      rc = TA_CDLBREAKAWAY(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_CLOSINGMARUBOZU:
      rc = TA_CDLCLOSINGMARUBOZU(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_CONCEALBABYSWALL:
      rc = TA_CDLCONCEALBABYSWALL(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_COUNTERATTACK:
      rc = TA_CDLCOUNTERATTACK(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_DARKCLOUDCOVER:
      rc = TA_CDLDARKCLOUDCOVER(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], _pen, &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_DOJI:
      rc = TA_CDLDOJI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_DOJISTAR:
      rc = TA_CDLDOJISTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_DRAGONFLYDOJI:
      rc = TA_CDLDRAGONFLYDOJI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_ENGULFING:
      rc = TA_CDLENGULFING(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_EVENINGDOJISTAR:
      rc = TA_CDLEVENINGDOJISTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], _pen, &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_EVENINGSTAR:
      rc = TA_CDLEVENINGSTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], _pen, &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_GAPSIDESIDEWHITE:
      rc = TA_CDLGAPSIDESIDEWHITE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_GRAVESTONEDOJI:
      rc = TA_CDLGRAVESTONEDOJI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_HAMMER:
      rc = TA_CDLHAMMER(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_HANGINGMAN:
      rc = TA_CDLHANGINGMAN(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_HARAMI:
      rc = TA_CDLHARAMI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_HARAMICROSS:
      rc = TA_CDLHARAMICROSS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_HIGHWAVE:
      rc = TA_CDLHIGHWAVE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_HIKKAKE:
      rc = TA_CDLHIKKAKE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_HIKKAKEMOD:
      rc = TA_CDLHIKKAKEMOD(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_HOMINGPIGEON:
      rc = TA_CDLHOMINGPIGEON(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_IDENTICAL3CROWS:
      rc = TA_CDLIDENTICAL3CROWS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_INNECK:
      rc = TA_CDLINNECK(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_INVERTEDHAMMER:
      rc = TA_CDLINVERTEDHAMMER(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_KICKING:
      rc = TA_CDLKICKING(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_KICKINGBYLENGTH:
      rc = TA_CDLKICKINGBYLENGTH(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_LADDERBOTTOM:
      rc = TA_CDLLADDERBOTTOM(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_LONGLEGGEDDOJI:
      rc = TA_CDLLONGLEGGEDDOJI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_LONGLINE:
      rc = TA_CDLLONGLINE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_MARUBOZU:
      rc = TA_CDLMARUBOZU(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_MATCHINGLOW:
      rc = TA_CDLMATCHINGLOW(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_MORNINGDOJISTAR:
      rc = TA_CDLMORNINGDOJISTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], _pen, &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_ONNECK:
      rc = TA_CDLONNECK(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_PIERCING:
      rc = TA_CDLPIERCING(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_RICKSHAWMAN:
      rc = TA_CDLRICKSHAWMAN(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_RISEFALL3METHODS:
      rc = TA_CDLRISEFALL3METHODS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_SEPARATINGLINES:
      rc = TA_CDLSEPARATINGLINES(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_SHOOTINGSTAR:
      rc = TA_CDLSHOOTINGSTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_SHORTLINE:
      rc = TA_CDLSHORTLINE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_SPINNINGTOP:
      rc = TA_CDLSPINNINGTOP(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_STALLEDPATTERN:
      rc = TA_CDLSTALLEDPATTERN(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_STICKSANDWICH:
      rc = TA_CDLSTICKSANDWICH(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_TAKURI:
      rc = TA_CDLTAKURI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_TASUKIGAP:
      rc = TA_CDLTASUKIGAP(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_THRUSTING:
      rc = TA_CDLTHRUSTING(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_TRISTAR:
      rc = TA_CDLTRISTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_UNIQUE3RIVER:
      rc = TA_CDLUNIQUE3RIVER(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_UPSIDEGAP2CROWS:
      rc = TA_CDLUPSIDEGAP2CROWS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TypeCandle::_XSIDEGAP3METHODS:
      rc = TA_CDLXSIDEGAP3METHODS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "CDLObject::getCDL: TA-Lib error" << rc;
    return 0;
  }

  int outLoop = outNb - 1;
  it.toBack();
  while (it.hasPrevious() && outLoop > -1)
  {
    it.previous();
    Data *b = new Data;
    b->insert(_outputKey, out[outLoop--]);
    _bars.insert(it.key(), b);
  }
  
  return 1;
}

int
CDLObject::dialog (ObjectCommand *oc)
{
  CDLDialog *dialog = new CDLDialog(oc->getObjects(), _name);
  dialog->setSettings(_inputObject, _openKey, _highKey, _lowKey, _closeKey, _method);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
CDLObject::dialogDone (void *dialog)
{
  CDLDialog *d = (CDLDialog *) dialog;
  d->settings(_inputObject, _openKey, _highKey, _lowKey, _closeKey, _method);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
CDLObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _outputKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
CDLObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setMap(_bars);
  return 1;
}

int
CDLObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "CDLObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _openKey = settings->value(QString("open_key")).toString();
  _highKey = settings->value(QString("high_key")).toString();
  _lowKey = settings->value(QString("low_key")).toString();
  _closeKey = settings->value(QString("close_key")).toString();
  _method = settings->value(QString("method")).toString();

  return 1;
}

int
CDLObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "CDLObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("open_key"), _openKey);
  settings->setValue(QString("high_key"), _highKey);
  settings->setValue(QString("low_key"), _lowKey);
  settings->setValue(QString("close_key"), _closeKey);
  settings->setValue(QString("method"), _method);

  return 1;
}
