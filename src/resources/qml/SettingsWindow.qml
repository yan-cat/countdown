import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.kde.kirigami as Kirigami

Window {
    id: settingsWindow
    title: qsTr("设置")
    width: 640
    height: 480
    visible: false
    color: Kirigami.Theme.backgroundColor

    ColumnLayout {
        anchors.fill: parent
        Label {
            text: qsTr("日期显示格式：")
            Layout.alignment: Qt.AlignHCenter
        }
        ComboBox {
            id: dayshow
            currentIndex: manager.setting("dayshow", 0)
            onActivated: manager.setSetting("dayshow", currentIndex)
            model: [qsTr("456 天"), qsTr("1 年 3 个月 1 天")]
            Layout.alignment: Qt.AlignHCenter
        }
        Label {
            text: qsTr("注：年月日显示模式为估算仅作参考，具体以单天数显示为准")
            Layout.alignment: Qt.AlignHCenter
        }

        CheckBox {
            id: debugmode
            text: qsTr("调试模式")
            checked: manager.setting("debugmode", false)
            onCheckedChanged: {
                manager.setSetting("debugmode", checked)
            }
            Layout.alignment: Qt.AlignHCenter
        }
        Label {
            text: qsTr("调试选项需要重启才能生效！")
            visible: debugmode.checked
            Layout.alignment: Qt.AlignHCenter
        }

        ComboBox {
            id: outputDebuglog
            visible: debugmode.checked
            currentIndex: manager.setting("outputDebuglog", 0)
            model: [qsTr("关闭调试日志"), qsTr("仅APP调试日志"), qsTr("调试日志全开（此设置会拖慢软件速度！！！）")]
            onActivated: manager.setSetting("outputDebuglog", currentIndex)
            Layout.alignment: Qt.AlignHCenter
        }

        CheckBox {
            id: useWindowsReminderType
            text: qsTr("强制使用windows通知窗口而非当前系统默认")
            visible: debugmode.checked
            checked: manager.setting("useWindowsReminderType", false)
            onCheckedChanged: {
                manager.setSetting("useWindowsReminderType", checked)
            }
            Layout.alignment: Qt.AlignHCenter
        }

        CheckBox {
            id: useEnLang
            text: qsTr("强制软件语言为英语")
            visible: debugmode.checked
            checked: manager.setting("useEnLang", false)
            onCheckedChanged: {
                manager.setSetting("useEnLang", checked)
            }
            Layout.alignment: Qt.AlignHCenter
        }

        // 占满不让分配
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}