import com.countdown
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.dateandtime as DateAndTime

Kirigami.ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true
    title: qsTr("倒数日")

    // 懒加载子窗口
    Loader {
        id: settingsLoader
        active: false
        sourceComponent: Component { SettingsWindow { } }
    }
    Loader {
        id: updaterLoader
        active: false
        sourceComponent: Component { UpdaterWindow { } }
    }
    Loader {
        id: aboutPageLoader
        active: false
        sourceComponent: Component { AboutPageWindow { } }
    }
    Loader {
        id: detailsLoader
        active: false
        sourceComponent: Component {
            DetailsWindow {
                onVisibleChanged: { // 关掉就销毁，不然几下就eglSwapBuffers failed with 0x300d，偷个懒嘿嘿
                    if (!visible) {
                        detailsLoader.active = false
                    }
                }
            }
        }
    }
    Loader {
        id: guideLoader
        active: false
        sourceComponent: Component { GuideWindow { } }
    }
    Connections {
        target: CountdownTray
        function onMainWindowVisibilityChanged(visible) {
            if (visible) {
            }
            else {
                settingsLoader.active = false
                updaterLoader.active = false
                aboutPageLoader.active = false
                detailsLoader.active = false
                guideLoader.active = false
            }
        }
    }

    Component.onCompleted: {
        if (!CountdownManager.setting("guideOpened", 0))
        {
            if (!guideLoader.active) {
                guideLoader.active = true
            }
            guideLoader.item.show()
            CountdownManager.setSetting("guideOpened", 1)
        }
    }

    // 自动开更新页面
    Connections {
        target: CountdownUpdater

        function onNewVersion(latestVersion, version , updateLog) { // latestVersion 为 true 是软件版本小于 latest
            if (latestVersion && CountdownManager.setting("autoGetNewVersion", false)) {
                if (!updaterLoader.active) {
                    updaterLoader.active = true
                }
                updaterLoader.item.show()
            }
        }
    }

    // 安卓加载转圈圈
    Rectangle {
        id: splashOverlay
        anchors.fill: parent
        color: Kirigami.Theme.backgroundColor
        z: 99999
        visible: Qt.platform.os === "android"
        Column {
            anchors.centerIn: parent
            spacing: Kirigami.Units.largeSpacing
            Image {
                source: "qrc:/qt/qml/com/countdown/src/resources/icon/com.countdown.svg"
                width: 100
                height: 100
                anchors.horizontalCenter: parent.horizontalCenter
            }
            BusyIndicator {
                anchors.horizontalCenter: parent.horizontalCenter
                running: true
                width: 60
                height: 60
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("加载中...")
                color: Kirigami.Theme.textColor
            }
        }
        Component.onCompleted: {
            splashTimer.start()
        }
        Timer {
            id: splashTimer
            interval: 300
            onTriggered: fadeOutAnim.start()
        }
        NumberAnimation {
            id: fadeOutAnim
            target: splashOverlay
            property: "opacity"
            to: 0
            duration: 300
            onFinished: splashOverlay.visible = false
        }
    }

    // 左上角菜单
    globalDrawer: Kirigami.GlobalDrawer {
        title: qsTr("菜单")
        isMenu: true
        actions: [
            Kirigami.Action {
                text: qsTr("设置")
                icon.name: "settings-configure"
                onTriggered: {
                    if (!settingsLoader.active) {
                        settingsLoader.active = true
                    }
                    settingsLoader.item.show()
                }
            },
            Kirigami.Action {
                text: qsTr("检查更新")
                icon.name: "update-none"
                onTriggered: {
                    if (!updaterLoader.active) {
                        updaterLoader.active = true
                    }
                    updaterLoader.item.show()
                }
            },
            Kirigami.Action {
                text: qsTr("关于")
                icon.name: "help-about-symbolic"
                onTriggered: {
                    if (!aboutPageLoader.active) {
                        aboutPageLoader.active = true
                    }
                    aboutPageLoader.item.show()
                }
            }
        ]
    }

    // 倒数日页
    pageStack.initialPage: CountdownsPage { }

    // 报错
    Connections {
        target: CountdownDebug
        function onCriticalOccurred(message) {
            errorDialog.errorTitle = qsTr("错误！继续运行可能会导致数据丢失等问题，是否退出软件")
            errorDialog.errorMessage = qsTr("日志：%1").arg(message)
            errorDialog.exitApp = true
            errorDialog.open()
        }

        function onShowErrorDialog(message) {
            errorDialog.errorTitle = ""
            errorDialog.errorMessage = message
            errorDialog.exitApp = false
            errorDialog.open()
        }
    }

    MessageDialog {
        id: errorDialog
        title: errorTitle
        buttons: exitApp ? (MessageDialog.Ok | MessageDialog.Cancel) : MessageDialog.Ok

        text: errorMessage

        onAccepted: {
            if (exitApp) Qt.quit()
        }

        property string errorTitle: ""
        property string errorMessage: ""
        property bool exitApp: false
    }
}
