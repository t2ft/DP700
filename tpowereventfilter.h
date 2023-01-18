// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// tpowereventfilter.h
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2023 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2023-1-18  tt  Initial version created
// ***************************************************************************
#ifndef TPOWEREVENTFILTER_H
#define TPOWEREVENTFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>

class TPowerEventFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit TPowerEventFilter(QObject *parent = NULL);

    virtual bool nativeEventFilter(const QByteArray& eventType, void* message, long *result) Q_DECL_OVERRIDE;

signals:
    void PowerStatusChange();   // system power status has changed (battery low, ...)
    void ResumeAutomatic();     // system has resumed from power down; with or without a user being present
    void ResumeSuspend();       // system has resumed from power down; a user is present
    void Suspend();             // system will go to power down very soon

};

#endif // TPOWEREVENTFILTER_H
