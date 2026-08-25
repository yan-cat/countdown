#include "datediff.h"

DateDiff dateDiff(const QDate &from, const QDate &to) //天换算年月日
{
    DateDiff d;

    int days = qAbs(from.daysTo(to));

    int months = 0;
    while (days >= 30) { //天多了减天
        days -= 30;
        months++;
    }

    int years = 0;
    while (months >= 12) { //月多了减月
        months -= 12;
        years++;
    }

    d.years = years;
    d.months = months;
    d.days = days;
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