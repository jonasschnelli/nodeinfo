// Copyright (c) 2019 Jonas Schnelli
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "updater.h"

#include <QDebug>
#include <QPainter>
#include <QFont>
#include <QWidget>
#include <QEvent>
#include <QKeyEvent>

#include <memory>

class DataDrawerWidget : public QWidget {
    Q_OBJECT;

protected:
    void paintEvent(QPaintEvent *);

public slots:
    void updatePaint() {
        // update through paint event
        this->update();
    }

public:
    DataDrawerWidget(QWidget *parent);
    std::shared_ptr<DataUpdater> m_updater;

private:
    void drawCenterText(QPainter *painter, const QString &str);
    bool eventFilter(QObject *object, QEvent *event);

    QFont m_font_bold;
    QFontMetrics m_font_bold_m;
    QFont m_font_thin;
    QFontMetrics m_font_thin_m;
    QFont m_font_bold_blocks;
    QFontMetrics m_font_bold_blocks_m;
    QFont m_font_bold_blocks_title;
    QFontMetrics m_font_bold_blocks_title_m;
    QFont m_font_thin_blocks;
    QFontMetrics m_font_thin_blocks_m;
    QFont m_font_thin_footer;
    QFontMetrics m_font_thin_footer_m;

};
