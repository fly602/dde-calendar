/*
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
#ifndef DAYMONTHVIEW_H
#define DAYMONTHVIEW_H
#include "constants.h"
#include <QWidget>
#include <QList>
#include <QDate>
#include <QStyleOption>
#include <QSignalMapper>
#include <DArrowButton>
#include <dbasebutton.h>
#include <DLabel>
#include "calendardbus.h"
#include <DFrame>
DWIDGET_USE_NAMESPACE
class QLabel;
class CDayHuangLiLabel;
class CDayMonthView: public QWidget
{
    Q_OBJECT

public:
    explicit CDayMonthView(QWidget *parent = 0);
    void setFirstWeekday(int weekday);
    int getDateType(const QDate &date);
    inline bool cellSelectable() const
    {
        return m_cellSelectable;
    }
    void updateSelectState();
    void setLunarVisible(bool visible);
signals:
    void dateSelected(const QDate date, const CaHuangLiDayInfo &detail) const;
    void signalcurrentDateChanged(QDate date);
    void signalcurrentLunarDateChanged(QDate date,  CaHuangLiDayInfo detail, int type = 0);
    void cellSelectableChanged(bool cellSelectable) const;
public slots:
    void setCurrentDate(const QDate date, int type = 0);
    void setCellSelectable(bool selectable);
    void handleCurrentDateChanged(const QDate date, const CaHuangLiDayInfo &detail);

private:
    int getDateIndex(const QDate &date) const;
    const QString getCellDayNum(int pos);
    const QDate getCellDate(int pos);
    void paintCell(QWidget *cell);
    bool eventFilter(QObject *o, QEvent *e);
    void updateDate();

    const QString getLunar(int pos);
    const CaHuangLiDayInfo getCaHuangLiDayInfo(int pos);
    void updateCurrentLunar(const CaHuangLiDayInfo &info);

    void initUI();
    void initConnection();

    void updateDateShow();
    void updateDateLunarDay();
private slots:
    void cellClicked(QWidget *cell);
    void setSelectedCell(int index);
    void slotprev();
    void slotnext();
    void slottoday();
    void getDbusData();
private:
    DArrowButton      *m_prevButton = nullptr;
    DArrowButton      *m_nextButton = nullptr;
    DBaseButton       *m_today = nullptr;
    DLabel *m_currentMouth;
    DLabel *m_currentDay;
    DLabel *m_currentYear;
    DLabel *m_currentWeek;
    DLabel *m_currentLuna;
    CDayHuangLiLabel *m_yiLabel;
    CDayHuangLiLabel *m_jiLabel;
    QList<QWidget *> m_cellList;
    QDate m_days[42];
    QDate m_currentDate;

    CalendarDBus *m_DBusInter;
    int m_selectedCell = 0;
    bool m_cellSelectable = true;

    QFont m_dayNumFont;

    QColor m_topBorderColor = Qt::red;
    QColor m_backgroundCircleColor = "#2ca7f8";

    QColor m_defaultTextColor = Qt::black;
    QColor m_currentDayTextColor = "#2ca7f8";
    QColor m_weekendsTextColor = Qt::black;
    QColor m_selectedTextColor = Qt::white;
    QColor m_festivalTextColor = Qt::black;
    QColor m_notCurrentTextColor = "#b2b2b2";
    int m_firstWeekDay;
    bool m_huanglistate = true;

    QStringList     m_weeklist;
    QQueue<int> *queue = nullptr;
    QMap<QDate, CaHuangLiDayInfo> *lunarCache = nullptr;
    CaHuangLiDayInfo *emptyCaHuangLiDayInfo = nullptr;
};

#endif // YEARVIEW_H