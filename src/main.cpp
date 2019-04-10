// Copyright (c) 2019 Jonas Schnelli
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QThread>

#include <QFontDatabase>
#include <QProcessEnvironment>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    // create application, main window
    QApplication a(argc, argv);
    StatsMainWindow mainWindow;

    if (QProcessEnvironment::systemEnvironment().value("WINDOWED", "") == "1") {
        mainWindow.setMinimumSize(1000,500);
        mainWindow.show();
    }
    else {
        mainWindow.setWindowState(Qt::WindowFullScreen);
        mainWindow.showFullScreen();
    }

    // add fonts
    QFontDatabase::addApplicationFont(":/fonts/crypto-thin.ttf");
    QFontDatabase::addApplicationFont(":/fonts/crypto-bold.ttf");

    a.exec();
    return 0;
}
