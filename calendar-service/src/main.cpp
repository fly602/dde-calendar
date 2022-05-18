// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "dservicemanager.h"
#include "ddatabasemanagement.h"

#include <DLog>

#include <QDBusConnection>
#include <QDBusError>
#include <QTranslator>
#include <QCoreApplication>

const static QString CalendarServiceTranslationsDir = "/usr/share/dde-calendar/translations";

bool loadTranslator(QCoreApplication *app, QList<QLocale> localeFallback = QList<QLocale>() << QLocale::system())
{
    bool bsuccess = false;
    for (auto &locale : localeFallback) {
        QString translateFilename = QString("%1_%2").arg(app->applicationName()).arg(locale.name());
        QString translatePath = QString("%1/%2.qm").arg(CalendarServiceTranslationsDir).arg(translateFilename);
        if (QFile(translatePath).exists()) {
            QTranslator *translator = new QTranslator(app);
            translator->load(translatePath);
            app->installTranslator(translator);
            bsuccess = true;
        }
        QStringList parseLocalNameList = locale.name().split("_", QString::SkipEmptyParts);
        if (parseLocalNameList.length() > 0 && !bsuccess) {
            translateFilename = QString("%1_%2").arg(app->applicationName()).arg(parseLocalNameList.at(0));
            QString parseTranslatePath = QString("%1/%2.qm").arg(CalendarServiceTranslationsDir).arg(translateFilename);
            if (QFile::exists(parseTranslatePath)) {
                QTranslator *translator = new QTranslator(app);
                translator->load(parseTranslatePath);
                app->installTranslator(translator);
                bsuccess = true;
            }
        }
    }
    return bsuccess;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName("deepin");
    a.setApplicationName("dde-calendar-service");

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    qInfo() << "test";
    //加载翻译
    if (!loadTranslator(&a)) {
        qDebug() << "loadtranslator failed";
    }
    //TODO: 数据库迁移
    DDataBaseManagement dbManagement;

    DServiceManager serviceManager;
    return a.exec();
}
