// Copyright (c) 2019 Jonas Schnelli
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "mainwindow.h"

#include <QDebug>
#include <QThread>

StatsMainWindow::StatsMainWindow() {
    m_updater = std::make_shared<DataUpdater>();
    m_drawer_widget = std::make_shared<DataDrawerWidget>(this);
    setCentralWidget(m_drawer_widget.get());
    this->installEventFilter(this);
    startUpdater();
}
void StatsMainWindow::startUpdater() {
    m_drawer_widget->m_updater = m_updater;
    m_updater->moveToThread(&m_updater_thread);
    connect(&m_updater_thread, &QThread::started, m_updater.get(), &DataUpdater::startUpdate);
    connect(m_updater.get(), &DataUpdater::resultReady, m_drawer_widget.get(), &DataDrawerWidget::updatePaint);

    m_updater_thread.start();
}

StatsMainWindow::~StatsMainWindow() {
    m_updater_thread.requestInterruption();
    m_updater_thread.quit();
    m_updater_thread.wait();
}


bool StatsMainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape) {
            close();
        }
    }
    return QMainWindow::eventFilter(object, event);
}
