#ifndef COUNTDOWNDATA_H
#define COUNTDOWNDATA_H

#include <QJsonArray>

// 把存储的原始数组加工成 QML 要的显示列表
QVariantList buildCountdownViewData(const QJsonArray &rawCountdowns);

#endif // COUNTDOWNDATA_H
