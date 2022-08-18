// ***************************************************************************
// DP700 power supply serial control tool
// ---------------------------------------------------------------------------
// main.cpp
// main application entry point
// ---------------------------------------------------------------------------
// Copyright (C) 2022 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2022-08-18  tt  Initial version created
// ***************************************************************************
#include "mainwidget.h"
#include "tapp.h"

int main(int argc, char *argv[])
{
    TApp a(argc, argv);
    MainWidget w;
    w.show();
    return a.exec();
}
