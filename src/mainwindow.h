// Copyright (c) 2019 Jonas Schnelli
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "datadrawer.h"

#include <QDebug>
#include <QMainWindow>
#include <QThread>
#include <QTimer>

class StatsMainWindow : public QMainWindow {
    Q_OBJECT;
public:
    StatsMainWindow();
    std::shared_ptr<DataDrawerWidget> m_drawer_widget;
    void startUpdater();
    ~StatsMainWindow();
private:
    std::shared_ptr<DataUpdater> m_updater;
    QThread m_updater_thread;
    bool eventFilter(QObject *object, QEvent *event);
    QTimer *m_unblank_timer;
};

