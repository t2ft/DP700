// ***************************************************************************
// Generic serial device
// ---------------------------------------------------------------------------
// serdev.cpp
// implementation of generic serial device
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2021-07-28  tt  Initial version created
// ---------------------------------------------------------------------------
#ifndef SERDEV_H
#define SERDEV_H

#include <QObject>

class QSerialPort;

class SerDev : public QObject
{
    Q_OBJECT
public:
    explicit SerDev(const QString &portName, quint32 baudrate, QObject *parent = nullptr);
    bool isValid() const { return m_port != nullptr; }
    ~SerDev();

protected:
    virtual void decodeBuffer(QByteArray &buffer) = 0;
    void sendData(const QByteArray &data, quint32 charDelay = 0);

private slots:
    void onNewData();

private:
    QSerialPort     *m_port;
    QByteArray      m_rxBuffer;

};

#endif // SERDEV_H
