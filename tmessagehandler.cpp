// ***************************************************************************
// General Support Classes
// ---------------------------------------------------------------------------
// tmessagegandler.cpp
// improved message handler for qDebug() etc.
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2021-6-7  tt  Initial version created
// ---------------------------------------------------------------------------

#include "tmessagehandler.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>

#define MESSAGE_LIMIT   10000
static const int msgCollateTime = 5;   // print out identical messages after 5 seconds, latest

TMessageHandler::TMessageHandler(const QString &filename, QObject *parent)
    : QObject(parent)
    , m_filename(filename)
{
#ifdef QT_DEBUG
    fprintf(stderr, "+++ TMessageHandler::TMessageHandler()\n");
#endif
    qRegisterMetaType<TMessageHandler*>("TMessageHandlerStar");
    m_lastMsg.text = QString();
    m_lastMsg.firstTime = QDateTime::currentMSecsSinceEpoch();
    m_lastMsg.lastTime = m_lastMsg.firstTime;
    m_lastMsg.repeat = 0;
#ifdef QT_DEBUG
    fprintf(stderr, "--- TMessageHandler::TMessageHandler()\n");
#endif
}

TMessageHandler::~TMessageHandler()
{
#ifdef QT_DEBUG
    fprintf(stderr, "+++ TMessageHandler::~TMessageHandler()\n");
#endif
    saveMessages(m_filename);
#ifdef QT_DEBUG
    fprintf(stderr, "--- TMessageHandler::~TMessageHandler()\n");
#endif
}

void TMessageHandler::addMessage(const QString &text)
{
#ifdef QT_DEBUG
    //fprintf(stderr, "+++ TMessageHandler::addMessage()\n");
#endif
    MSG_ENTRY msg;
    msg.text = text;
    msg.lastTime = QDateTime::currentMSecsSinceEpoch();
    msg.repeat = 0;
    if (msg.text != m_lastMsg.text) {
        //this is a new message
        if (m_lastMsg.repeat)
            append(m_lastMsg);
        append(msg);
        m_lastMsg = msg;
        m_lastMsg.firstTime = msg.lastTime;
    } else {
        m_lastMsg.repeat++;
        m_lastMsg.lastTime = msg.lastTime;
        if ((m_lastMsg.lastTime - m_lastMsg.firstTime) >= msgCollateTime) {
            // repeated same message for too long -> print out message
            append(m_lastMsg);
            m_lastMsg.repeat = 0;
            m_lastMsg.firstTime = m_lastMsg.lastTime;
        }
    }
#ifdef QT_DEBUG
    //fprintf(stderr, "--- TMessageHandler::addMessage()\n");
#endif
}

void TMessageHandler::saveMessages(const QString &fileName)
{
#ifdef QT_DEBUG
    fprintf(stderr, "+++ TMessageHandler::saveMessages(fileName=\"%s\")\n", fileName.toLocal8Bit().constData());
#endif
    if (m_msg.size()) {
        QFile f(fileName);
        if (f.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream t(&f);
            for(MSG_ENTRY &msg : m_msg) {
                t << QDateTime::fromMSecsSinceEpoch(msg.lastTime).toString("[yyyy-MM-dd hh:mm:ss.zzz] ") + msg.text;
                if (msg.repeat > 1) {
                    t << " (repeated " << msg.repeat << " times";
                    uint dT = msg.lastTime - msg.firstTime;
                    if (dT)
                        t << ", within last " << dT << " seconds)";
                    else
                        t << ")";
                }
                t << Qt::endl;
            }
            f.close();
            emit messageSaved();
#ifdef QT_DEBUG
            fprintf(stderr, "    TMessageHandler::saveMessages(): messageSaved!\n");
#endif
        }
    }
#ifdef QT_DEBUG
    fprintf(stderr, "--- TMessageHandler::saveMessages()\n");
#endif
}

void TMessageHandler::append(const MSG_ENTRY &msg)
{
    if (m_msg.size() >= MESSAGE_LIMIT)
        m_msg.removeFirst();
    m_msg.append(msg);
    emit messageAdded(msg.text);
}
