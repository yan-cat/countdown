#pragma once
#include <QDate>

struct DateDiff {
    int years = 0;
    int months = 0;
    int days = 0;
};

DateDiff exactDateDiff(const QDate &from, const QDate &to);
