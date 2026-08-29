#include <QJsonObject>
#include <QSettings>
#include <QString>
#include "countdowndata.h"
#include "datediff.h"
#include "debug.h"

// 按id查
QJsonObject getCountdownJson(QJsonArray m_countdowns, int id, QString key)
{
    for (const QJsonValue &v : m_countdowns) {
        QJsonObject obj = v.toObject();
        if (obj.value("id").toInteger() == id) {
            if (key != "none")
            {
                QJsonObject result;
                result.insert(key, obj.value(key));
                return result;
            }
            else return obj;
        }
    }
    return QJsonObject();
}

QDate getNextDue(const QJsonObject &obj, const QDate &today)
{
    QString date = obj.value("date").toString();
    QDateTime target = QDateTime::fromString(date, "yyyy-MM-dd");
    QDate targetDate = target.date();

    int repeat = obj.value("repeat").toInteger(); // 0=不重复, 1=月重复, 2=年重复

    QDate nextDue;
    if (repeat == 2) {
        // 年重复
        int m = targetDate.month();
        int d = targetDate.day();
        nextDue = QDate(today.year(), m, d);
        if (nextDue < today) {
            nextDue = QDate(today.year() + 1, m, d);
        }
    } else if (repeat == 1) {
        // 月重复
        int d = targetDate.day();
        nextDue = QDate(today.year(), today.month(), d);
        if (nextDue < today) {
            nextDue = QDate(today.addMonths(1).year(),
                            today.addMonths(1).month(), d);
        }
    } else {
        // 不重复或数据异常
        nextDue = targetDate;
    }
    return nextDue;
}

qint64 getnotificationdays(const QJsonObject &obj){return obj.value("notificationdays").toInteger();}

QVariantList buildCountdownViewData(const QJsonArray &rawCountdowns)
{
    QSettings s;
    qCDebug(CountdownLog) << "[ Debug ]" << "查询数据";
    QVariantList list;
    QDate today = QDate::currentDate();

    for (const QJsonValue &v : rawCountdowns) {
        QJsonObject obj = v.toObject();

        // 重复文案
        QString repeat;
        int repeatVal = obj.value("repeat").toInteger();
        if (repeatVal == 0) repeat = "不重复";
        else if (repeatVal == 1) repeat = "月重复";
        else if (repeatVal == 2) repeat = "年重复";

        // 提醒
        qint64 notificationdays = obj.value("notificationdays").toInteger();
        QString notificationdaystext;
        if (notificationdays == -1) notificationdaystext = "无提醒";
        else if (notificationdays == 0) notificationdaystext = "当天提醒";
        else notificationdaystext = QString("提前 %1 天提醒").arg(notificationdays);

        // 计算下一个到期日
        QDate nextDue = getNextDue(obj, today);

        // 计算天数
        qint64 days = today.daysTo(nextDue);
        QString daysText;
        if (s.value("dayshow", 0).toInt() == 0) {
            if (days > 0) {
                daysText = QString("还有 %1 天").arg(days);
            } else if (days < 0) {
                daysText = QString("已经过了 %1 天").arg(-days);
            } else {
                daysText = QStringLiteral("今天");
            }
        } else {
            if (days > 0) {
                daysText = QString("还有 %1").arg(dateDiffText(today, nextDue));
            } else if (days < 0) {
                daysText = QString("已经过了 %1").arg(dateDiffText(today, nextDue));
            } else {
                daysText = QStringLiteral("今天");
            }
        }

        obj.insert("repeat", repeat);
        obj.insert("notificationdaystext", notificationdaystext);
        obj.insert("daysText", daysText);

        list.append(obj.toVariantMap());
    }
    return list;
}