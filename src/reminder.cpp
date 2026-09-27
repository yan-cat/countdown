#include <QVariant>
#include <QQuickView>
#include <QUrl>
#include <QQuickItem>
#include "reminder.hpp"
#include "debug.hpp"
#include "main.hpp"

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#endif

CountdownReminder::CountdownReminder(QObject *parent) : QObject(parent) { }

CountdownReminder &reminder()
{
    static CountdownReminder instance(nullptr);
    return instance;
}
CountdownReminder *CountdownReminder::create(QQmlEngine *, QJSEngine *)
{
    CountdownReminder *m = &reminder();                          // 复用同一个实例
    QJSEngine::setObjectOwnership(m, QJSEngine::CppOwnership);  // 别让引擎删它
    return m;
}

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
void linux_reminder(QString title, QString body) {
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
    args << title; // 通知标题/摘要 (QString)
    args << body; // 通知正文 (QString)
    args << QStringList(); // 操作列表 (QStringList)
    args << QVariantMap(); // 额外提示 (QVariantMap)
    args << 0; // 超时时间，单位毫秒 (int)

    // 4. 异步调用，避免阻塞UI线程[reference:4]
    QDBusPendingCall call = iface.asyncCallWithArgumentList("Notify", args);

    // (可选) 等待并处理回复
    QDBusPendingReply<uint> reply = call;
    reply.waitForFinished();
    if (reply.isError()) qCritical() << "发送通知失败:" << reply.error().message();
    else qCDebug(CountdownLog) << "已成功发送通知";

    qCDebug(CountdownLog) << "通知id:" << reply.value();
    qCDebug(CountdownLog) << "通知内容:" << body;
}
#endif

void windows_reminder(QString body) {
    qCDebug(CountdownLog) << "准备显示弹窗";

    QQuickView *view = new QQuickView;

    view->setFlags(Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint |
                   Qt::Tool);
    view->setResizeMode(QQuickView::SizeViewToRootObject);
    view->setColor(Qt::transparent);
    view->setSource(QUrl("qrc:/qt/qml/com/countdown/src/resources/qml/ReminderWindow.qml"));
    if (view->status() != QQuickView::Ready) {
        qCritical() << "加载弹窗失败:" << view->errors();
        delete view;
        return;
    }
    QObject *root = view->rootObject();
    if (root) {
        root->setProperty("message", body);
        QObject::connect(root, SIGNAL(closeRequested()),
                         view, SLOT(close()));

        int w = root->property("width").toInt();
        int h = root->property("height").toInt();
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect screenRect = screen->geometry();
            int margin = 30;
            view->setPosition(screenRect.right() - w - margin, screenRect.bottom() - h - margin);
        }
    }

    QObject::connect(view, &QQuickWindow::closing, view, &QObject::deleteLater);
    view->show();
}

void CountdownReminder::pushReminder(QString title, QString body) {
    qCDebug(CountdownLog) << "当前系统为：" << os;

    QString os1 = os; // 总共就这几行有用，也懒得想名字了

    if (debug().getDebugOn("useWindowsReminderType")) {
        os1 = "win";
        qCDebug(CountdownLog) << "强制win通知模式为开启";
    }

    if (os1 == "linux") {
        qCDebug(CountdownLog) << "通知发送模式：通知";
        #if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
        linux_reminder(title, body);
        #endif
    }
    else if (os1 == "win") {
        qCDebug(CountdownLog) << "通知发送模式：弹窗";
        windows_reminder(body);
    }
    else qCDebug(CountdownLog) << "通知发送模式：无";
}
