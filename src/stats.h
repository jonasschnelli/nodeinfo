#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QLabel>
#include <QMainWindow>

#include <QThread>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>

#include <QVector>

class StatsWidget;

class DataUpdater : public QObject
{
    Q_OBJECT

private:
    bool getBitcoinRPC(const QString &cmd, QVariantMap &mapOut);
public:
    DataUpdater() {
        m_bitcoin_height = -1;
    }
    QMutex m_mutex;
    StatsWidget *m_stats_widget;

    int m_bitcoin_height;
    QList<QVariantMap> m_bitcoin_blocks;

    bool readyToPaint() {
        return m_bitcoin_height != -1;
    }
public slots:
    void startUpdate();

signals:
    void resultReady();
};

class StatsWidget : public QWidget {
    Q_OBJECT;

protected:
    void paintEvent(QPaintEvent *);

public slots:
    void updatePaint() {
        // update through paint event
        this->update();
    }

public:
    std::shared_ptr<DataUpdater> m_updater;
};

class StatsMainWindow : public QMainWindow {
    Q_OBJECT;
public:
    StatsMainWindow();
    StatsWidget m_stats_widget;
    void startUpdater();
    ~StatsMainWindow();
private:
    std::shared_ptr<DataUpdater> m_updater;
    QThread m_updater_thread;
};

