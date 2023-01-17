// ***************************************************************************
// generic t2ft Qt Application
// ---------------------------------------------------------------------------
// mainwidget.h
// main application widget, header file
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2021-06-07  tt  Initial version created
// ---------------------------------------------------------------------------
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "tmainwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class DP700;

class MainWidget : public TMainWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

protected:
    void timerEvent(QTimerEvent *event) override;


private slots:
    void startDevice();
    void on_messageAdded(const QString &msg);
    void setMeasuredVoltage(double x);
    void setMeasuredCurrent(double x);
    void setMeasuredPower(double x);
    void setVoltageSet(double x);
    void setCurrentSet(double x);
    void setOnOff(bool x);
    void printIdentification(const QString &x);
    void printVersion(const QString &x);
    void printError(const QString &x);

    void on_onoff_toggled(bool checked);
    void on_setVA_clicked();
    void on_setVolts_valueChanged(double x);
    void on_setAmps_valueChanged(double x);

    void updateIndicator(bool connected);

    void on_alwaysOnTop_toggled(bool checked);

private:
    Ui::MainWidget *ui;

    typedef enum {
        IdentificationReceived  = 0x00000001,
        VersionReceived         = 0x00000002,
        MeasuredVoltageReceived = 0x00000004,
        MeasuredCurrentReceived = 0x00000008,
        MeasuredPowerReceived   = 0x00000010,
        SetVoltageReceived      = 0x00000020,
        SetCurrentReceived      = 0x00000040,
        OnOffReceived           = 0x00000080,
        ErrorReceived           = 0x00000100,
    } MessageFlags;

    void setOnOffText(bool on);
    void reconnectDevice();
    void triggerWatchdog();

    bool            m_lastCommandErrorRequest;
    DP700           *m_dev;
    quint32         m_flags;
    int             m_idUpdateTimer;
    int             m_idWatchdogTimer;
    bool            m_setOnOff;
    bool            m_newOnOff;
    bool            m_setVA;
    bool            m_setVoltageChanged;
    bool            m_setCurrentChanged;
    double          m_newVoltage;
    double          m_newCurrent;
    int             m_indicatorCount, m_indicatorInc;
};

#endif // MAINWIDGET_H
