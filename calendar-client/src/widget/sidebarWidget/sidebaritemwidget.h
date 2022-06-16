/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leilong  <leilong@uniontech.com>
*
* Maintainer: leilong  <leilong@uniontech.com>
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
#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include "scheduledatamanage.h"
#include "dscheduletype.h"
#include "accountitem.h"

#include <DIconButton>
#include <DLabel>
#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QTreeWidgetItem>

DWIDGET_USE_NAMESPACE

class SidebarItemWidget : public QWidget
{
    Q_OBJECT
public:

    SidebarItemWidget(QWidget *parent = nullptr);

    //获取顶层item控件，有箭头
    static SidebarItemWidget *getAccountItemWidget(AccountItem::Ptr);
    //获取子层本地日程item控件，有复选框
    static SidebarItemWidget *getTypeItemWidget(DScheduleType::Ptr);

    //设置选中状态
    void setSelectStatus(bool);
    //获取当前状态
    bool getSelectStatus();
    //切换状态
    void switchState();
    //设置item
    void setItem(QTreeWidgetItem *);

    QTreeWidgetItem* getTreeItem();

signals:
    //状态改变信号
    void signalStatusChange(bool status, QString id = "");

public slots:


protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void updateStatus() = 0;

protected:
    QString m_title = "";   //标题
    QString m_id = ""; //数据id
    bool m_selectStatus = false;    //选中状态
    QTreeWidgetItem *m_item = nullptr; //关联的item
};

class SidebarTypeItemWidget : public SidebarItemWidget
{
    Q_OBJECT
public:
    SidebarTypeItemWidget(DScheduleType::Ptr, QWidget *parent = nullptr);

protected:
    void initView();
    void updateStatus() override;

private:
    DScheduleType::Ptr m_scheduleType;

    DIconButton *m_rearIconButton = nullptr;    //尾部icon控件
    QCheckBox *m_checkBox = nullptr;    //复选框
    DLabel *m_titleLabel = nullptr; //标题显示区域
};

class SidebarAccountItemWidget : public SidebarItemWidget
{
    Q_OBJECT
public:
    SidebarAccountItemWidget(AccountItem::Ptr, QWidget *parent = nullptr);

    AccountItem::Ptr getAccountItem();

signals:


public slots:
    //尾部图标控件点击事件
    void slotRearIconClicked();
    //同步状态改变事件
    void slotSyncStatusChange(DAccount::AccountSyncState);
    //帐户同步状态发生改变
    void slotAccountStateChange();

protected:
    void initView();
    void initConnect();
    void updateStatus() override;
    void resetRearIconButton();

private:
    AccountItem::Ptr m_accountItem;

    DIconButton *m_syncIconButton = nullptr;    //尾部同步控件
    DLabel *m_warningLabel = nullptr;           //尾部异常警告控件
    DIconButton *m_headIconButton = nullptr;    //头部展开控件
    DLabel *m_titleLabel = nullptr; //标题显示区域
};

#endif // ITEMWIDGET_H
