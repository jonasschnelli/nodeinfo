// Copyright (c) 2019 Jonas Schnelli
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QList>
#include <QMutex>

class DataUpdater : public QObject
{
    Q_OBJECT

private:
    bool getBitcoinRPC(const QString &cmd, QVariantMap &mapOut);
public:
    DataUpdater() {
        m_bitcoin_height = -1;
        m_bitcoin_IBD = false;
        m_connection_bitcoin_established = false;
        m_updater_try = false;
        m_bitcoin_verification_progress = 0.0;
    }
    QMutex m_mutex;

    bool m_connection_bitcoin_established;
    bool m_updater_try;
    int m_bitcoin_height;
    QList<QVariantMap> m_bitcoin_blocks;
    bool m_bitcoin_IBD;
    float m_bitcoin_verification_progress;

    bool readyToPaint() {
        return m_updater_try;
    }
    bool hasBitcoinConnection() {
        return m_connection_bitcoin_established;
    }
    bool hasBitcoinData() {
        return m_bitcoin_height != -1;
    }
public slots:
    void startUpdate();

signals:
    void resultReady();
};


