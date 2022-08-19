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
#include "serdev.h"
#include <QSerialPort>
#include <QDebug>
#include <QThread>

SerDev::SerDev(const QString &portName, quint32 baudrate, QObject *parent) : QObject(parent)
  , m_port(new QSerialPort(portName, this))
{
    m_port->setBaudRate(baudrate);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setParity(QSerialPort::NoParity);
    if (m_port->open(QSerialPort::ReadWrite)) {
        qDebug().nospace() << qPrintable(portName) << ": serial port is open";
        connect(m_port, &QSerialPort::readyRead, this, &SerDev::onNewData);
    } else {
        qDebug().nospace() << qPrintable(portName) << ": failed to open serial port";
        delete m_port;
        m_port = nullptr;
    }
}

SerDev::~SerDev()
{
    qDebug() << "Serdev::~SerDev()";
    delete m_port;
}


void SerDev::onNewData()
{
    m_rxBuffer.append(m_port->readAll());
    decodeBuffer(m_rxBuffer);
}


void SerDev::sendData(const QByteArray &data, quint32 charDelay)
{
    if (nullptr != m_port) {
        if (charDelay) {
            for (auto x : data) {
                m_port->write(&x, 1);
                m_port->flush();
                QThread::msleep(charDelay);
            }
        } else {
            m_port->write(data);
        }
    }
}
