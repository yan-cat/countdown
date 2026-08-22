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
        Item { //占满不让分配
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}