#include "stats.h"

#include <QDebug>
#include <QThread>
#include <QProcess>

#include <QTimer>
#include <QDateTime>

QString formatBytes(long long bytes)
{
    float num = bytes;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}

QString formatNiceTimeOffset(qint64 secs)
{
    // Represent time from last generated block in human readable text
    QString timeBehindText;
    const int HOUR_IN_SECONDS = 60*60;
    const int DAY_IN_SECONDS = 24*60*60;
    const int WEEK_IN_SECONDS = 7*24*60*60;
    const int YEAR_IN_SECONDS = 31556952; // Average length of year in Gregorian calendar
    if(secs < 60)
    {
        timeBehindText = QObject::tr("%n second(s)","",secs);
    }
    else if(secs < 2*HOUR_IN_SECONDS)
    {
        timeBehindText = QObject::tr("%n minute(s)","",secs/60);
    }
    else if(secs < 2*DAY_IN_SECONDS)
    {
        timeBehindText = QObject::tr("%n hour(s)","",secs/HOUR_IN_SECONDS);
    }
    else if(secs < 2*WEEK_IN_SECONDS)
    {
        timeBehindText = QObject::tr("%n day(s)","",secs/DAY_IN_SECONDS);
    }
    else if(secs < YEAR_IN_SECONDS)
    {
        timeBehindText = QObject::tr("%n week(s)","",secs/WEEK_IN_SECONDS);
    }
    else
    {
        qint64 years = secs / YEAR_IN_SECONDS;
        qint64 remainder = secs % YEAR_IN_SECONDS;
        timeBehindText = QObject::tr("%1 and %2").arg(QObject::tr("%n year(s)", "", years)).arg(QObject::tr("%n week(s)","", remainder/WEEK_IN_SECONDS));
    }
    return timeBehindText;
}

void StatsWidget::paintEvent(QPaintEvent *)
{
    if (!m_updater->readyToPaint()) return;
    QPainter painter(this);
    int width = painter.device()->width();
    int height = painter.device()->height();
    const int x_margin = 20;
    const int window_border = 10;
    int fontsize_tipheight = 72;
    int fontsize_blocks = 18;
    int fontsize_blocks_title = 24;

    QList<int> cols { 140, 130, 80, 150, 150};
    QList<QString> cols_hdr { "Height", "Hash", "Txns", "Size", "Age"};
    QList<QString> cols_map_key { "height", "hash", "nTx", "size", "time"};

    QDateTime now = QDateTime::currentDateTime();

    QFont font_bold("Crypto", fontsize_tipheight);
    font_bold.setWeight(QFont::Bold);
    QFontMetrics font_bold_metrics(font_bold);

    QFont font_thin("Crypto", fontsize_tipheight);
    font_thin.setWeight(QFont::Thin);
    QFontMetrics font_thin_metrics(font_thin);

    QFont font_bold_blocks("Crypto", fontsize_blocks);
    font_bold_blocks.setWeight(QFont::Bold);
    QFontMetrics font_bold_blocks_metrics(font_bold_blocks);

    QFont font_bold_blocks_title("Crypto", fontsize_blocks_title);
    font_bold_blocks_title.setWeight(QFont::Bold);
    QFontMetrics font_bold_blocks_title_metrics(font_bold_blocks_title);

    QFont font_thin_blocks("Crypto", fontsize_blocks);
    font_thin_blocks.setWeight(QFont::Thin);
    QFontMetrics font_thin_blocks_metrics(font_thin_blocks);

    QString key_text = "Blocks";
    int key_width = font_bold_metrics.width(key_text);
    int key_height = font_bold_metrics.height()-20;

    painter.setFont(font_bold);
    painter.drawText(window_border,window_border+key_height, key_text);
    painter.setFont(font_thin);
    QMutexLocker ml(&m_updater->m_mutex);
    painter.drawText(window_border+key_width+x_margin,window_border+key_height, QString::number(m_updater->m_bitcoin_height));

    // draw best blocks
    int cur_height = font_bold_blocks_title_metrics.height();
    int cur_x = width-window_border;
    int cur_y = window_border+cur_height;
    int blocks_x_margin = 5;
    int sumcols = 0;
    for(int i = cols.size()-1; i >= 0; i--) { sumcols += cols[i]; }

    painter.setFont(font_bold_blocks_title);
    painter.drawText(cur_x-sumcols,cur_y, "Last Blocks");
    cur_y += cur_height;


    cur_x = width-window_border;
    for(int i = cols.size()-1; i >= 0; i--) {
        painter.setFont(font_bold_blocks);

        cur_x-=cols[i];
        painter.drawText(cur_x, cur_y, cols_hdr[i]);
    }
    cur_y += cur_height;
    for(const QVariantMap &block : m_updater->m_bitcoin_blocks) {
        cur_x = width-window_border;
        for(int i = cols.size()-1; i >= 0; i--) {
            painter.setFont(font_thin_blocks);

            QString text = block[cols_map_key[i]].toString();
            if(cols_map_key[i] == "hash") {
                text = text.right(6);
            }
            else if(cols_map_key[i] == "size") {
                text = formatBytes(block[cols_map_key[i]].toLongLong());
            }
            else if(cols_map_key[i] == "time") {
                QDateTime timestamp;
                timestamp.setTime_t(block[cols_map_key[i]].toLongLong());
                text = formatNiceTimeOffset(timestamp.secsTo(now));
            }
            cur_x-=cols[i];
            painter.drawText(cur_x, cur_y, text);
        }
        cur_y += cur_height;
    }

    painter.drawText(window_border,height-window_border, "Last Update: "+now.toString());
}

bool DataUpdater::getBitcoinRPC(const QString &cmd, QVariantMap &mapOut) {
    // call bitcoin-cli
    QString bitcoin_cli = QProcessEnvironment::systemEnvironment().value("BITCOIN_CLI", "bitcoin-cli");
    QString bitcoin_args = QProcessEnvironment::systemEnvironment().value("BITCOIN_ARGS", "-regtest");
    QString call = bitcoin_cli + " " + bitcoin_args + " " + cmd;
    qDebug() << "executing:" << call << endl;
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
    if (getBitcoinRPC("getblockchaininfo", map))
    {
        // parse JSON data
        QMutexLocker ml(&m_mutex);
        m_bitcoin_height = map["blocks"].toInt();
        QString bbhash = map["bestblockhash"].toString();

        m_bitcoin_blocks.clear();
        for(int i = 0;i<10;i++) {
            getBitcoinRPC("getblock "+bbhash, map);
            m_bitcoin_blocks << map;
            bbhash = map["previousblockhash"].toString();
        }
    }

    // signal that the data is ready
    Q_EMIT resultReady();
    if (QThread::currentThread()->isInterruptionRequested()) {
        return;
    }

    // make sure we update again in 30 seconds
    QTimer::singleShot(30000, this, SLOT(startUpdate()));
}

StatsMainWindow::StatsMainWindow() {
    m_updater = std::make_shared<DataUpdater>();
    setCentralWidget(&m_stats_widget);
    startUpdater();
}
void StatsMainWindow::startUpdater() {
    m_stats_widget.m_updater = m_updater;
    m_updater->m_stats_widget = &m_stats_widget;
    m_updater->moveToThread(&m_updater_thread);
    connect(&m_updater_thread, &QThread::started, m_updater.get(), &DataUpdater::startUpdate);
    connect(m_updater.get(), &DataUpdater::resultReady, &m_stats_widget, &StatsWidget::updatePaint);

    m_updater_thread.start();
}

StatsMainWindow::~StatsMainWindow() {
    m_updater_thread.requestInterruption();
    m_updater_thread.quit();
    m_updater_thread.wait();
}
