// Copyright (c) 2019 Jonas Schnelli
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "datadrawer.h"

#include "utils.h"

#include "config/bitcoin-config.h"

#include <QDebug>
#include <QThread>
#include <QProcess>
#include <QTimer>
#include <QDateTime>

static const QList<int> cols { 140, 130, 80, 150, 150};
static const QList<QString> cols_hdr { "Height", "Hash", "Txns", "Size", "Age"};
static const QList<QString> cols_map_key { "height", "hash", "nTx", "size", "time"};

DataDrawerWidget::DataDrawerWidget(QWidget *parent) :
    QWidget(parent),
    m_font_bold_m(QFont()),
    m_font_thin_m(QFont()),
    m_font_bold_blocks_m(QFont()),
    m_font_bold_blocks_title_m(QFont()),
    m_font_thin_blocks_m(QFont()),
    m_font_thin_footer_m(QFont())
{
    static const int fontsize_tipheight = 72;
    static const int fontsize_blocks = 18;
    static const int fontsize_blocks_title = 24;
    static const int fontsize_footer = 12;

    m_font_bold.setFamily("Crypto");
    m_font_bold.setPointSize(fontsize_tipheight);
    m_font_bold.setWeight(QFont::Bold);
    m_font_bold_m = QFontMetrics(m_font_bold);

    m_font_thin.setFamily("Crypto");
    m_font_thin.setPointSize(fontsize_tipheight);
    m_font_thin.setWeight(QFont::Thin);
    m_font_thin_m = QFontMetrics(m_font_thin);

    m_font_bold_blocks.setFamily("Crypto");
    m_font_bold_blocks.setPointSize(fontsize_blocks);
    m_font_bold_blocks.setWeight(QFont::Bold);
    m_font_bold_blocks_m = QFontMetrics(m_font_bold_blocks);

    m_font_bold_blocks_title.setFamily("Crypto");
    m_font_bold_blocks_title.setPointSize(fontsize_blocks_title);
    m_font_bold_blocks_title.setWeight(QFont::Bold);
    m_font_bold_blocks_title_m = QFontMetrics(m_font_bold_blocks_title);

    m_font_thin_blocks.setFamily("Crypto");
    m_font_thin_blocks.setPointSize(fontsize_blocks);
    m_font_thin_blocks.setWeight(QFont::Thin);
    m_font_thin_blocks_m = QFontMetrics(m_font_thin_blocks);

    m_font_thin_footer.setFamily("Crypto");
    m_font_thin_footer.setPointSize(fontsize_footer);
    m_font_thin_footer.setWeight(QFont::Thin);
    m_font_thin_footer_m = QFontMetrics(m_font_thin_footer);
}

void DataDrawerWidget::drawCenterText(QPainter *painter, const QString &text) {
    int width = painter->device()->width();
    int height = painter->device()->height();
    int key_width = m_font_bold_blocks_m.width(text);
    int key_height = m_font_bold_blocks_m.height();
    painter->setFont(m_font_bold_blocks);
    painter->drawText((width-key_width)/2,(height)/2, text);
}
void DataDrawerWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    int width = painter.device()->width();
    int height = painter.device()->height();
    const int x_margin = 20;
    const int window_border = 10;
    QDateTime now = QDateTime::currentDateTime();

    // paint global info
    painter.setFont(m_font_thin_footer);
    painter.drawText(window_border,height-window_border, "Last Update: "+now.toString());
    QString about = QString("NodeInfo V%1.%2.%3").arg(CLIENT_VERSION_MAJOR).arg(CLIENT_VERSION_MINOR).arg(CLIENT_VERSION_REVISION);
    int about_width = m_font_thin_footer_m.width(about);
    painter.drawText(width-about_width-window_border,height-window_border, about);

    QMutexLocker ml(&m_updater->m_mutex);

    // don't paint if not ready
    if (!m_updater->readyToPaint()) {
        drawCenterText(&painter, "Loading...");
        return;
    }

    if (!m_updater->hasBitcoinConnection()) {
        // draw disconnected
        QString disconnected_text = "Disconnected";
        int key_width = m_font_bold_m.width(disconnected_text);
        int key_height = m_font_bold_m.height();
        painter.setFont(m_font_bold);
        painter.drawText((width-key_width)/2,(height)/2, disconnected_text);
        return;
    }

    // draw info
    int cur_y = window_border;
    QString key_text = "Blocks";
    int key_width = m_font_bold_m.width(key_text);
    int key_height = m_font_bold_m.height()-20;
    cur_y+=key_height;
    painter.setFont(m_font_bold);
    painter.drawText(window_border,cur_y, key_text);
    painter.setFont(m_font_thin);
    painter.drawText(window_border+key_width+x_margin,cur_y, QString::number(m_updater->m_bitcoin_height));

    if (m_updater->m_bitcoin_IBD) {
        cur_y += m_font_thin_blocks_m.height();
        QString key_text = "Synchronizing... "+QString::number(m_updater->m_bitcoin_verification_progress*100, 'f', 2)+"%";
        painter.setFont(m_font_thin_blocks);
        painter.drawText(window_border,cur_y, key_text);
    }

    cur_y+=key_height*1.5;

    if (m_updater->m_exchange_rate != 0) {
        key_text = m_updater->m_exchange_rate_title_overwrite.isEmpty() ? "BTC/" + (m_updater->m_exchange_rate_currency_overwrite.isEmpty() ? "USD" : m_updater->m_exchange_rate_currency_overwrite) : m_updater->m_exchange_rate_title_overwrite;
        key_width = m_font_bold_m.width(key_text);
        painter.setFont(m_font_bold);
        painter.drawText(window_border,cur_y, key_text);
        painter.setFont(m_font_thin);
        painter.drawText(window_border+key_width+x_margin,cur_y, QString::number(m_updater->m_exchange_rate));
    }

    // draw best blocks
    int cur_height = m_font_bold_blocks_title_m.height();
    int cur_x = width-window_border;
    cur_y = window_border+cur_height;
    int blocks_x_margin = 5;
    int sumcols = 0;
    for(int i = cols.size()-1; i >= 0; i--) { sumcols += cols[i]; }

    if (!m_updater->m_bitcoin_IBD) {
        // draw latest blocks
        painter.setFont(m_font_bold_blocks_title);
        painter.drawText(cur_x-sumcols,cur_y, "Latest Blocks");
        cur_y += cur_height;

        cur_x = width-window_border;
        for(int i = cols.size()-1; i >= 0; i--) {
            painter.setFont(m_font_bold_blocks);

            cur_x-=cols[i];
            painter.drawText(cur_x, cur_y, cols_hdr[i]);
        }
        cur_y += cur_height;
        if (m_updater->m_bitcoin_blocks.size() == 0) {
            cur_x = width-window_border;
            painter.drawText(cur_x-sumcols, cur_y, "Loading...");
        }
        else {
            for(const QVariantMap &block : m_updater->m_bitcoin_blocks) {
                cur_x = width-window_border;
                for(int i = cols.size()-1; i >= 0; i--) {
                    painter.setFont(m_font_thin_blocks);

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
        }
    }
}

bool DataDrawerWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape) {
            qDebug() << "close";
            close();
        }
    }
    return QWidget::eventFilter(object, event);
}
