/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     chenhaifeng  <chenhaifeng@uniontech.com>
*
* Maintainer: chenhaifeng  <chenhaifeng@uniontech.com>
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
#ifndef CSCHEDULEMANAGE_H
#define CSCHEDULEMANAGE_H

#include "lunarmanager.h"

#include <com_deepin_daemon_timedate.h>
#include <QObject>

struct ShowDateRange {
    int showYear{0};
    QDate startDate;
    QDate stopDate;
};

typedef  com::deepin::daemon::Timedate DaemonTimeDate;
class CScheduleBaseWidget;
/**
 * @brief The CalendarManage class
 *  日历数据管理类
 */
class CalendarManager : public QObject
{
    Q_OBJECT
public:
    static CalendarManager *getInstance();
    static void releaseInstance();

    //设置选择时间
    void setSelectDate(const QDate &selectDate, bool isSwitchYear = false);
    //获取选择时间
    QDate getSelectDate() const;
    //设置当前时间
    void setCurrentDateTime(const QDateTime &currentDateTime = QDateTime::currentDateTime());
    //获取当前时间
    QDateTime getCurrentDate() const;
    //获取月份的所有时间
    QVector<QDate> getMonthDate(const int &year, const int &month);
    //获取一周的所有时间
    QVector<QDate> getWeekDate(const QDate &date);
    //设置周显示格式
    void setWeekDayFormatByID(const int &weekDayFormatID);
    //获取周显示格式
    QString getWeekDayFormat() const;
    //返回显示的年份,开始和结束时间
    ShowDateRange getShowDateRange() const;
    //根据日期获取当前周第一天的日期
    QDate getFirstDayOfWeek(const QDate &date);
    //根据日期获取该日期处于该年第多少周
    static int getWeekNumOfYear(const QDate &date);

    void setTimeFormatChanged(int value, bool update = true);
    void setDateFormatChanged(int value);
    QString getTimeFormat() const;
    int getTimeFormatValue() const;
    QString getDateFormat() const;

    //添加显示界面
    void addShowWidget(CScheduleBaseWidget *showWidget);
    //移除显示界面
    void removeShowWidget(CScheduleBaseWidget *showWidget);
    //根据编号获取显示界面
    CScheduleBaseWidget *getShowWidget(const int index);
    //获取显示窗口的数目
    int  getShowWidgetSize();
    //获取是否显示农历信息
    bool getShowLunar() const;
    //获取一周首日
    Qt::DayOfWeek getFirstDayOfWeek();
    //设置一周首日
    void setFirstDayOfWeek(int, bool update = true);
    void updateData();

signals:
    void signalTimeFormatChanged(int value);
    void signalDateFormatChanged(int value);

public slots:
    //关联dbus信号，每周首日改变事触发
    void weekBeginsChanged(int value);
    //农历更新成功刷新界面
    void slotGetLunarSuccess();

    void slotGeneralSettingsUpdate();

    void slotDateFormatChanged(int value);

private:
    explicit CalendarManager(QObject *parent = nullptr);
    ~CalendarManager();

    void setYearBeginAndEndDate(const int year);
private:
    //初始化数据
    void initData();
    //初始化关联
    void initConnection();

private:
    static CalendarManager      *m_scheduleManager;
    DaemonTimeDate              *m_timeDateDbus;    //控制中心dbus
    QList<CScheduleBaseWidget*> m_showWidget;  //日历主视图
    bool                        m_showLunar;    //显示农历
    int                         m_firstDayOfWeek = Qt::Sunday;  //一周首日
    QDateTime                   m_currentDateTime;  //当前时间
    QDate                       m_selectDate;       //当前选择的时间
    QString                     m_weekDayFormat{"ddd"}; //周显示格式， ddd: 周一， dddd: 星期一
    ShowDateRange               m_showDateRange;        //时间范围
    QString                     m_timeFormat = "h:mm";  //时间显示格式
    QString                     m_dateFormat = "yyyy-MM-dd";    //日期显示格式
    int                         m_timeFormatValue = 0;
};
#define gCalendarManager CalendarManager::getInstance()
#endif // CSCHEDULEMANAGE_H
