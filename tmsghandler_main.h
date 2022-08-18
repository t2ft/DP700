// ***************************************************************************
// General Support Classes
// ---------------------------------------------------------------------------
// pfmessagegandler_main.h, header file
// improved message handler for qDebug() etc.
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// ---------------------------------------------------------------------------
// 2021-6-7  tt  Initial version created
// thomas@t2ft.de
// ---------------------------------------------------------------------------
#ifndef TMSGHANDLER_MAIN_H
#define TMSGHANDLER_MAIN_H

#include "tmessagehandler.h"

#define T_INSTALL_MSGHANDLER(filename) {               \
    pTMsgHandler = new TMessageHandler(filename); \
    qInstallMessageHandler(_TMessageHandler);          \
    }

#define T_REMOVE_MSGHANDLER() {                        \
    delete pTMsgHandler;                               \
    }

static TMessageHandler *pTMsgHandler = nullptr;

void _TMessageHandler(QtMsgType t, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    QString tag = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz] ");
    if (pTMsgHandler) {
        switch (t) {
#ifdef QT_DEBUG
        case QtDebugMsg:
            pTMsgHandler->addMessage(tag + "DBUG " + msg);
            break;
#endif
        case QtInfoMsg:
            pTMsgHandler->addMessage(tag + "INFO " + msg);
            break;
        case QtWarningMsg:
            pTMsgHandler->addMessage(tag + "WARN " + msg);
            break;
        case QtCriticalMsg:
            pTMsgHandler->addMessage(tag + "CRIT " + msg);
            break;
        case QtFatalMsg:
            pTMsgHandler->addMessage(tag + "FATL " + msg);
            abort();
        }
    }
#ifndef QT_DEBUG
    else    // always print to stderr in DEBUG mode
#endif
    {
        switch (t) {
#ifdef QT_DEBUG
        case QtDebugMsg:
            fprintf(stdout, "%s DBUG %s\n", qPrintable(tag), qPrintable(msg));
            break;
#endif
        case QtInfoMsg:
            fprintf(stdout, "%s INFO %s\n", qPrintable(tag), qPrintable(msg));
            break;
        case QtWarningMsg:
            fprintf(stderr, "%s WARN %s\n", qPrintable(tag), qPrintable(msg));
            break;
        case QtCriticalMsg:
            fprintf(stderr, "%s CRIT %s\n", qPrintable(tag), qPrintable(msg));
            break;
        case QtFatalMsg:
            fprintf(stderr, "%s FATL %s\n", qPrintable(tag), qPrintable(msg));
            abort();
        }
        fflush(stdout);
        fflush(stderr);
    }
}

#endif // PFMSGHANDLER_MAIN_H
