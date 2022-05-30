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
#ifndef DDATABASE_H
#define DDATABASE_H

#include <QObject>
#include <QSqlDatabase>

class DDataBase : public QObject
{
    Q_OBJECT
public:
    explicit DDataBase(QObject *parent = nullptr);
    virtual ~DDataBase();

    QString getDBPath() const;
    void setDBPath(const QString &DBPath);
    static QString createUuid();

    QString getConnectionName() const;
    void setConnectionName(const QString &connectionName);

    //初始化数据库数据，会创建数据库文件和相关数据表
    virtual void initDBData();
    void dbOpen();

    //判断数据库是否存在
    bool dbFileExists();

protected:
    //创建数据库
    virtual void createDB() = 0;

protected:
    QSqlDatabase m_database;
    QString m_DBPath;
    QString m_connectionName;
};

#endif // DDATABASE_H