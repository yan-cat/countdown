#include <QSystemTrayIcon>
#include <QMenu>
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>
#include <QApplication>
#include <QWindow>
#include <QGuiApplication>
#include "tray.hpp"
#include "debug.hpp"

CountdownTray::CountdownTray(QObject *parent) : QObject(parent) { }

CountdownTray &tray()
{
    static CountdownTray instance(nullptr);
    return instance;
}
CountdownTray *CountdownTray::create(QQmlEngine *, QJSEngine *)
{
    CountdownTray *m = &tray();                          // 复用同一个实例
    QJSEngine::setObjectOwnership(m, QJSEngine::CppOwnership);  // 别让引擎删它
    return m;
}

// 初始化托盘
void CountdownTray::trayInit() {
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setToolTip(QStringLiteral("倒数日"));
    m_trayMenu = new QMenu();

    // 选项
    m_trayMenu->addAction(QStringLiteral("显示主窗口"), qApp, [this]() {
        showMainWindow();
    });
    m_trayMenu->addAction(QStringLiteral("退出"), qApp, &QCoreApplication::quit);

    m_trayIcon->setContextMenu(m_trayMenu);
    updateTrayIcon();
    m_trayIcon->show();

    // 托盘自动深浅
    qApp->installEventFilter(this);

    // 左键打开主窗口
    QObject::connect(m_trayIcon, &QSystemTrayIcon::activated, this,
                     [this](QSystemTrayIcon::ActivationReason reason) {
                         switch (reason) {
                         case QSystemTrayIcon::Trigger: // 左键
                             showMainWindow();
                             break;
                         case QSystemTrayIcon::Context: // 右键
                             break;
                         case QSystemTrayIcon::DoubleClick: // 双击（Windows）
                             showMainWindow();
                             break;
                         default:
                             break;
                         }
                     });

}

// 托盘自动深浅
bool CountdownTray::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::ApplicationPaletteChange) {
        updateTrayIcon();
    }
    return QObject::eventFilter(obj, event);
}

// 托盘颜色
void CountdownTray::updateTrayIcon() {
    if (!m_trayIcon) return;

    QSvgRenderer renderer(QStringLiteral(
        ":/qt/qml/com/countdown/src/resources/icon/Countdown_tray.svg"));
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    renderer.render(&painter);
    painter.end();

    // 用应用前景色给图标染色
    QColor iconColor = qApp->palette().color(QPalette::WindowText);
    QPainter tintPainter(&pixmap);
    tintPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tintPainter.fillRect(pixmap.rect(), iconColor);
    tintPainter.end();

    m_trayIcon->setIcon(QIcon(pixmap));
}

// 销毁
void CountdownTray::shutdown() {
    if (m_trayIcon) {
        m_trayIcon->hide();
        delete m_trayIcon;
        m_trayIcon = nullptr;
    }
    if (m_trayMenu) {
        delete m_trayMenu;
        m_trayMenu = nullptr;
    }
}

void CountdownTray::showMainWindow() {
    const QWindowList windows = QGuiApplication::topLevelWindows();
    bool s = false; // 成功了吗
    for (QWindow *w : windows) {
        if (w->type() != Qt::Window) continue;
        w->show();
        w->raise();
        w->requestActivate();
        s = true;
        qCDebug(CountdownLog) << "主窗口开启";
        break;
    }
    if(!s) qCritical() << "主窗口开启失败：未找到主窗口，请重启程序";
}
