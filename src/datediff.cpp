#include "datediff.h"

// 天换算年月日
DateDiff dateDiff(const QDate &from, const QDate &to)
{
    DateDiff d;

    int days = qAbs(from.daysTo(to));

    // 天多了减天
    int months = 0;
    while (days >= 30) {
        days -= 30;
        months++;
    }

    // 月多了减月
    int years = 0;
    while (months >= 12) {
        months -= 12;
        years++;
    }

    d.years = years;
    d.months = months;
    d.days = days;
    return d;
}