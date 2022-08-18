// ***************************************************************************
// General Support Classes
// ---------------------------------------------------------------------------
// tmainwidget.h, header file
// QWidget with additional features to implement standard main widget behavior
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2021-6-7  tt  Initial version created
// ---------------------------------------------------------------------------
#ifndef TMAINWIDGET_H
#define TMAINWIDGET_H

#include <QWidget>

class TMainWidget : public QWidget
{
    Q_OBJECT

private slots:
    void updateGeometry();

public:
    explicit TMainWidget(QWidget *parent = nullptr);

    void closeEvent(QCloseEvent *event) override;

signals:

};

#endif // TMAINWIDGET_H
