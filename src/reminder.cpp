#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QVariant>
#include "reminder.h"
#include "debug.h"

void reminder(QString body)
{
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications");

    // 2. 检查接口是否有效
    if (!iface.isValid()) {
        qWarning() << "D-Bus 通知接口无效";
        return;
    }

    // 3. 准备 Notify 方法的参数
    QList<QVariant> args;
    args << "Countdown"; // 应用程序名称 (QString)
    args << 0u; // 通知ID，0表示让服务自动分配 (uint)
    args << ""; // 图标名称或路径 (QString)
    args << "倒数日提醒"; // 通知标题/摘要 (QString)
    args << body; // 通知正文 (QString)
    args << QStringList(); // 操作列表 (QStringList)
    args << QVariantMap(); // 额外提示 (QVariantMap)
    args << 0; // 超时时间，单位毫秒 (int)

    // 4. 异步调用，避免阻塞UI线程[reference:4]
    QDBusPendingCall call = iface.asyncCallWithArgumentList("Notify", args);

    // (可选) 等待并处理回复
    QDBusPendingReply<uint> reply = call;
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << "发送通知失败:" << reply.error().message();
    } else {
        qCDebug(CountdownLog) << "[ Debug ]" << "已成功发送通知";
        qCDebug(CountdownLog) << "[ Debug ]" << "通知id:" << reply.value();
        qCDebug(CountdownLog) << "[ Debug ]" << "通知内容:" << body;
    }
}