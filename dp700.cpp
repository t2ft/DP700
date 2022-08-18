// ***************************************************************************
// DP700 power supply serial control tool
// ---------------------------------------------------------------------------
// dp700.cpp
// instrument communication class
// ---------------------------------------------------------------------------
// Copyright (C) 2022 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2022-8-18  tt  Initial version created
// ***************************************************************************
#include "dp700.h"
#include <QMutexLocker>
#include <QDebug>

DP700::DP700(QObject *parent)
    : SerDev("COM1", 9600, parent)
    , m_state(Idle)
{
}

bool DP700::queryInfo()
{
    return sendCommand("*IDN?", Idle, QueryIdentification);
}

bool DP700::measureAll()
{
    return sendCommand(":MEAS:ALL?", Idle, MeasureAll);
}

bool DP700::setOnOff(bool on)
{
    return sendCommand(QString(":OUTP:STAT CH1,%1").arg(on ? "ON" : "OFF").toLatin1(), Idle, Idle);
}

bool DP700::setVoltageCurrent(double v, double c)
{
    return sendCommand(QString(":APPL CH1,%1,%2").arg(v, 0, 'f', 2).arg(c, 0, 'f', 2).toLatin1(), Idle, Idle);
}

void DP700::decodeBuffer(QByteArray &buffer)
{
    // check if there is a reply terminator in the received data
    int inx;
    while ((inx = buffer.indexOf('\n')) >= 0) {
        decodeCommand(buffer.left(inx));
        buffer = buffer.mid(inx+1);
    }
}

void DP700::decodeCommand(const QByteArray &buffer)
{
//    qDebug() << "+++ DP700::decodeCommand(buffer =" << buffer << ") +++";
//    qDebug() << "      m_state =" << m_state;
    switch(m_state) {
    case QueryIdentification: {
        sendCommand(":SYST:VERS?", m_state, privQueryVersion);
        emit idn(buffer);
        break;
    }
    case privQueryVersion: {
        m_state = Idle;
        emit version(buffer);
        break;
    }
    case MeasureAll: {
        sendCommand(":OUTP:STAT?", m_state, privQueryOnOff);
        QList<QByteArray> reply = buffer.split(',');
        if (reply.size()==3) {
            emit measuredVoltage(reply[0].trimmed().toDouble());
            emit measuredCurrent(reply[1].trimmed().toDouble());
            emit measuredPower(reply[2].trimmed().toDouble());
        }
        break;
    }
    case privQueryOnOff: {
        emit onoff(buffer=="ON" ? true : false);
        sendCommand(":APPL?", m_state, privQueryVoltageCurrent);
        break;
    }
    case privQueryVoltageCurrent: {
        sendCommand(":SYST:ERR?", m_state, privGetError);
        QList<QByteArray> reply = buffer.split(',');
        if (reply.size()==2) {
            emit voltageSet(reply[0].trimmed().toDouble());
            emit currentSet(reply[1].trimmed().toDouble());
        }
        break;
    }
    case privGetError: {
        m_state = Idle;
        emit error(buffer);
        break;
    }
    case SetOnOff: {
        m_state = Idle;
        emit onoff(buffer=="ON" ? true : false);
        break;
    }
    case SetVoltageCurrent: {
        m_state = Idle;
        QList<QByteArray> reply = buffer.split(',');
        if (reply.size()==2) {
            emit voltageSet(reply[0].trimmed().toDouble());
            emit currentSet(reply[1].trimmed().toDouble());
        }
        break;
    }
    default: {
        m_state = Idle;
        qWarning() << "      unexpected data received";
        break;
    }
    }

//    qDebug() << "--- DP700::decodeCommand() ---";
}


bool DP700::sendCommand(const QByteArray &cmd, STATE currentState, STATE newState)
{
//    qDebug() << "+++ DP700::sendCommand(cmd =" << cmd << "currentState =" << currentState << "newState =" << newState << ") +++";
//    qDebug() << "      m_state =" << m_state;
    bool ret = false;
    QMutexLocker lock(&m_lock);
    if (m_state==currentState) {
        ret = true;
        m_state = newState;
        QByteArray txData(cmd);
        if (txData.right(1) != "\n")
            txData.append('\n');
        sendData(txData);
    }
//    qDebug() << "--- DP700::sendCommand() -> " << ret << "---";
    return ret;
}
