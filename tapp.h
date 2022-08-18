// ***************************************************************************
// General Support Classes
// ---------------------------------------------------------------------------
// tapp.h, header file
// QApplication with additional features
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// ---------------------------------------------------------------------------
// 2021-6-7  tt  Initial version created
// ---------------------------------------------------------------------------
#ifndef TAPP_H
#define TAPP_H

#include <QApplication>

class TMessageHandler;


class TApp : public QApplication
{
    Q_OBJECT
public:
    TApp(int &argc, char **argv, const QString &fallbackVersion=QString(), const QString &fallbackName=QString());
    ~TApp();
    TMessageHandler *msgHandler();
};

#define tApp (static_cast<TApp *>(QCoreApplication::instance()))

#endif // TAPP_H
