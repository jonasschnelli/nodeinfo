// Copyright (c) 2019 Jonas Schnelli
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "updater.h"

#include <QDebug>
#include <QThread>
#include <QProcess>
#include <QFile>
#include <QTimer>
#include <QDateTime>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QString callBitcoinRPC(const QString &cmd) {
    // call bitcoin-cli
    QString bitcoin_cli = QProcessEnvironment::systemEnvironment().value("BITCOIN_CLI", "bitcoin-cli");
    QString bitcoin_args = QProcessEnvironment::systemEnvironment().value("BITCOIN_ARGS", "-regtest");
    QString call = bitcoin_cli + " " + bitcoin_args + " " + cmd;
    qDebug() << "executing:" << call << endl;
    QProcess process;
    process.start(call);
    process.waitForFinished(QProcessEnvironment::systemEnvironment().value("BITCOIN_RPC_TIMEOUT", "10000").toInt());
    return process.readAllStandardOutput();
}
bool DataUpdater::getBitcoinRPC(const QString &cmd, QVariantMap &mapOut) {
    QJsonDocument doc = QJsonDocument::fromJson(callBitcoinRPC(cmd).toUtf8());
    QJsonObject jObject = doc.object();
    if (doc.isEmpty()) {
        qDebug() << "no json response" << endl;
        return false;
    }
    mapOut = jObject.toVariantMap();
    return true;
}

bool DataUpdater::getBitcoinRPC(const QString &cmd, QVariantList &list_out) {
    QJsonDocument doc = QJsonDocument::fromJson(callBitcoinRPC(cmd).toUtf8());
    if (doc.isEmpty()) {
        qDebug() << "no json response" << endl;
        return false;
    }
    list_out = doc.array().toVariantList();
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
            if (m_bitcoin_IBD) {
                // during IBD, update every 5 seconds
                // don't stress bitcoind too much
                next_update = 5000;
            }
        }
        ml.unlock();
        Q_EMIT resultReady();

        if (!m_bitcoin_IBD && m_last_bestblock != bbhash) {
            m_last_bestblock = bbhash;
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

        // get peerinfos
        QVariantList list_res;
        ml.unlock();
        if (getBitcoinRPC("getpeerinfo", list_res)) {
            ml.relock();
            m_bitcoin_peer_inbound = 0;
            m_bitcoin_peer_outbound = 0;
            for (const QVariant &item : list_res) {
                QVariantMap item_map = item.toMap();
                item_map["inbound"].toBool() ? m_bitcoin_peer_inbound++ : m_bitcoin_peer_outbound++;
            }
        }

        ml.unlock();
        if (getBitcoinRPC("getmempoolinfo", map)) {
            ml.relock();
            m_bitcoin_mempool_count = map["size"].toLongLong();
        }

        m_updater_try = true;
    }
    else {
        QMutexLocker ml(&m_mutex);
        m_connection_bitcoin_established = false;
        m_updater_try = true;
    }

    // read exchangerate file
    QFile f(QProcessEnvironment::systemEnvironment().value("NODE_INFO_EXCHANGE_RATE_FILE", "exchangerate"));
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&f);
        {
            QMutexLocker ml(&m_mutex);
            QString data_raw = in.readAll();
            QStringList data_list = data_raw.split(',');
            m_exchange_rate = 0;
            m_exchange_rate_currency_overwrite.clear();
            m_exchange_rate_title_overwrite.clear();
            if (data_list.size() >= 1) {
                m_exchange_rate = data_list[0].toFloat();
            }
            if (data_list.size() >= 2) {
                m_exchange_rate_currency_overwrite = data_list[1];
            }
            if (data_list.size() >= 3) {
                m_exchange_rate_title_overwrite = data_list[2];
            }
        }
    }

    // signal that the data is ready
    Q_EMIT resultReady();
    if (QThread::currentThread()->isInterruptionRequested()) {
        return;
    }

    // make sure we update again in 30 seconds (or every second if no connection is established)
    QTimer::singleShot(next_update, this, SLOT(startUpdate()));
}
