#include <QJsonObject>
#include <QString>
#include "countdowndata.h"
#include "datediff.h"
#include "debug.h"
#include "manager.h"

// 按id查
QJsonObject CountdownData::getCountdownJson(QJsonArray m_countdowns, int id, QString key) {
    for (const QJsonValue &v : m_countdowns) {
        QJsonObject obj = v.toObject();
        if (obj.value("id").toInteger() == id) {
            if (key != "none") {
                QJsonObject result;
                result.insert(key, obj.value(key));
                return result;
            }
            else return obj;
        }
    }
    return QJsonObject();
}

// 下一次的日期
QDate CountdownData::getNextDue(const QJsonObject &obj, const QDate &today) {
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

qint64 CountdownData::getnotificationdays(const QJsonObject &obj){return obj.value("notificationdays").toInteger();}

// 构建显示的数据
QVariantList CountdownData::buildCountdownViewData(const QJsonArray &rawCountdowns) {
    qCDebug(CountdownLog) << "[ Debug ]" << "查询数据";
    QVariantList list;
    QDate today = QDate::currentDate();

    for (const QJsonValue &v : rawCountdowns) {
        QJsonObject obj = v.toObject();

        // 重复文案
        QString repeatText;
        int repeat = obj.value("repeat").toInteger();
        if (repeat == 0) repeatText = tr("不重复");
        else if (repeat == 1) repeatText = tr("月重复");
        else if (repeat == 2) repeatText = tr("年重复");
        obj.insert("repeatText", repeatText);

        // 提醒
        qint64 notificationdays = obj.value("notificationdays").toInteger();
        QString notificationdaysText;
        if (notificationdays == -1) notificationdaysText = tr("无提醒");
        else if (notificationdays == 0) notificationdaysText = tr("当天提醒");
        else notificationdaysText = tr("提前 %1 天提醒").arg(notificationdays);
        obj.insert("notificationdaysText", notificationdaysText);

        // 计算下一个到期日
        QDate nextDue = getNextDue(obj, today);

        // 计算天数
        qint64 days = today.daysTo(nextDue);
        obj.insert("days", days);

        // 天数文本
        QString daysText;
        if (manager().setting("dayshow", 0) == 0) {
            if (days > 0) {
                daysText = tr("还有 %1 天").arg(days);
            } else if (days < 0) {
                daysText = tr("已经过了 %1 天").arg(-days);
            } else {
                daysText = tr("今天");
            }
        } else {
            DateDiff ymd = dateDiff(today, nextDue);
            QString ymdtext;
            if (ymd.years != 0) ymdtext += tr("%1 年 ").arg(ymd.years);
            if (ymd.months != 0) ymdtext += tr("%1 个月 ").arg(ymd.months);
            if (ymd.days != 0) ymdtext += tr("%1 天").arg(ymd.days);

            if (days > 0) {
                daysText = tr("还有 %1").arg(ymdtext);
            } else if (days < 0) {
                daysText = tr("已经过了 %1").arg(ymdtext);
            } else {
                daysText = tr("今天");
            }
        }
        obj.insert("daysText", daysText);

        // 临近吗
        qint64 upcomingdays = 3;
        bool upcoming = days >= 0 && days <= upcomingdays && manager().setting("upcoming", 0);
        obj.insert("upcoming", upcoming);

        // qCDebug(CountdownLog) << "[ Debug ]" << "卡片数据：" << obj;
        list.append(obj.toVariantMap());
    }
    return list;
}