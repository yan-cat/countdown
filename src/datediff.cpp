#include "datediff.hpp"

// 天换算年月日估算
DateDiff approxDateDiff(const QDate &from, const QDate &to) {
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

// 天转年月日准确
DateDiff exactDateDiff(const QDate &from, const QDate &to) {
    DateDiff d;

    QDate start = from;
    QDate end = to;
    if (start > end) {
        qSwap(start, end);
    }

    // 1. 年：能加几年就加几年
    int years = 0;
    QDate cursor = start;
    while (true) {
        QDate next = cursor.addYears(1);
        if (!next.isValid() || next > end) break;
        cursor = next;
        years++;
    }

    // 2. 月
    int months = 0;
    while (true) {
        QDate next = cursor.addMonths(1);
        if (!next.isValid() || next > end) break;
        cursor = next;
        months++;
    }

    // 3. 剩下的天数
    int days = cursor.daysTo(end);

    d.years  = years;
    d.months = months;
    d.days   = days;
    return d;
}

