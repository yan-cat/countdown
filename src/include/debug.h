#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(CountdownLog)

#ifndef DEBUG_H
#define DEBUG_H

#include <QString>

qint64 getDebugOn(const QString &key);

#endif