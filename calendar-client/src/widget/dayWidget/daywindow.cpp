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
#include "daywindow.h"
#include "constants.h"
#include "customframe.h"
#include "daymonthview.h"
#include "schedulesearchview.h"
#include "scheduleview.h"

#include <DPalette>

#include <QHBoxLayout>
#include <QMessageBox>

DGUI_USE_NAMESPACE
CDayWindow::CDayWindow(QWidget *parent)
    : CScheduleBaseWidget(parent)
{
    initUI();
    initConnection();
    setLunarVisible(m_calendarManager->getShowLunar());
}

CDayWindow::~CDayWindow() {}
void CDayWindow::setTheMe(int type)
{
    if (type == 0 || type == 1) {
        m_leftground->setBColor("#FFFFFF");

        DPalette ypa = m_YearLabel->palette();
        ypa.setColor(DPalette::WindowText, QColor("#3B3B3B"));
        m_YearLabel->setPalette(ypa);
        m_YearLabel->setForegroundRole(DPalette::WindowText);
        DPalette lpa = m_LunarLabel->palette();
        lpa.setColor(DPalette::WindowText, QColor("#8A8A8A"));
        m_LunarLabel->setPalette(lpa);
        m_LunarLabel->setForegroundRole(DPalette::WindowText);
        DPalette spa = m_SolarDay->palette();
        spa.setColor(DPalette::WindowText, Qt::red);
        m_SolarDay->setPalette(spa);
        m_SolarDay->setForegroundRole(DPalette::WindowText);
    } else if (type == 2) {
        m_leftground->setBColor("#282828");

        DPalette ypa = m_YearLabel->palette();
        ypa.setColor(DPalette::WindowText, QColor("#C0C6D4"));
        m_YearLabel->setPalette(ypa);
        m_YearLabel->setForegroundRole(DPalette::WindowText);
        DPalette lpa = m_LunarLabel->palette();
        lpa.setColor(DPalette::WindowText, QColor("#798BA8"));
        m_LunarLabel->setPalette(lpa);
        m_LunarLabel->setForegroundRole(DPalette::WindowText);
        DPalette spa = m_SolarDay->palette();
        spa.setColor(DPalette::WindowText, Qt::red);
        m_SolarDay->setPalette(spa);
        m_SolarDay->setForegroundRole(DPalette::WindowText);
    }
    m_daymonthView->setTheMe(type);
    m_scheduleView->setTheMe(type);
}

/**
 * @brief CDayWindow::setTime           设置显示时间位置
 * @param time
 */
void CDayWindow::setTime(const QTime time)
{
    if (time.isValid()) {
        //如果时间有效
        m_scheduleView->setTime(time);
    } else {
        if (getSelectDate() == getCurrendDateTime().date()) {
            m_scheduleView->setTime(getCurrendDateTime().time());
        } else {
            m_scheduleView->setTime(m_makeTime);
        }
    }
}

/**
 * @brief CDayWindow::updateHigh            更新全天和非全天高度
 */
void CDayWindow::updateHigh()
{
    m_scheduleView->updateHigh();
}

/**
 * @brief CDayWindow::setCurrentDateTime    设置当前时间
 * @param currentDate
 */
void CDayWindow::setCurrentDateTime(const QDateTime &currentDate)
{
    //设置当前时间
    CScheduleBaseWidget::setCurrentDateTime(currentDate);
    //如果选择日期为当前日期,则更新当前时间
    if (getSelectDate() == currentDate.date()) {
        m_scheduleView->setCurrentDate(getCurrendDateTime());
    }
}

/**
 * @brief CDayWindow::setYearData   设置选择时间年信息显示
 */
void CDayWindow::setYearData()
{
    QLocale locale;
    //判断是否为中文环境
    if (getShowLunar()) {
        m_YearLabel->setText(QString::number(getSelectDate().year()) + tr("Y") +
                             QString::number(getSelectDate().month()) + tr("M") +
                             QString::number(getSelectDate().day()) + tr("D"));
    } else {
        m_YearLabel->setText(locale.toString(getSelectDate(), "yyyy/M/d"));
    }
}

/**
 * @brief CDayWindow::updateShowDate        更新显示时间
 * @param isUpdateBar
 */
void CDayWindow::updateShowDate(const bool isUpdateBar)
{
    Q_UNUSED(isUpdateBar)
    setYearData();
    int w = m_scheduleView->width() - 72;
    m_scheduleView->setRange(w, 1032, getSelectDate(), getSelectDate());
    //获取需要显示的时间
    QVector<QDate> _monthDate = m_calendarManager->getCalendarDateDataManage()->getMonthDate(getSelectDate().year(), getSelectDate().month());
    m_daymonthView->setShowDate(_monthDate, getSelectDate(), getCurrendDateTime().date());
    //如果为中文环境则显示农历信息
    if (getShowLunar())
        updateShowLunar();
    updateShowSchedule();
}

/**
 * @brief CDayWindow::updateShowSchedule    更新日程显示
 */
void CDayWindow::updateShowSchedule()
{
    //获取一天的日程信息
    QMap<QDate, QVector<ScheduleDataInfo> > _weekScheduleInfo = m_calendarManager->getScheduleTask()->getScheduleInfo(getSelectDate(), getSelectDate());
    //设置显示日程信息
    m_scheduleView->setShowScheduleInfo(_weekScheduleInfo);
    //获取界面显示定位时间位置
    setMakeTime(_weekScheduleInfo);
    QMap<QDate, bool> _fullInfo = m_calendarManager->getScheduleTask()->getDateHasSchedule();
    //获取当前月是否包含日程情况
    QVector<QDate> _monthDate = m_calendarManager->getCalendarDateDataManage()->getMonthDate(getSelectDate().year(), getSelectDate().month());
    QVector<bool> _monthFlag{};
    for (int i = 0; i < _monthDate.size(); ++i) {
        if (_fullInfo.contains(_monthDate.at(i))) {
            _monthFlag.append(_fullInfo[_monthDate.at(i)]);
        } else {
            _monthFlag.append(false);
        }
    }
    //设置日视图右侧月显示日期是否有日程
    m_daymonthView->setHasScheduleFlag(_monthFlag);
}

/**
 * @brief CDayWindow::updateShowLunar       更新显示农历信息
 */
void CDayWindow::updateShowLunar()
{
    CaHuangLiDayInfo _huangliInfo = getLunarInfo();
    m_LunarLabel->setText(_huangliInfo.mLunarMonthName + _huangliInfo.mLunarDayName);
    m_daymonthView->setHuangliInfo(_huangliInfo);
}

/**
 * @brief CDayWindow::setSelectSearchScheduleInfo       设置选中日程
 * @param info
 */
void CDayWindow::setSelectSearchScheduleInfo(const ScheduleDataInfo &info)
{
    if (info.getAllDay()) {
        setTime();
    } else {
        m_scheduleView->setTime(info.getBeginDateTime().time());
    }

    m_scheduleView->setSelectSchedule(info);
}

void CDayWindow::setSearchWFlag(bool flag)
{
    m_searchfalg = flag;
    m_daymonthView->setSearchFlag(flag);
}

void CDayWindow::setLunarVisible(bool state)
{
    m_LunarLabel->setVisible(state);
    m_SolarDay->setVisible(state);
    m_scheduleView->setLunarVisible(state);
    m_daymonthView->setLunarVisible(state);
}

void CDayWindow::initUI()
{
    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->setMargin(0);
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(10, 9, 0, 3);

    m_YearLabel = new QLabel();
    m_YearLabel->setMinimumHeight(DDEDayCalendar::D_YLableHeight);
    QFont labelF;
    labelF.setWeight(QFont::Medium);
    labelF.setPixelSize(DDECalendar::FontSizeTwentyfour);
    m_YearLabel->setFont(labelF);
    DPalette ypa = m_YearLabel->palette();
    ypa.setColor(DPalette::WindowText, QColor("#3B3B3B"));
    m_YearLabel->setPalette(ypa);
    titleLayout->addWidget(m_YearLabel);
    m_LunarLabel = new QLabel();
    titleLayout->addSpacing(15);
    m_LunarLabel->setFixedHeight(DDEDayCalendar::D_YLableHeight);
    labelF.setPixelSize(DDECalendar::FontSizeFourteen);
    m_LunarLabel->setFont(labelF);
    m_LunarLabel->setAlignment(Qt::AlignCenter);
    DPalette lpa = m_LunarLabel->palette();
    lpa.setColor(DPalette::WindowText, QColor("#8A8A8A"));
    m_LunarLabel->setPalette(lpa);
    titleLayout->addWidget(m_LunarLabel);
    m_SolarDay = new QLabel();
    labelF.setPixelSize(DDECalendar::FontSizeTen);
    m_SolarDay->setFixedHeight(DDEDayCalendar::D_YLableHeight);
    m_SolarDay->setFont(labelF);
    m_SolarDay->setAlignment(Qt::AlignCenter);
    DPalette spa = m_SolarDay->palette();
    spa.setColor(DPalette::WindowText, Qt::red);
    m_SolarDay->setPalette(spa);
    titleLayout->addWidget(m_SolarDay);
    titleLayout->addStretch();

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->setMargin(0);
    leftLayout->setSpacing(0);
    m_scheduleView = new CScheduleView(this, 1);
    m_scheduleView->setviewMagin(72, 109, 20, 0);
    m_scheduleView->setCurrentDate(getCurrendDateTime());
    leftLayout->addLayout(titleLayout);
    leftLayout->addWidget(m_scheduleView);

    m_verline = new DVerticalLine;
    m_verline->setFixedWidth(2);

    m_daymonthView = new CDayMonthView(this);

    QHBoxLayout *lfetmainLayout = new QHBoxLayout;
    lfetmainLayout->setMargin(0);
    lfetmainLayout->setSpacing(1);
    lfetmainLayout->setContentsMargins(0, 0, 0, 0);
    lfetmainLayout->addLayout(leftLayout);
    lfetmainLayout->addWidget(m_verline);

    lfetmainLayout->addWidget(m_daymonthView);
    m_leftground = new CustomFrame();
    m_leftground->setRoundState(true, true, true, true);
    m_leftground->setLayout(lfetmainLayout);
    m_leftground->setBColor("#FFFFFF");

    m_mainLayout = new QHBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->addWidget(m_leftground);

    this->setLayout(m_mainLayout);
}

void CDayWindow::initConnection()
{
    connect(m_daymonthView, &CDayMonthView::signalIsDragging, this, &CDayWindow::slotIsDragging);
    connect(m_daymonthView, &CDayMonthView::signalChangeSelectDate, this, &CDayWindow::slotChangeSelectDate);
    connect(m_scheduleView, &CScheduleView::signalViewtransparentFrame, this, &CDayWindow::signalViewtransparentFrame);
}

/**
 * @brief CDayWindow::setMakeTime       界面显示定位时间位置
 * @param info
 */
void CDayWindow::setMakeTime(QMap<QDate, QVector<ScheduleDataInfo> > &info)
{
    if (info.contains(getSelectDate())) {
        QVector<ScheduleDataInfo> _scheduleVector = info[getSelectDate()];
        QVector<ScheduleDataInfo> _partTimeScheduleVector;
        //获取非全天日程
        for (int i = 0 ; i < _scheduleVector.size(); ++i) {
            if (!_scheduleVector.at(i).getAllDay())
                _partTimeScheduleVector.append(_scheduleVector.at(i));
        }
        if (_partTimeScheduleVector.size() == 0) {
            m_makeTime.setHMS(13, 0, 0, 0);
        } else {
            std::sort(_partTimeScheduleVector.begin(), _partTimeScheduleVector.end());
            //如果日程开始日期为选择日期则获取日程开始的时间
            if (_partTimeScheduleVector.begin()->getBeginDateTime().date() == getSelectDate()) {
                m_makeTime = _partTimeScheduleVector.begin()->getBeginDateTime().time();
            } else {
                m_makeTime.setHMS(0, 0, 0, 0);
            }
        }
    } else {
        m_makeTime.setHMS(13, 0, 0, 0);
    }
}

void CDayWindow::slotScheduleHide()
{
    m_scheduleView->slotScheduleShow(false);
}

/**
 * @brief CDayWindow::slotChangeSelectDate          更新选择时间
 * @param date
 */
void CDayWindow::slotChangeSelectDate(const QDate &date)
{
    if (setSelectDate(date, true)) {
        updateShowDate();
        setTime();
    }
}

void CDayWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    qreal dw = 0.4046 * width();
    int dh = height() - 20;

    if (m_searchfalg) {
        m_mainLayout->setContentsMargins(10, 10, 0, 10);
    } else {
        m_mainLayout->setContentsMargins(10, 10, 10, 10);
    }
    if (dw < 350)
        dw = 350;
    m_daymonthView->setFixedSize(qRound(dw), dh);
}

void CDayWindow::slotIsDragging(bool &isDragging)
{
    isDragging = m_scheduleView->IsDragging();
}
