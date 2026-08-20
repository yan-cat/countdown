#include "manager.h"
#include <QDebug>
#include <QDateTime>

CountdownManager::CountdownManager(QObject *parent) : QObject(parent) {}

void CountdownManager::addCountdown(const QString &dateString)
{
    QDateTime target = QDateTime::fromString(dateString, "yyyy-MM-dd");
    if (!target.isValid()) {
        qWarning() << "无效日期：" << dateString;
        return;
    }

    qint64 days = QDateTime::currentDateTime().daysTo(target);
    qDebug() << "添加倒数日，目标日期：" << dateString << "，剩余天数：" << days;

    // 这里可以写入数据库或存储到列表，然后发射信号更新 QML
}
