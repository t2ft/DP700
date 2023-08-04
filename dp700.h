// ***************************************************************************
// DP700 power supply serial control tool
// ---------------------------------------------------------------------------
// dp700.h
// instrument communication class, header file
// ---------------------------------------------------------------------------
// Copyright (C) 2022 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2022-8-18  tt  Initial version created
// ***************************************************************************
#ifndef DP700_H
#define DP700_H

#include <QObject>
#include "serdev.h"
#include <QMutex>

class DP700 : public SerDev
{
    Q_OBJECT
public:
    explicit DP700(const QString &port, QObject *parent = nullptr);

public slots:
    bool queryInfo();
    bool measureAll();
    bool setOnOff(bool on);
    bool setVoltageCurrent(double v, double c);

signals:
    void measuredVoltage(double x);
    void measuredCurrent(double x);
    void measuredPower(double x);
    void voltageSet(double x);
    void currentSet(double x);
    void error(const QString &x);
    void idn(const QString &x);
    void version(const QString &x);
    void onoff(bool x);

protected:
    void decodeBuffer(QByteArray &buffer) override;
    void decodeCommand(const QByteArray &buffer);

private:
    typedef enum {
        Idle,
        QueryIdentification,
        privQueryVersion,
        MeasureAll,
        privQueryOnOff,
        SetOnOff,
        privQueryVoltageCurrent,
        SetVoltageCurrent,
        privGetError
    } STATE;

    bool sendCommand(const QByteArray &cmd, STATE currentState, STATE newState);

    QMutex      m_lock;
    STATE       m_state;

};

#endif // DP700_H
