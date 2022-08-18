// ***************************************************************************
// General Support Classes
// ---------------------------------------------------------------------------
// tmessagegandler.h, header file
// improved message handler for qDebug() etc.
// maybe included into main.cpp only and only once.
// messagehandler is installed inserting the following lines at
// the very beginning of main():
//   pTMsgHandler = new TMessageHandler();
//   qInstallMessageHandler(_TMessageHandler);
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2021-6-7  tt  Initial version created
// ---------------------------------------------------------------------------
#ifndef TMESSAGEHANDLER_H
#define TMESSAGEHANDLER_H

#include <QObject>
#include <QMetaType>
#include <QStringList>
#include <QDateTime>

class TMessageHandler : public QObject
{
    Q_OBJECT

public:
    explicit TMessageHandler(const QString &filename, QObject *parent = nullptr);
    ~TMessageHandler();

public slots:
    void addMessage(const QString &text);
    void saveMessages(const QString &fileName);

signals:
    void messageAdded(const QString &msg);
    void messageSaved();

private:
    typedef struct
    {
        QString     text;
        int         repeat;
        qint64      firstTime;
        qint64      lastTime;
    } MSG_ENTRY;

    QList<MSG_ENTRY>    m_msg;
    MSG_ENTRY           m_lastMsg;

    void append(const MSG_ENTRY &msg);

    QString         m_filename;
};

Q_DECLARE_METATYPE(TMessageHandler*)

#endif // TMESSAGEHANDLER_H
