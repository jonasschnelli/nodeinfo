// Copyright (c) 2019 Jonas Schnelli
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "mainwindow.h"

#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QApplication>

#ifdef __linux__
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>
#endif

StatsMainWindow::StatsMainWindow() : m_unblank_timer(nullptr) {
    m_updater = std::make_shared<DataUpdater>();
    m_drawer_widget = std::make_shared<DataDrawerWidget>(this);
    setCentralWidget(m_drawer_widget.get());
    this->installEventFilter(this);
    startUpdater();

#ifdef __linux__
    if (qApp->platformName() == "linuxfb") {
        // make sure we always unblank in case user did powerdown/powerup the screen
        // (only do that if we run linuxfb)
        m_unblank_timer = new QTimer();
        m_unblank_timer->setInterval(1000);
        connect(m_unblank_timer, &QTimer::timeout, [=]() {
            int fd = open("/dev/fb0", O_RDWR, 0);
            ioctl(fd, FBIOBLANK, VESA_NO_BLANKING);
            ::close(fd); // use :: to not conflict with the Qt QWidget::close()
          } );
        m_unblank_timer->start();
    }
#endif
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
    delete m_unblank_timer;
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
