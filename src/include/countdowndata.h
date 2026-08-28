#ifndef COUNTDOWNDATA_H
#define COUNTDOWNDATA_H

#include <QJsonArray>
#include <qjsonobject.h>

QJsonObject getCountdownJson(QJsonArray m_countdowns, int id, QString key);
QDate getNextDue(const QJsonObject &obj, const QDate &today);
qint64 getnotificationdays(const QJsonObject &obj);
// 把存储的原始数组加工成 QML 要的显示列表
QVariantList buildCountdownViewData(const QJsonArray &rawCountdowns);

#endif // COUNTDOWNDATA_H
