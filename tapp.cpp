// ***************************************************************************
// General Support Classes
// ---------------------------------------------------------------------------
// tapp.cpp
// QApplication with additional features
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2021-6-7  tt  Initial version created
// ---------------------------------------------------------------------------
#include "tapp.h"
#include "tmsghandler_main.h"
#include <QFile>

#define FALLBACK_ORGANIZATION "t2ft"
#define FALLBACK_DOMAIN "t2ft.de"


TApp::TApp(int &argc, char **argv, const QString &fallbackVersion, const QString &fallbackName)
    : QApplication(argc, argv)
{
    QString organization(FALLBACK_ORGANIZATION);
#ifdef APP_ORGANIZATION
    organization = APP_ORGANIZATION;
    if (organization.isEmpty())
        organization = FALLBACK_ORGANIZATION;
#endif
    QString domain(FALLBACK_DOMAIN);
#ifdef APP_DOMAIN
    domain = APP_DOMAIN;
    if (domain.isEmpty())
        domain = FALLBACK_DOMAIN;
#endif
    QString version(fallbackVersion);
#ifdef APP_VERSION
    version = APP_VERSION;
    if (version.isEmpty())
        version = fallbackVersion;
#endif

    QString name(fallbackName);
#ifdef APP_NAME
    name = APP_NAME;
    if (name.isEmpty())
        name = fallbackName;
#endif

    QString logname = name.replace( QRegExp( "[" + QRegExp::escape( "\\/:*?\"<>|" ) + "]" ), QString( "_" )) + ".log";
    T_INSTALL_MSGHANDLER(logname);

    setOrganizationName(organization);
    setOrganizationDomain(domain);
    setApplicationVersion(version);
    setApplicationName(name);
    setApplicationDisplayName(name + "-V" + version);
#ifdef QT_DEBUG
    setApplicationDisplayName(applicationDisplayName() + " (DEBUG)");
#endif
}

TApp::~TApp()
{
    T_REMOVE_MSGHANDLER();
}

TMessageHandler *TApp::msgHandler()
{
    return pTMsgHandler;
}
