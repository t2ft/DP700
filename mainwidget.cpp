// ***************************************************************************
// generic t2ft Qt Application
// ---------------------------------------------------------------------------
// mainwidget.cpp
// main application widget
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2021-06-07  tt  Initial version created
// ---------------------------------------------------------------------------

#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "tapp.h"
#include "tmessagehandler.h"
#include "silentcall.h"
#include <QDebug>
#include <QTimer>
#include <QSettings>
#include <QTimerEvent>
#include <QMessageBox>
#include "dp700.h"

#define InfoFlags (IdentificationReceived | VersionReceived)
#define UpdateFlags (MeasuredVoltageReceived | MeasuredCurrentReceived | MeasuredPowerReceived | SetVoltageReceived | SetCurrentReceived | OnOffReceived | ErrorReceived )

// expect a successful new measurement at least every second
#define WATCHDOG_MS 2000

MainWidget::MainWidget(QWidget *parent)
    : TMainWidget(parent)
    , ui(new Ui::MainWidget)
    , m_lastCommandErrorRequest(false)
    , m_dev(nullptr)
    , m_flags(0)
    , m_idUpdateTimer(0)
    , m_idWatchdogTimer(0)
    , m_setOnOff(false)
    , m_setVA(false)
    , m_setVoltageChanged(false)
    , m_setCurrentChanged(false)
    , m_indicatorCount(0)
    , m_indicatorInc(8)
{
    ui->setupUi(this);
	// allow debug message display
    connect(reinterpret_cast<TApp*>(qApp)->msgHandler(), SIGNAL(messageAdded(QString)), this, SLOT(on_messageAdded(QString)));

    // setup resources
    QFontDatabase::addApplicationFont(":/res/LCDM2B__.TTF");
    QFontDatabase::addApplicationFont(":/res/LCDMB___.TTF");

    // set custom widget fonts
    QFont fontLCD = QFont("LCDMono", 32);
    QFont fontLCDsmall = QFont("LCDMono2", 16);
    ui->measuredVolts->setFont(fontLCD);
    ui->measuredAmps->setFont(fontLCD);
    ui->measuredWatts->setFont(fontLCD);
    ui->setVolts->setFont(fontLCDsmall);
    ui->setAmps->setFont(fontLCDsmall);
    ui->setVolts->setStyleSheet("color:white;");
    ui->setAmps->setStyleSheet("color:white;");

    reconnectDevice();
}

void MainWidget::startDevice()
{
    // check if device is available
    if (!m_dev->isValid()) {
        QMessageBox::critical(this, qApp->applicationDisplayName(), tr("Cannot open serial port"));
        close();
    }
    // start regular operations
    m_idUpdateTimer = startTimer(20, Qt::PreciseTimer);
    triggerWatchdog();
}

MainWidget::~MainWidget()
{
    qDebug() << "MainWidget::~MainWidget()";
    delete m_dev;
    delete ui;
}

void MainWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_idUpdateTimer) {
//        qDebug() << "+++ MainWidget::timerEvent() +++";
//        qDebug() << "      flags =" << Qt::hex << m_flags;
        if ((m_flags & InfoFlags) != InfoFlags) {
//            qDebug() << "      -> query info";
            m_dev->queryInfo();
        } else {
            if ((m_flags & UpdateFlags) != UpdateFlags) {
//                qDebug() << "      -> measure all";
                m_dev->measureAll();
            } else {
                if (m_setOnOff) {
//                    qDebug() << "      -> set on/off to" << (m_newOnOff ? "ON" : "OFF");
                    m_setOnOff = !m_dev->setOnOff(m_newOnOff);
                } else if (m_setVA) {
//                    qDebug() << "      -> set voltage to" << m_newVoltage << "V, current to" << m_newCurrent << "A";
                    m_setVA = !m_dev->setVoltageCurrent(m_newVoltage, m_newCurrent);
                    if (!m_setVA) {
                        m_setVoltageChanged = false;
                        m_setCurrentChanged = false;
                        ui->setVolts->setStyleSheet("color:white;");
                        ui->setAmps->setStyleSheet("color:white;");
                    }
                } else {
//                    qDebug() << " start new measurement";
                    m_flags &= ~UpdateFlags;
                    updateIndicator(true);
                    triggerWatchdog();
                }
            }
        }
//        qDebug() << "--- MainWidget::timerEvent() ---";
    } else if (event->timerId() == m_idWatchdogTimer) {
        if (m_flags) {
            qWarning() << "Watchdog Timeout!";
        }
        updateIndicator(false);
        reconnectDevice();
    }
}

void MainWidget::on_messageAdded(const QString &msg)
{
    QTextCursor cursor = ui->textMessage->cursorForPosition(QPoint(0,1));
    QStringList sl = msg.split(']');
    QString lineTag = sl.takeFirst() + "]";
    QString lineText;
    while (!sl.isEmpty())
        lineText += sl.takeFirst();
    bool bold =  lineText.contains("send:");
    bool italics = lineText.contains("GUI:", Qt::CaseInsensitive);
    QString lineColor, tagColor;
	
	// format qDebug() generated lines
    if (lineText.contains("DBUG")) {
        // do not add debug lines
        return;
    } else if (lineText.contains("INFO")) {
	// format qInfo() generated lines
        tagColor = bold ? "black" : "grey";
        lineColor = "black";
        if (lineText.contains("\'BE\'")) {
            if (bold) {
                m_lastCommandErrorRequest = true;
            } else {
                if (!m_lastCommandErrorRequest) {
                    tagColor = "chocolate";
                    lineColor = "chocolate";
                }
                m_lastCommandErrorRequest = false;
            }
        } else if (lineText.contains("MCP connected", Qt::CaseInsensitive)) {
            tagColor = "darkgreen";
            lineColor = "green";
        } else if (lineText.contains("MCP disconnected", Qt::CaseInsensitive)) {
            tagColor = "maroon";
            lineColor = "brown";
        }
    } else if (lineText.contains("WARN")) {
	// format qWarning() generated lines
        tagColor = bold ? "mediumblue" : "royalblue";
        lineColor = "mediumblue";
        m_lastCommandErrorRequest = false;
    } else if (lineText.contains("CRIT")) {
	// format qCritical() generated lines
        tagColor = bold ? "firebrick" : "indianred";
        lineColor = "firebrick";
        m_lastCommandErrorRequest = false;
    } else if (lineText.contains("FATL")) {
	// format qFatal() generated lines
        tagColor = bold ? "darkviolet" : "blueviolet";
        lineColor = "darkviolet";
        m_lastCommandErrorRequest = false;
    } else {
	// format all other lines
        tagColor = bold ? "gray" : "darkgray";
        lineColor = "gray";
        m_lastCommandErrorRequest = false;
    }
	// generate HTML code for this line
    QString line = "<div><span style=\"color:" + tagColor + ";\">" + lineTag + "</span>" \
            "<span style=\"color:" + lineColor + ";font-weight:" + QString(bold ? "bold" : "regular") + ";\">" +
            QString(italics ? "<i>" : "") + lineText.toHtmlEscaped() + QString(italics ? "</i>" : "") + "</span></div>";
    ui->textMessage->appendHtml(line);
	// ensure last line is visible
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfLine);
    ui->textMessage->setTextCursor(cursor);
    ui->textMessage->ensureCursorVisible();
}

void MainWidget::setMeasuredVoltage(double x)
{
    m_flags |= MeasuredVoltageReceived;
    ui->measuredVolts->setText(QString("%1 V").arg(x, 5, 'f', 2, QLatin1Char('0')));
}

void MainWidget::setMeasuredCurrent(double x)
{
    m_flags |= MeasuredCurrentReceived;
    ui->measuredAmps->setText(QString("%1 A").arg(x, 5, 'f', 2, QLatin1Char('0')));
}

void MainWidget::setMeasuredPower(double x)
{
    m_flags |= MeasuredPowerReceived;
    ui->measuredWatts->setText(QString("%1 W").arg(x, 5, 'f', 2, QLatin1Char('0')));
}

void MainWidget::setVoltageSet(double x)
{
    m_flags |= SetVoltageReceived;
    if (!m_setVoltageChanged)
        SilentCall(ui->setVolts)->setValue(x);
}

void MainWidget::setCurrentSet(double x)
{
    m_flags |= SetCurrentReceived;
    if (!m_setCurrentChanged)
        SilentCall(ui->setAmps)->setValue(x);
}

void MainWidget::setOnOff(bool x)
{
    m_flags |= OnOffReceived;
    if (!m_setOnOff) {
        SilentCall(ui->onoff)->setChecked(x);
        setOnOffText(x);
    }
}

void MainWidget::printIdentification(const QString &x)
{
    m_flags |= IdentificationReceived;
    qInfo() << "Identification:" << x;
}

void MainWidget::printVersion(const QString &x)
{
    m_flags |= VersionReceived;
    qInfo() << "Version:" << x;
}

void MainWidget::printError(const QString &x)
{
    m_flags |= ErrorReceived;
    if (x!="0,\"No error\"")
        qCritical() << "Error:" << x;
}

void MainWidget::on_onoff_toggled(bool checked)
{
    m_setOnOff = true;
    m_newOnOff = checked;
    qInfo() << "switch " << (checked ? "ON" : "OFF");
    setOnOffText(checked);
}


void MainWidget::on_setVA_clicked()
{
    m_newVoltage = ui->setVolts->value();
    m_newCurrent = ui->setAmps->value();
    m_setVA = true;
    qInfo() << "set voltage to" << m_newVoltage << "V";
    qInfo() << "set current to" << m_newCurrent << "A";
}

void MainWidget::on_setVolts_valueChanged(double x)
{
    Q_UNUSED(x)
    m_setVoltageChanged = true;
    ui->setVolts->setStyleSheet("color:red;");
}


void MainWidget::on_setAmps_valueChanged(double x)
{
    Q_UNUSED(x)
    m_setCurrentChanged = true;
    ui->setAmps->setStyleSheet("color:red;");
}

void MainWidget::updateIndicator(bool connected)
{
    ui->indicator->setText(connected ? tr("connected") : tr("Error"));
    QColor backgroundColor = QColor::fromHsv(connected ? 120 : 0, 128, 239+m_indicatorCount/4);
    QColor borderColor = QColor::fromHsv(connected ? 120 : 0, 255, 127+m_indicatorCount);
    ui->indicator->setStyleSheet( QString("color:green;font-weight:bold ;background:#%1%2%3;border-radius:15px;border-style:solid;border-width:4px;border-color:#%4%5%6;")
                                     .arg(backgroundColor.red(),   2, 16, QLatin1Char('0'))
                                     .arg(backgroundColor.green(), 2, 16, QLatin1Char('0'))
                                     .arg(backgroundColor.blue(),  2, 16, QLatin1Char('0'))
                                     .arg(borderColor.red(),   2, 16, QLatin1Char('0'))
                                     .arg(borderColor.green(), 2, 16, QLatin1Char('0'))
                                     .arg(borderColor.blue(),  2, 16, QLatin1Char('0')));
    m_indicatorCount +=m_indicatorInc;
    if ((m_indicatorCount > 64) || (m_indicatorCount < 0)) {
        m_indicatorInc = -m_indicatorInc;
        m_indicatorCount +=m_indicatorInc;
    }
}

void MainWidget::setOnOffText(bool on)
{
    ui->measuredAmps->setStyleSheet(on ? "color:yellow" : "color:darkgrey");
    ui->measuredVolts->setStyleSheet(on ? "color:yellow" : "color:darkgrey");
    ui->measuredWatts->setStyleSheet(on ? "color:yellow" : "color:darkgrey");
    ui->onoff->setText(on ? tr("ON / off") : tr("on / OFF"));
}

void MainWidget::reconnectDevice()
{
    delete m_dev;
    m_flags = 0;
    m_dev = new DP700(this);
    killTimer(m_idUpdateTimer);
    m_idUpdateTimer = 0;
    connect(m_dev, &DP700::measuredVoltage, this, &MainWidget::setMeasuredVoltage);
    connect(m_dev, &DP700::measuredCurrent, this, &MainWidget::setMeasuredCurrent);
    connect(m_dev, &DP700::measuredPower, this, &MainWidget::setMeasuredPower);
    connect(m_dev, &DP700::voltageSet, this, &MainWidget::setVoltageSet);
    connect(m_dev, &DP700::currentSet, this, &MainWidget::setCurrentSet);
    connect(m_dev, &DP700::onoff, this, &MainWidget::setOnOff);
    connect(m_dev, &DP700::idn, this, &MainWidget::printIdentification);
    connect(m_dev, &DP700::version, this, &MainWidget::printVersion);
    connect(m_dev, &DP700::error, this, &MainWidget::printError);

    QTimer::singleShot(250, this, &MainWidget::startDevice);
}

void MainWidget::triggerWatchdog()
{
    killTimer(m_idWatchdogTimer);
    m_idWatchdogTimer = startTimer(WATCHDOG_MS);
}

