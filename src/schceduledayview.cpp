﻿/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
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
#include "schceduledayview.h"
#include <QAction>
#include <QMenu>
#include <QListWidget>
#include <QLabel>
#include <QPainter>
#include <QHBoxLayout>
#include <QStylePainter>
#include <QRect>
#include "dbmanager.h"
#include "schceduledlg.h"
CSchceduleWidgetItem::CSchceduleWidgetItem( QWidget *parent /*= nullptr*/ ): QPushButton(parent)
{
    //setMargin(0);
    m_editAction = new QAction(tr("Edit"), this);
    m_deleteAction = new QAction(tr("Delete"), this);
    connect(m_editAction, SIGNAL(triggered(bool)), this, SLOT(slotEdit()));
    connect(m_deleteAction, SIGNAL(triggered(bool)), this, SLOT(slotDelete()));
    m_item = NULL;
}

void CSchceduleWidgetItem::setColor( QColor color1, QColor color2, bool GradientFlag /*= false*/ )
{
    m_color1 = color1;
    m_color2 = color2;
    m_GradientFlag = GradientFlag;
}

void CSchceduleWidgetItem::setText( QColor tcolor, QFont font, QPoint pos, bool avgeflag)
{
    m_textcolor = tcolor;
    m_font = font;
    m_pos = pos;
    m_avgeflag = avgeflag;
}

void CSchceduleWidgetItem::getColor(QColor &color1, QColor &color2, bool &GradientFlag)
{
    color2 = m_color2;
    color1 = m_color1;
    GradientFlag = m_GradientFlag;
}

void CSchceduleWidgetItem::getText(QColor &tcolor, QFont &font, QPoint &pos)
{
    tcolor = m_textcolor;
    font = m_font;
    pos = m_pos;
}

void CSchceduleWidgetItem::setData( ScheduleInfo vScheduleInfo )
{
    m_ScheduleInfo = vScheduleInfo;
    setToolTip(m_ScheduleInfo.titleName);
    update();
}
void CSchceduleWidgetItem::slotEdit()
{
    CSchceduleDlg dlg(0, this);
    dlg.setData(m_ScheduleInfo);
    if (dlg.exec() == DDialog::Accepted) {
        ScheduleInfo info = dlg.getData();
        info.id = m_ScheduleInfo.id;
        ScheduleDbManager::updateScheduleInfo(info);
        emit signalsEdit(this);
    }
}

void CSchceduleWidgetItem::slotDelete()
{
    emit signalsDelete(this);
    ScheduleDbManager::deleteScheduleInfoById(m_ScheduleInfo.id);
}

void CSchceduleWidgetItem::paintEvent( QPaintEvent *e )
{
    int labelwidth = width();
    int labelheight = height();
    float avge = 1;
    if (m_avgeflag) {
        avge = 0.5;
    }
    QPainter painter(this);
    if (m_GradientFlag) {
        QLinearGradient linearGradient(0, 0, labelwidth, 0);
        linearGradient.setColorAt(0, m_color1);
        linearGradient.setColorAt(1, m_color2);
        QRect fillRect = QRect(2, 2 * avge, labelwidth - 2, labelheight - 2 * avge);
        //将直线开始点设为0，终点设为1，然后分段设置颜色
        painter.setRenderHints(QPainter::HighQualityAntialiasing);
        painter.setBrush(linearGradient);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(fillRect, 3, 3 * avge);

        painter.setFont(m_font);
        painter.setPen(m_textcolor);
        QFontMetrics fm = painter.fontMetrics();
        QString str = "-" + m_ScheduleInfo.titleName;
        if (fm.width(str) > width()) {
            int widthT = fm.width(str);
            int singlecharw = widthT * 1.0 / str.count();
            int rcharcount = width() * 1.0 / singlecharw;
            QString tstr;
            for (int i = 0; i < rcharcount - 8; i++) {
                tstr.append(str.at(i));
            }
            str = tstr + "...";
        }
        painter.drawText(QRect(m_pos.x(), m_pos.y(), labelwidth - m_pos.x(), m_pos.y() + 2 * avge), Qt::AlignLeft, str);
    } else {
        QRect fillRect = QRect(2, 2 * avge, labelwidth - 2, labelheight - 2 * avge);
        //将直线开始点设为0，终点设为1，然后分段设置颜色
        painter.setRenderHints(QPainter::HighQualityAntialiasing);
        painter.setBrush(m_color1);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(fillRect, 3, 3);

        painter.setFont(m_font);
        painter.setPen(m_textcolor);
        QFontMetrics fm = painter.fontMetrics();
        QString str = m_ScheduleInfo.titleName;
        if (fm.width(str) > width()) {
            int widthT = fm.width(str);
            int singlecharw = widthT * 1.0 / str.count();
            int rcharcount = width() * 1.0 / singlecharw;
            QString tstr;
            for (int i = 0; i < rcharcount - 8; i++) {
                tstr.append(str.at(i));
            }
            str = tstr + "...";
        }
        painter.drawText(QRect(m_pos.x(), m_pos.y(), labelwidth - m_pos.x(), m_pos.y() + 4 * avge), Qt::AlignLeft, str);
    }
}
void CSchceduleWidgetItem::contextMenuEvent( QContextMenuEvent *event )
{
    QMenu Context(this);
    Context.addAction(m_editAction);
    Context.addAction(m_deleteAction);
    Context.exec(QCursor::pos());
}
CSchceduleNumButton::CSchceduleNumButton(QWidget *parent /*= nullptr*/): QPushButton(parent)
{

}

CSchceduleNumButton::~CSchceduleNumButton()
{

}

void CSchceduleNumButton::setColor( QColor color1, QColor color2, bool GradientFlag /*= false*/ )
{
    m_color1 = color1;
    m_color2 = color2;
    m_GradientFlag = GradientFlag;
}

void CSchceduleNumButton::setText( QColor tcolor, QFont font, QPoint pos)
{
    m_textcolor = tcolor;
    m_font = font;
    m_pos = pos;
}

void CSchceduleNumButton::paintEvent(QPaintEvent *e)
{
    int labelwidth = width();
    int labelheight = height();

    QPainter painter(this);
    if (m_GradientFlag) {
        QLinearGradient linearGradient(0, 0, labelwidth, 0);
        linearGradient.setColorAt(0, m_color1);
        linearGradient.setColorAt(1, m_color2);
        QRect fillRect = QRect(2, 1, labelwidth - 2, labelheight - 1);
        //将直线开始点设为0，终点设为1，然后分段设置颜色
        painter.setRenderHints(QPainter::HighQualityAntialiasing);
        painter.setBrush(linearGradient);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(fillRect, 3, 2);

        painter.setFont(m_font);
        painter.setPen(m_textcolor);
        painter.drawText(QRect(m_pos.x(), m_pos.y(), labelwidth - m_pos.x(), m_pos.y() + 3), Qt::AlignLeft, "-" + QString("There is %1 schedule").arg(m_num));
    } else {
        QRect fillRect = QRect(2, 1, labelwidth - 2, labelheight - 1);
        //将直线开始点设为0，终点设为1，然后分段设置颜色
        painter.setRenderHints(QPainter::HighQualityAntialiasing);
        painter.setBrush(m_color1);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(fillRect, 3, 2);

        painter.setFont(m_font);
        painter.setPen(m_textcolor);
        painter.drawText(QRect(m_pos.x(), m_pos.y(), labelwidth - m_pos.x(), m_pos.y() + 3), Qt::AlignLeft, QString("There is %1 schedule").arg(m_num));
    }
}


void CSchceduleDayView::setALLDayData( QVector<ScheduleInfo> &vlistData )
{
    m_vlistData = vlistData;
    m_numButton->setColor(QColor("#DFB3FF"), QColor("#FBE9B7"), true);
    QFont font("PingFangSC-Light");
    font.setPixelSize(6);
    m_numButton->setText(QColor("#000000"), font, QPoint(23, 6));
    m_numButton->setData(vlistData.count());
    m_type = 0;
    updateDateShow();
}

void CSchceduleDayView::setDayData(QVector<ScheduleInfo> &vlistData)
{
    m_vlistData = vlistData;
    m_numButton->setColor(QColor("#DFB3FF"), QColor("#FBE9B7"), true);
    QFont font("PingFangSC-Light");
    font.setPixelSize(6);
    m_numButton->setText(QColor("#000000"), font, QPoint(8, 5));
    m_numButton->setData(vlistData.count());
    m_type = 1;
    updateDateShow();
}

void CSchceduleDayView::setDate(QDate date, int type)
{
    m_currentDate = date;
    if (type) {
        m_vlistData = ScheduleDbManager::getScheduleInfo(date, 0);
    } else {
        m_vlistData = ScheduleDbManager::getALLDayScheduleInfo(date);
    }
    m_type = type;
    updateDateShow();
}

CSchceduleDayView::CSchceduleDayView(QWidget *parent) : QWidget(parent)
{
    m_widt = new QWidget(parent);

    m_widt->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    m_widt->setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout *layout = new QVBoxLayout;
    m_widgetFlag = false;

    m_layout = new QVBoxLayout;
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_gradientItemList = new QListWidget(parent);
    m_gradientItemList->setAlternatingRowColors(true);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_gradientItemList);

    m_widt->setLayout(layout);
    m_widt->hide();

    m_numButton = new CSchceduleNumButton(this);
    m_numButton->setColor(Qt::white, Qt::white, false);

    m_numButton->hide();
    m_layout->addWidget(m_numButton);
    connect(m_numButton, SIGNAL(clicked()), this, SLOT(slothidelistw()));
    // set default row
    m_gradientItemList->setCurrentRow(0);
    setLayout(m_layout);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    m_createAction = new QAction(tr("Create"), this);
    connect(m_createAction, &QAction::triggered, this, &CSchceduleDayView::slotCreate);
}

CSchceduleDayView::~CSchceduleDayView()
{

}
void CSchceduleDayView::slothidelistw()
{
    m_widgetFlag = !m_widgetFlag;

    if (m_widgetFlag) {
        int px = x();
        int py = y();
        QPoint pos = mapToGlobal(QPoint(px, py));
        int heighty = height() / 2 * m_gradientItemList->count();
        //if (heighty > window()->height() - this->y())
        // heighty = this->y();
        m_widt->resize(this->width(), heighty);
        m_widt->move(pos.x() - px, pos.y() + height() - py);
        m_widt->show();
    } else {
        m_widt->hide();
    }
}

void CSchceduleDayView::slotCreate()
{
    CSchceduleDlg dlg(1, this);
    QDateTime tDatatime;
    tDatatime.setDate(m_currentDate);
    dlg.setDate(tDatatime);
    if (dlg.exec() == DDialog::Accepted) {
        ScheduleInfo info = dlg.getData();
        info.id = ScheduleDbManager::addSchedule(info);
        m_vlistData.append(info);
        emit signalsUpdateShcedule(info.id);
        //updateDateShow();
    }
}

void CSchceduleDayView::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_vlistData.isEmpty()) {
        QMenu Context(this);
        Context.addAction(m_createAction);
        Context.exec(QCursor::pos());
    }
}

void CSchceduleDayView::updateDateShow()
{
    //remove
    for (int i = 0; i < m_gradientItemList->count(); i++) {
        QListWidgetItem *item11 = m_gradientItemList->takeItem(i);
        m_gradientItemList->removeItemWidget(item11);
    }
    m_gradientItemList->clear();
    for (int i = 0; i < m_baseShowItem.count(); i++) {
        //m_baseShowItem[i]->setParent(NULL);
        m_layout->removeWidget(m_baseShowItem[i]);
        m_baseShowItem[i]->deleteLater();
    }
    m_baseShowItem.clear();
    m_gradientItemList->clear();
    m_numButton->hide();
    if (m_vlistData.count() == 1) {
        m_numButton->hide();
        CSchceduleWidgetItem *gwi = createItemWidget(0);
        m_layout->insertWidget(0, gwi);
        m_baseShowItem.append(gwi);
    } else if (m_vlistData.count() == 2) {
        m_numButton->hide();
        m_widt->hide();
        CSchceduleWidgetItem *gwiup = createItemWidget(0, true);
        CSchceduleWidgetItem *gwidown = createItemWidget(1, true);
        m_layout->insertWidget(0, gwiup);
        m_baseShowItem.append(gwiup);
        m_layout->insertWidget(0, gwidown);
        m_baseShowItem.append(gwidown);
    } else if (m_vlistData.count() >= 3) {
        CSchceduleWidgetItem *gwiup = createItemWidget(0, true);
        m_layout->insertWidget(0, gwiup);
        m_baseShowItem.append(gwiup);
        m_numButton->show();
        CSchceduleWidgetItem *firstgwi = NULL;
        for (int i = 1; i < m_vlistData.size(); ++i) {
            CSchceduleWidgetItem *gwi = createItemWidget(i, true);
            QListWidgetItem *listItem = new QListWidgetItem;
            m_gradientItemList->addItem(listItem);
            m_gradientItemList->setItemWidget(listItem, gwi);
            gwi->setItem(listItem);
            if (i == 1) firstgwi = gwi;
        }
        QColor color1;
        QColor color2;
        bool GradientFlag;
        QColor tcolor;
        QFont font;
        QPoint pos;
        firstgwi->getColor(color1, color2, GradientFlag);
        firstgwi->getText(tcolor, font, pos);
        //m_numButton->setColor(color1, color2, GradientFlag);
        m_numButton->setText(tcolor, font, pos);
        m_numButton->setData(m_vlistData.count() - 1);
        m_numButton->update();
    }
}

CSchceduleWidgetItem *CSchceduleDayView::createItemWidget(int index, bool average)
{
    const ScheduleInfo &gd = m_vlistData.at(index);
    CSchceduleWidgetItem *gwi = new CSchceduleWidgetItem();
    if (m_type == 0) {
        gwi->setColor(QColor("#DFB3FF"), QColor("#FBE9B7"), true);
        QFont font("PingFangSC-Light");
        if (average) {
            font.setPixelSize(6);
        } else {
            font.setPixelSize(12);
        }
        gwi->setData(gd);
        if (average) {
            gwi->setFixedSize(width(), height() / 2);
            gwi->setText(QColor("#000000"), font, QPoint(23, 6), average);
        } else {
            gwi->setFixedSize(width(), height());
            gwi->setText(QColor("#000000"), font, QPoint(23, 13), average);
        }
        gwi->setItem(NULL);
    } else {
        gwi->setColor(ScheduleDbManager::getTypeColor(gd.infotype), QColor("#FBE9B7"), false);
        QFont font("PingFangSC-Light");
        if (average) {
            font.setPixelSize(6);
        } else {
            font.setPixelSize(12);
        }
        if (average) {
            gwi->setFixedSize(width(), height() / 2);
            gwi->setText(QColor("#000000"), font, QPoint(8, 5), average);
        } else {
            gwi->setFixedSize(width(), height());
            gwi->setText(QColor("#000000"), font, QPoint(8, 10), average);
        }
        gwi->setData(gd);
        gwi->setItem(NULL);
    }
    connect(gwi, &CSchceduleWidgetItem::signalsDelete, this, &CSchceduleDayView::slotdeleteitem);
    connect(gwi, &CSchceduleWidgetItem::signalsEdit, this, &CSchceduleDayView::slotedititem);
    return gwi;
}

void CSchceduleDayView::slotdeleteitem( CSchceduleWidgetItem *item )
{
    emit signalsUpdateShcedule(item->getData().id);
    for (int i = 0; i < m_vlistData.count(); i++) {
        if (m_vlistData.at(i).id == item->getData().id) {
            m_vlistData.removeAt(i);
            break;
        }
    }
    if (item != NULL) {
        int row = m_gradientItemList->row(item->getItem());
        QListWidgetItem *item11 = m_gradientItemList->takeItem(row);
        m_gradientItemList->removeItemWidget(item11);
    }
    updateDateShow();
    update();
}

void CSchceduleDayView::slotedititem(CSchceduleWidgetItem *item)
{
    emit signalsUpdateShcedule(item->getData().id);
}
