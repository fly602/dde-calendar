
/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "calendardbus.h"

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QEvent>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <QQueue>
#include "yearview.h"
#include <QSpacerItem>
#include <DPalette>
#include <QLocale>
#include <DHiDPIHelper>
#include "scheduledatamanage.h"
#include "yearschceduleview.h"
#include <QDesktopWidget>
#include <DApplication>
DGUI_USE_NAMESPACE
CYearView::CYearView(QWidget *parent) : CustomFrame(parent)
{
    m_dayNumFont.setFamily("Helvetica");
    m_dayNumFont.setPixelSize(12);
    //m_dayNumFont.setWeight(QFont::Light);

    //setStyleSheet("QWidget { background: rgba(0, 0, 0, 0) }");

    //add separator line
    m_currentMouth = new CustomFrame();
    m_currentMouth->setFixedHeight(24);
    m_currentMouth->setContentsMargins(0, 0, 0, 0);
    //m_currentMouth->setMinimumWidth(100);
    m_currentMouth->setTextAlign(Qt::AlignLeft);
    //m_currentMouth->setStyleSheet("border: 1px solid rgba(0, 0, 0, 0.05);");

    m_momthFont.setFamily("SourceHanSansSC");
    m_momthFont.setWeight(QFont::Medium);
    m_momthFont.setPixelSize(16);
    m_currentMouth->setTextFont(m_momthFont);
    //DPalette Lunadpa = m_currentMouth->palette();
    //Lunadpa.setColor(DPalette::WindowText, QColor("#CF0059"));
    //m_currentMouth->setPalette(Lunadpa);
    QHBoxLayout *separatorLineLayout = new QHBoxLayout;
    separatorLineLayout->setMargin(0);
    separatorLineLayout->setSpacing(0);
    separatorLineLayout->setContentsMargins(0, 0, 0, 0);
    //separatorLineLayout->addSpacing(13);
    separatorLineLayout->addWidget(m_currentMouth);
    separatorLineLayout->addStretch();
    // QSpacerItem *t_spaceitem = new QSpacerItem(30, 32, QSizePolicy::Expanding, QSizePolicy::Fixed);
    //separatorLineLayout->addSpacerItem(t_spaceitem);

    m_currentMouth->show();
    m_currentMouth->installEventFilter(this);

    // cells grid
    m_gridLayout = new QGridLayout;
    m_gridLayout->setMargin(0);
    m_gridLayout->setSpacing(0);
    m_gridLayout->setHorizontalSpacing(6);
    m_gridLayout->setVerticalSpacing(3);
    for (int r = 0; r != 6; ++r) {
        for (int c = 0; c != 7; ++c) {
            QWidget *cell = new QWidget;
            cell->setFixedSize(cellwidth, cellheight);
            cell->installEventFilter(this);
            cell->setFocusPolicy(Qt::ClickFocus);
            m_gridLayout->addWidget(cell, r, c);
            m_cellList.append(cell);
        }
    }
    m_hhLayout = new QVBoxLayout;
    m_hhLayout->addLayout(separatorLineLayout);
    m_hhLayout->addLayout(m_gridLayout);
    m_hhLayout->setMargin(0);
    m_hhLayout->setSpacing(0);
    m_hhLayout->setContentsMargins(13, 10, 10, 10);

    setLayout(m_hhLayout);
    connect(this, &CYearView::dateSelected, this, &CYearView::handleCurrentDateChanged);
    m_monthList.append( "一月" );
    m_monthList.append( "二月");
    m_monthList.append( "三月" );
    m_monthList.append( "四月" );
    m_monthList.append( "五月" );
    m_monthList.append( "六月" );
    m_monthList.append( "七月" );
    m_monthList.append( "八月");
    m_monthList.append( "九月" );
    m_monthList.append( "十月" );
    m_monthList.append( "十一月");
    m_monthList.append( "十二月");
    m_hightFont.setFamily("Helvetica");
    m_hightFont.setPixelSize(12);

    m_Scheduleview = new CYearSchceduleView(parent);
    //m_Scheduleview->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    //m_Scheduleview->setAttribute(Qt::WA_TranslucentBackground);
    //m_Scheduleview->setWindowFlags(m_Scheduleview->windowFlags()& Qt::WindowStaysOnTopHint);
    //m_Scheduleview->setVisible(false);
}
void CYearView::handleCurrentDateChanged(const QDate date, const CaLunarDayInfo &detail)
{
    return;
    Q_UNUSED(detail);


    if (date != m_currentDate) {
        setCurrentDate(date);
    }
}

void CYearView::setFirstWeekday(int weekday)
{
    m_firstWeekDay = weekday;
    updateDate();
}


void CYearView::updateSelectState()
{
    m_selectedCell = -1;
    for (int i(0); i != 42; ++i) {
        m_cellList.at(i)->update();
    }
}

void CYearView::setTheMe(int type)
{
    m_themetype = type;
    if (type == 0 || type == 1) {
        DPalette bpa = palette();
        bpa.setColor(DPalette::Background, Qt::white);
        setPalette(bpa);
        setBackgroundRole(DPalette::Background);
        // setBColor( Qt::white);
        m_currentMouth->setTextColor( QColor("#CF0059"));

        m_bnormalColor = "#FFFFFF";
        m_topBorderColor = Qt::red;
        m_backgroundCircleColor = "#0081FF";

        m_defaultTextColor = Qt::black;
        m_currentDayTextColor = "#0081FF";
        m_weekendsTextColor = Qt::black;
        m_selectedTextColor = Qt::white;
        m_festivalTextColor = Qt::black;
        m_notCurrentTextColor = "#b2b2b2";

    } else if (type == 2) {

        QColor framecolor("#FFFFFF");
        //framecolor.setAlphaF(0.15);
        framecolor = "#414141";
        framecolor.setAlphaF(0.3);
        DPalette bpa = palette();
        bpa.setColor(DPalette::Background, framecolor);
        setPalette(bpa);
        setBackgroundRole(DPalette::Background);
        m_bnormalColor = framecolor;
        //setBColor(m_bnormalColor);
        m_currentMouth->setTextColor( QColor("#BF1D63"));
        // m_currentMouth->setBColor(framecolor);

        m_topBorderColor = Qt::red;
        m_backgroundCircleColor = "#0059D2";

        m_defaultTextColor = "#C0C6D4";
        m_currentDayTextColor = "#0059D2";
        m_weekendsTextColor = Qt::black;
        m_selectedTextColor = Qt::white;
        m_festivalTextColor = Qt::black;
        m_notCurrentTextColor = "#C0C6D4";
        m_notCurrentTextColor.setAlphaF(0.5);
    }
    QColor monthcolor = Qt::white;
    monthcolor.setAlphaF(0);
    m_currentMouth->setBColor(monthcolor);
    m_Scheduleview->setTheMe(type);
}

void CYearView::updateHigh()
{
    for (int i = 0; i < 42; i++) {
        m_cellList.at(i)->update();
    }
}

void CYearView::setCurrentDate(const QDate date, int type)
{
    qDebug() << "set current date " << date;

    //if (date == m_currentDate) {
    //   return;
    // }

    m_currentDate = date;
    QLocale locale;
    if (locale.language() == QLocale::Chinese) {
        m_currentMouth->setTextStr(m_monthList.at(date.month() - 1));
    } else {
        m_currentMouth->setTextStr(locale.monthName(date.month(), QLocale::ShortFormat));
    }
    //QString monthName(int month, QLocale::FormatType type = LongFormat)

    //m_currentMouth->setText(QString::number(date.month()) + tr("Mon"));
    updateDate();
    if (type == 1)
        setSelectedCell(getDateIndex(date));
    emit signalcurrentDateChanged(date);
}

void CYearView::setCellSelectable(bool selectable)
{
    if (selectable == m_cellSelectable)
        return;
    m_cellSelectable = selectable;
}

int CYearView::getDateIndex(const QDate &date) const
{
    for (int i = 0; i != 42; ++i)
        if (m_days[i] == date)
            return i;

    return 0;
}

bool CYearView::eventFilter(QObject *o, QEvent *e)
{
    QWidget *cell = qobject_cast<QWidget *>(o);

    if (cell && m_cellList.contains(cell)) {
        if (e->type() == QEvent::Paint) {
            paintCell(cell);
        } else if (e->type() == QEvent::MouseButtonPress) {
            cellClicked(cell);

            const int pos = m_cellList.indexOf(cell);
            CScheduleDataManage *tdataManage = CScheduleDataManage::getScheduleDataManage();
            QString soloday;
            if (tdataManage->getHuangliDayDataManage()->getSoloDay(m_days[pos], soloday)) {
                m_Scheduleview->setSoloDay(soloday);
            }
            QVector<ScheduleDateRangeInfo> out;
            if (tdataManage->getscheduleDataCtrl()->getScheduleInfo(m_days[pos], m_days[pos], out)) {
                if (!out.isEmpty()) {
                    m_Scheduleview->setData(out[0].vData);
                }
            }
            int px = cell->x();
            int py = cell->y();
            //QPoint pos22 = mapToGlobal(QPoint(px, py));
            QPoint pos22 = QCursor::pos();
            QDesktopWidget *w = QApplication::desktop();
            QRect wR = w->screenGeometry(w->primaryScreen());

            m_Scheduleview->showWindow();
            int lfetorright = 0;
            int mw = pos22.x() + 10 + m_Scheduleview->width();
            if (mw > wR.width()) {
                mw = pos22.x() - 10 - m_Scheduleview->width();
                lfetorright = 1;
            } else {
                mw = pos22.x() + 10;
                lfetorright = 0;
            }
            int mh = pos22.y() + m_Scheduleview->height();
            if (mh > wR.height()) {
                mh = wR.height() - m_Scheduleview->height();
                m_Scheduleview->setDtype(lfetorright, pos22.y() - mh);
            } else {
                mh = pos22.y() - m_Scheduleview->height() / 2;
                m_Scheduleview->setDtype(lfetorright, m_Scheduleview->height() / 2);
            }
            m_Scheduleview->move(mw, mh);
            m_Scheduleview->show();
        } else if (e->type() == QEvent::MouseButtonDblClick) {
            const int pos = m_cellList.indexOf(cell);
            if (pos != -1) {
                emit signaldoubleclickDate(m_days[pos]);
            }
        }
        if (e->type() == QEvent::Leave) {
            const int pos = m_cellList.indexOf(cell);
            m_cellList[pos]->update();
            m_Scheduleview->hide();
            m_Scheduleview->clearData();
        } else if (e->type() == QEvent::ToolTip) {
            //return ;
//            const int pos = m_cellList.indexOf(cell);
//            CScheduleDataManage *tdataManage = CScheduleDataManage::getScheduleDataManage();
//            QString soloday;
//            if (tdataManage->getHuangliDayDataManage()->getSoloDay(m_days[pos], soloday)) {
//                m_Scheduleview->setSoloDay(soloday);
//            }
//            QVector<ScheduleDateRangeInfo> out;
//            if (tdataManage->getscheduleDataCtrl()->getScheduleInfo(m_days[pos], m_days[pos], out)) {
//                if (!out.isEmpty()) {
//                    m_Scheduleview->setData(out[0].vData);
//                }
//            }
//            int px = cell->x();
//            int py = cell->y();
//            //QPoint pos22 = mapToGlobal(QPoint(px, py));
//            QPoint pos22 = QCursor::pos();
//            QDesktopWidget *w = QApplication::desktop();
//            QRect wR = w->screenGeometry(w->primaryScreen());

//            m_Scheduleview->showWindow();
//            int lfetorright = 0;
//            int mw = pos22.x() + 10 + m_Scheduleview->width();
//            if (mw > wR.width()) {
//                mw = pos22.x() - 10 - m_Scheduleview->width();
//                lfetorright = 1;
//            } else {
//                mw = pos22.x() + 10;
//                lfetorright = 0;
//            }
//            int mh = pos22.y() + m_Scheduleview->height();
//            if (mh > wR.height()) {
//                mh = wR.height() - m_Scheduleview->height();
//                m_Scheduleview->setDtype(lfetorright, pos22.y() - mh);
//            } else {
//                mh = pos22.y() - m_Scheduleview->height() / 2;
//                m_Scheduleview->setDtype(lfetorright, m_Scheduleview->height() / 2);
//            }
//            m_Scheduleview->move(mw, mh);
//            m_Scheduleview->show();
        }
    }
    if (cell == m_currentMouth) {
        if (e->type() == QEvent::MouseButtonDblClick) {
            //emit signalselectMonth(QDate(m_currentDate.year(), m_currentDate.month(), 1));
            emit signalselectMonth(m_currentDate);
        }
    }
    return false;
}

void CYearView::updateDate()
{
    const QDate firstDay(m_currentDate.year(), m_currentDate.month(), 1);
    const int day = (firstDay.dayOfWeek() + m_firstWeekDay) % 7;
    const int currentIndex = day + m_currentDate.day() - 1;

    if (currentIndex < 0) {
        return;
    }

    for (int i(0); i != 42; ++i) {
        m_days[i] = firstDay.addDays(i - day);
        m_cellList.at(i)->update();
    }

    //setSelectedCell(currentIndex);
    update();
}
const QString CYearView::getCellDayNum(int pos)
{
    return QString::number(m_days[pos].day());
}

const QDate CYearView::getCellDate(int pos)
{
    return m_days[pos];
}
void CYearView::paintCell(QWidget *cell)
{
    const QRect rect(0, 0, cell->width(), cell->height());

    const int pos = m_cellList.indexOf(cell);
    const bool isSelectedCell = pos == m_selectedCell;
    const bool isCurrentDay = getCellDate(pos) == QDate::currentDate() && getCellDate(pos).month() == m_currentDate.month();

    QPainter painter(cell);
    bool highflag = false;
    if (getCellDate(pos).month() == m_currentDate.month()) {
        highflag = CScheduleDataManage::getScheduleDataManage()->getSearchResult(m_days[pos]);
    }
    if (highflag) {
        int hh = 0;
        QRect fillRect;
        int t_pix = (height() - 159) / 22.33;
        if (cell->width() > cell->height()) {
            hh = cell->height() - t_pix;
            fillRect = QRect((cell->width() - hh) / 2.0 + 0.5 + t_pix / 2, 0, hh, hh);
        } else {
            hh = cell->width() - t_pix;
            fillRect = QRect(t_pix / 2, (cell->height() - hh) / 2.0 + 0.5, hh, hh);
        }
        painter.setRenderHints(QPainter::HighQualityAntialiasing);
        painter.setBrush(QBrush(m_highColor));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(fillRect);
        painter.setPen(Qt::SolidLine);

        const QString dayNum = getCellDayNum(pos);
        painter.setPen(m_highTextColor);
        QRect test;
        painter.setFont(m_hightFont);
        painter.drawText(rect, Qt::AlignCenter, dayNum, &test);

    } else {
        //    painter.drawRoundedRect(cell->rect(), 4, 4);
#if 0
        // draw selected cell background circle
        if (isSelectedCell) {
            int hh = 0;
            QRect fillRect;
            if (cell->width() > cell->height()) {
                hh = cell->height() + 6;
                fillRect = QRect((cell->width() - hh) / 2.0 + 0.5 - 2,  cellwidth / cellheight, hh + 4, hh + 4);
            } else {
                hh = cell->width() + 6;
                fillRect = QRect(- 5, (cell->height() - hh) / 2.0 + hh * 0.0625 * cellheight / cellwidth, hh + 4, hh + 4);
            }
            QPixmap pixmap;
            if (m_themetype == 2)
                pixmap = DHiDPIHelper::loadNxPixmap(":/resources/icon/darkchoose20X20_checked .svg").scaled(hh + 4, hh + 4, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            else {
                pixmap = DHiDPIHelper::loadNxPixmap(":/resources/icon/choose20X20_checked .svg").scaled(hh + 4, hh + 4, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            }

            pixmap.setDevicePixelRatio(devicePixelRatioF());
            painter.save();
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setRenderHint(QPainter::HighQualityAntialiasing);
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            painter.drawPixmap(fillRect, pixmap);
            painter.restore();

            // painter.setRenderHints(QPainter::HighQualityAntialiasing);
            // painter.setBrush(QBrush(m_backgroundCircleColor));
            // painter.setPen(Qt::NoPen);
            // painter.drawEllipse(fillRect);
        }

        painter.setPen(Qt::SolidLine);

        const QString dayNum = getCellDayNum(pos);

        // draw text of day
        if (isSelectedCell) {
            painter.setPen(m_selectedTextColor);
        } else if (isCurrentDay) {
            painter.setPen(m_currentDayTextColor);
        } else {
            if (m_currentDate.month() == getCellDate(pos).month())
                painter.setPen(m_defaultTextColor);
            else
                painter.setPen(m_notCurrentTextColor);
        }

        //    painter.drawRect(rect);
        QRect test;
        painter.setFont(m_dayNumFont);

        painter.drawText(rect, Qt::AlignCenter, dayNum, &test);
#else
        // draw selected cell background circle
        if (isSelectedCell) {
            int hh = 0;
            QRect fillRect;
            if (cell->width() > cell->height()) {
                hh = cell->height();
                fillRect = QRect((cell->width() - hh) / 2.0 + 0.5 - 3, hh * 0.11 - 2, hh + 6, hh + 6);
            } else {
                hh = cell->width();
                fillRect = QRect(-3, (cell->height() - hh) / 2.0 + hh * 0.11 - 2, hh + 6, hh + 6);
            }
            QPixmap pixmap;
            if (m_themetype == 2)
                pixmap = DHiDPIHelper::loadNxPixmap(":/resources/icon/darkchoose30X30_checked .svg");
            else {
                pixmap = DHiDPIHelper::loadNxPixmap(":/resources/icon/choose30X30_checked .svg");
            }

            pixmap.setDevicePixelRatio(devicePixelRatioF());
            painter.save();
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setRenderHint(QPainter::HighQualityAntialiasing);
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            painter.drawPixmap(fillRect, pixmap);
            painter.restore();
        }

        painter.setPen(Qt::SolidLine);

        const QString dayNum = getCellDayNum(pos);

        // draw text of day
        if (isSelectedCell) {
            painter.setPen(m_selectedTextColor);
        } else if (isCurrentDay) {
            painter.setPen(m_currentDayTextColor);
        } else {
            if (m_currentDate.month() == getCellDate(pos).month())
                painter.setPen(m_defaultTextColor);
            else
                painter.setPen(m_notCurrentTextColor);
        }

        //    painter.drawRect(rect);
        QRect test;
        painter.setFont(m_dayNumFont);

        painter.drawText(rect, Qt::AlignCenter, dayNum, &test);
#endif
    }
    painter.end();
}

void CYearView::cellClicked(QWidget *cell)
{
    if (!m_cellSelectable)
        return;

    const int pos = m_cellList.indexOf(cell);
    if (pos == -1)
        return;

    setSelectedCell(pos);

    // my gift eggs
    static int gift = 0;
    if (m_days[pos] == QDate(1993, 7, 28))
        if (++gift == 10)
            QMessageBox::about(this, "LinuxDeepin", "by shibowen <sbw@sbw.so> :P");
}

void CYearView::setSelectedCell(int index)
{
    if (m_selectedCell == index)
        return;

    const int prevPos = m_selectedCell;
    m_selectedCell = index;

    updateDate();
    //m_cellList.at(prevPos)->update();
    //m_cellList.at(index)->update();
    if (m_days[index].year() < 1900) return;
    emit singanleActiveW(this);
    emit signalcurrentDateChanged(m_days[index]);
}

void CYearView::resizeEvent(QResizeEvent *event)
{
    //cellwidth = width() * 0.099 + 0.5;
    //cellheight = height() * 0.1257 + 0.5;

    m_gridLayout->setHorizontalSpacing(width() * 0.0297 + 0.5);
    m_gridLayout->setVerticalSpacing(height() * 0.0034 + 0.5);
    int leftmagin = width() * 0.06435 + 0.5;
    int rightmagin = leftmagin;
    int topmagin = height() * 0.02955 + 0.5;
    int buttonmagin = height() * 0.044 + 0.5;
    cellwidth = (width() - 2 * leftmagin) / 7;
    cellheight = (height() -  (24 + (height() - 159) / 12 - topmagin - buttonmagin)) / 6;
    m_hhLayout->setContentsMargins(leftmagin, topmagin, rightmagin, buttonmagin);
    m_dayNumFont.setPixelSize(12 + (height() - 159) / 22.33);
    m_hightFont.setPixelSize(12 + (height() - 159) / 22.33);
    m_momthFont.setPixelSize(16 + (height() - 159) / 16.75);
    m_currentMouth->setTextFont(m_momthFont);
    m_currentMouth->setFixedHeight(24 + (height() - 159) / 12);
    m_currentMouth->update();
    for (int i(0); i != 42; ++i) {
        m_cellList.at(i)->setFixedSize(cellwidth, cellheight);
    }
    QFrame::resizeEvent(event);
}

void CYearView::paintEvent(QPaintEvent *e)
{
    int labelwidth = width() - 2 * m_borderframew;
    int labelheight = height() - 2 * m_borderframew;

    QPainter painter(this);
    QRect fillRect = QRect(m_borderframew, m_borderframew, labelwidth, labelheight);
    if (1) {
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
        painter.setBrush(QBrush(m_bnormalColor));
        painter.setPen(Qt::NoPen);
        QPainterPath painterPath;
        painterPath.moveTo(m_radius, m_borderframew);
        if (1) {
            painterPath.arcTo(QRect(m_borderframew, m_borderframew, m_radius * 2, m_radius * 2), 90, 90);
        } else {
            painterPath.lineTo(m_borderframew, m_borderframew);
            painterPath.lineTo(m_borderframew, m_radius);
        }
        painterPath.lineTo(0, labelheight - m_radius);
        if (1) {
            painterPath.arcTo(QRect(m_borderframew, labelheight - m_radius * 2, m_radius * 2, m_radius * 2), 180, 90);
        } else {
            painterPath.lineTo(m_borderframew, labelheight);
            painterPath.lineTo(m_radius, labelheight);
        }
        painterPath.lineTo(labelwidth - m_radius, labelheight);
        if (1) {
            painterPath.arcTo(QRect(labelwidth - m_radius * 2, labelheight - m_radius * 2, m_radius * 2, m_radius * 2), 270, 90);
        } else {
            painterPath.lineTo(labelwidth, labelheight);
            painterPath.lineTo(labelwidth, labelheight - m_radius);
        }
        painterPath.lineTo(labelwidth, m_radius);
        //painterPath.moveTo(labelwidth, m_radius);
        if (1) {

            painterPath.arcTo(QRect(labelwidth - m_radius * 2, m_borderframew, m_radius * 2, m_radius * 2), 0, 90);

        } else {
            painterPath.lineTo(labelwidth, m_borderframew);
            painterPath.lineTo(labelwidth - m_radius, m_borderframew);
        }
        painterPath.lineTo(m_radius, m_borderframew);
        painterPath.closeSubpath();
        painter.drawPath(painterPath);
        painter.restore();
    }
}
