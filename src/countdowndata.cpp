#include "countdowndata.h"
#include "datediff.h"
#include <QJsonObject>
#include <QSettings>

QVariantList buildCountdownViewData(const QJsonArray &rawCountdowns)
{
    QSettings s;
    qDebug() << "查询数据";
    QVariantList list;
    for (const QJsonValue &v : rawCountdowns) {
        QJsonObject obj = v.toObject(); //拿出来

        QString repeat; //重复文案
        if (obj.value("repeat") == 0) repeat = "不重复";
        if (obj.value("repeat") == 1) repeat = "月重复";
        if (obj.value("repeat") == 2) repeat = "年重复";

        QString date = obj.value("date").toString(); //日期

        QDateTime target = QDateTime::fromString(date, "yyyy-MM-dd"); //天数
        QDate today = QDate::currentDate();
        QDate nextDue;
        if (obj.value("repeat") == 2) { //重复吗
            int m = target.date().month();
            int d = target.date().day();
            nextDue = QDate(today.year(), m, d);
            if (nextDue < today) {
                nextDue = QDate(today.year() + 1, m, d);
            }
        } else if (obj.value("repeat") == 1) {
            int d = target.date().day();
            nextDue = QDate(today.year(), today.month(), d);
            if (nextDue < today) {
                nextDue = QDate(today.addMonths(1).year(),
                                today.addMonths(1).month(), d);
            }
        } else { //不重复或数据异常
            nextDue = target.date();
        }
        qint64 days = today.daysTo(nextDue);
        QString daysText; //文案
        if (s.value("dayshow", 0).toInt() == 0) { //xxx天
            if (days > 0) {
                daysText = QString("还有 %1 天").arg(days);
            } else if (days < 0) {
                daysText = QString("已经过了 %1 天").arg(-days);
            } else {
                daysText = QStringLiteral("今天");
            }
        } else { //xx年xx月xx日
            if (days > 0) {
                daysText = QString("还有 %1").arg(dateDiffText(today, nextDue));
            } else if (days < 0) {
                daysText = QString("已经过了 %1").arg(dateDiffText(today, nextDue));
            } else {
                daysText = QStringLiteral("今天");
            }
        }


        obj.insert("repeat", repeat); //修改
        obj.insert("daysText", daysText);

        list.append(obj.toVariantMap()); //丢回去
    }
    return list;
}
