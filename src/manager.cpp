#include "manager.h"
#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

CountdownManager::CountdownManager(QObject *parent) : QObject(parent)
{
    // 初始化
}

void CountdownManager::addCountdown(const QString &dateString)
{
    // 用 QJsonDocument 把字符串解析成 JSON 对象
    QJsonObject obj = QJsonDocument::fromJson(dateString.toUtf8()).object();

    QString name = obj.value("name").toString();        // 取字段
    QString repeat = obj.value("repeat").toString();
    QString date = obj.value("date").toString();

    QDateTime target = QDateTime::fromString(date, "yyyy-MM-dd");
    if (!target.isValid()) {
        qWarning() << "无效日期：" << date;
        return;
    }

    qint64 days = QDateTime::currentDateTime().daysTo(target);
    qDebug() << "添加倒数日：" << name << repeat << date << "剩余" << days << "天";
}