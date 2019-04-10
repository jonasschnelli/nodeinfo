// Copyright (c) 2019 Jonas Schnelli
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "updater.h"

#include <QDebug>
#include <QThread>
#include <QProcess>

#include <QTimer>
#include <QDateTime>

#include <QJsonDocument>
#include <QJsonObject>

bool DataUpdater::getBitcoinRPC(const QString &cmd, QVariantMap &mapOut) {
    // call bitcoin-cli
    QString bitcoin_cli = QProcessEnvironment::systemEnvironment().value("BITCOIN_CLI", "bitcoin-cli");
    QString bitcoin_args = QProcessEnvironment::systemEnvironment().value("BITCOIN_ARGS", "-regtest");
    QString call = bitcoin_cli + " " + bitcoin_args + " " + cmd;
    //qDebug() << "executing:" << call << endl;
    QProcess process;
    process.start(call);
    process.waitForFinished(10000);
    QString stdout = process.readAllStandardOutput();
    QJsonDocument doc = QJsonDocument::fromJson(stdout.toUtf8());
    QJsonObject jObject = doc.object();
    if (doc.isEmpty()) {
        qDebug() << "no json response" << endl;
        return false;
    }

    mapOut = jObject.toVariantMap();
    return true;
}
void DataUpdater::startUpdate() {
    QVariantMap map;
    int next_update = 1000;
    if (getBitcoinRPC("getblockchaininfo", map))
    {
        QMutexLocker ml(&m_mutex);

        // we got data, next update in 30 seconds
        next_update = 30000;

        m_updater_try = true;
        QString bbhash;
        // parse JSON data
        {
            m_connection_bitcoin_established = true;

            // store basic blockchain sync infos
            m_bitcoin_height = map["blocks"].toInt();
            bbhash = map["bestblockhash"].toString();
            m_bitcoin_IBD = map["initialblockdownload"].toBool();
            m_bitcoin_verification_progress = map["verificationprogress"].toFloat();
            if (m_bitcoin_verification_progress) {
                // during IBD, update every 5 seconds
                // don't stress bitcoind too much
                next_update = 5000;
            }
        }
        ml.unlock();
        Q_EMIT resultReady();

        if (!m_bitcoin_verification_progress) {
            QList<QVariantMap> bitcoin_blocks;
            {
                // fetch and store last 10 blocks
                for(int i = 0;i<10;i++) {
                    getBitcoinRPC("getblock "+bbhash, map);
                    bitcoin_blocks << map;
                    bbhash = map["previousblockhash"].toString();
                }
            }
            ml.relock();
            m_bitcoin_blocks.clear();
            m_bitcoin_blocks = bitcoin_blocks;
        }
        m_updater_try = true;
    }
    else {
        QMutexLocker ml(&m_mutex);
        m_connection_bitcoin_established = false;
        m_updater_try = true;
    }
    // signal that the data is ready
    Q_EMIT resultReady();
    if (QThread::currentThread()->isInterruptionRequested()) {
        return;
    }

    // make sure we update again in 30 seconds (or every second if no connection is established)
    QTimer::singleShot(next_update, this, SLOT(startUpdate()));
}
