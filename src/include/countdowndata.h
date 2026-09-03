#ifndef COUNTDOWNDATA_H
#define COUNTDOWNDATA_H

#include <QJsonArray>
#include <QJsonObject>

class CountdownData : public QObject {
    Q_OBJECT
public:
    static QJsonObject getCountdownJson(QJsonArray m_countdowns, int id, QString key);
    static QDate getNextDue(const QJsonObject &obj, const QDate &today);
    static qint64 getnotificationdays(const QJsonObject &obj);
    static QVariantList buildCountdownViewData(const QJsonArray &rawCountdowns);
};

#endif // COUNTDOWNDATA_H
