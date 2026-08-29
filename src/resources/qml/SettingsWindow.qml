import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.kde.kirigami as Kirigami

Window {
    id: settingsWindow
    title: "设置"
    width: 420
    height: 360
    visible: false
    color: Kirigami.Theme.backgroundColor

    ColumnLayout {
        anchors.fill: parent
        Label {
            text: "日期显示格式："
            Layout.alignment: Qt.AlignHCenter
        }
        ComboBox {
            id: dayshow
            currentIndex: manager.setting("dayshow", 0)
            onActivated: manager.setSetting("dayshow", currentIndex)
            model: ["456 天", "1 年 3 个月 1 天"]
            Layout.alignment: Qt.AlignHCenter
        }
        Label {
            text: "注：年月日显示模式为估算仅作参考，具体以单天数显示为准"
            Layout.alignment: Qt.AlignHCenter
        }

        CheckBox {
            id: debugmode
            text: "调试模式"
            checked: manager.setting("debugmode", false)
            onCheckedChanged: {
                manager.setSetting("debugmode", checked)
            }
            Layout.alignment: Qt.AlignHCenter
        }
        Label {
            text: "调试选项需要重启才能生效！"
            visible: debugmode.checked
            Layout.alignment: Qt.AlignHCenter
        }
        CheckBox {
            id: outputDebuglog
            text: "输出APP调试日志"
            visible: debugmode.checked
            checked: manager.setting("outputDebuglog", false)
            onCheckedChanged: {
                manager.setSetting("outputDebuglog", checked)
            }
            Layout.alignment: Qt.AlignHCenter
        }
        CheckBox {
            id: outputAllDebuglog
            text: "输出全部调试日志"
            visible: debugmode.checked
            checked: manager.setting("outputAllDebuglog", false)
            onCheckedChanged: {
                manager.setSetting("outputAllDebuglog", checked)
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