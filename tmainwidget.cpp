// ***************************************************************************
// General Support Classes
// ---------------------------------------------------------------------------
// tmainwidget.cpp
// QWidget with additional features to implement standard main widget behavior
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2021-6-7  tt  Initial version created
// ---------------------------------------------------------------------------

#include "tmainwidget.h"
#include <QSettings>
#include <QTimer>
#include <QSplitter>
#include <QDebug>

#define CFG_GRP_WINDOW  "TWindow"
#define CFG_GEOMETRY    "geometry"
#define CFG_MAXIMIZED   "maximized"


TMainWidget::TMainWidget(QWidget *parent)
    : QWidget(parent)
{
    qDebug() << "+++ TMainWidget::TMainWidget(QWidget *parent)";
    QTimer::singleShot(1, this, SLOT(updateGeometry()));
    qDebug() << "--- TMainWidget::TMainWidget(QWidget *parent)";
}

void TMainWidget::closeEvent(QCloseEvent *event)
{
    qDebug() << "+++ TMainWidget::closeEvent(QCloseEvent *event)";
    QSettings cfg;
    cfg.beginGroup(CFG_GRP_WINDOW);
    cfg.setValue(CFG_GEOMETRY, saveGeometry());
    cfg.setValue(CFG_MAXIMIZED, isMaximized());
    // save state of all QSplitter children
    for (auto &child : findChildren<QSplitter*>()) {
        qDebug() << "saveState for " << child->objectName();
        cfg.setValue(child->objectName(), child->saveState());
    }
    cfg.endGroup();
    QWidget::closeEvent(event);
    qDebug() << "--- TMainWidget::closeEvent(QCloseEvent *event)";
}

void TMainWidget::updateGeometry()
{
    qDebug() << "+++ TMainWidget::updateGeometry()";
    QSettings cfg;
    cfg.beginGroup(CFG_GRP_WINDOW);
    restoreGeometry(cfg.value(CFG_GEOMETRY).toByteArray());
    // restore state for all QSplitter children
    for (auto &child : findChildren<QSplitter*>()) {
        qDebug() << "restoreState for " << child->objectName();
        child->restoreState(cfg.value(child->objectName()).toByteArray());
    }
    if (cfg.value(CFG_MAXIMIZED, false).toBool())
            QTimer::singleShot(1, this, SLOT(showMaximized()));
    cfg.endGroup();
    qDebug() << "--- TMainWidget::updateGeometry()";
}
