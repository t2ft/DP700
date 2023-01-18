// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// tpowereventfilter.cpp
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2023 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2023-1-18  tt  Initial version created
// ***************************************************************************
#include "tpowereventfilter.h"
#include <QDebug>
#include <QAbstractEventDispatcher>
#include <QDebug>
#include <windows.h>

TPowerEventFilter::TPowerEventFilter(QObject *parent) : QObject(parent)
{

}

bool TPowerEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)
    bool ret = false;
#ifdef WIN32
    MSG* msg = static_cast< MSG* >( message );

    if (msg->message == WM_POWERBROADCAST) {
        switch (msg->wParam) {
        case PBT_APMPOWERSTATUSCHANGE:
            qDebug() << ("PBT_APMPOWERSTATUSCHANGE  received\n");
            emit PowerStatusChange();
            ret = true;
            break;
        case PBT_APMRESUMEAUTOMATIC:
            qDebug() << ("PBT_APMRESUMEAUTOMATIC  received\n");
            emit ResumeAutomatic();
            ret = true;
            break;
        case PBT_APMRESUMESUSPEND:
            qDebug() << ("PBT_APMRESUMESUSPEND  received\n");
            emit ResumeSuspend();
            ret = true;
            break;
        case PBT_APMSUSPEND:
            qDebug() << ("PBT_APMSUSPEND  received\n");
            emit Suspend();
            ret = true;
            break;
        }
    }
#else
    Q_UNUSED(message)
#endif
    return ret;
}
