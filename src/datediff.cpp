#include "datediff.h"

DateDiff dateDiff(const QDate &from, const QDate &to) //天转年月日
{
    DateDiff d;
    d.years  = to.year()  - from.year();
    d.months = to.month() - from.month();
    d.days   = to.day()   - from.day();

    if (d.days < 0) { //天数不够，向月份借
        d.months--;
        d.days += to.addMonths(-1).daysInMonth();
    }
    if (d.months < 0) { //月份不够，向年份借
        d.years--;
        d.months += 12;
    }
    return d;
}
QString dateDiffText(const QDate &from, const QDate &to) //年月日文案
{
    DateDiff d = dateDiff(from, to);

    d.years = qAbs(d.years);
    d.months = qAbs(d.months);
    d.days = qAbs(d.days);

    QString text;
    if (d.years != 0) text += QString("%1 年 ").arg(d.years);
    if (d.months != 0) text += QString("%1 个月 ").arg(d.months);
    if (d.days != 0) text += QString("%1 天").arg(d.days);
    return text;
}