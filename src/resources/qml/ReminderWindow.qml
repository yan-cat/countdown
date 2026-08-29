import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.dateandtime 1.0 as KA

Item {
    id: root
    width: 360
    height: 80

    signal closeRequested()
    Rectangle {
        anchors.fill: parent
        color: Kirigami.Theme.backgroundColor
        radius: 16
    }

    property string message: "无提醒"
    ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 4

        Text {
            text: message
            color: Kirigami.Theme.textColor
            font.pointSize: 14
            font.bold: true
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            text: "点击关闭"
            color: Kirigami.Theme.textColor
            font.pointSize: 8
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.closeRequested()
    }
}