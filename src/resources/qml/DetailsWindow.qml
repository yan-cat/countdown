import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.kde.kirigami as Kirigami

Window {
    id: detailsWindow
    title: qsTr("详情")
    width: 240
    height: 480
    visible: false
    color: Kirigami.Theme.backgroundColor

    property var modelData: ({})

    Text {
        id: titleText
        text: modelData.name || qsTr("无数据")
        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
        color: Kirigami.Theme.textColor
        font.weight: Font.Bold
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 10
    }

    ScrollView {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: titleText.bottom
        anchors.bottom: parent.bottom
        anchors.topMargin: 10
        contentWidth: availableWidth

        ScrollBar.vertical.policy: ScrollBar.AlwaysOn

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            Text {
                text: "[ Debug ] id：" + (modelData.id ?? qsTr("无数据"))
                font.pointSize: Kirigami.Theme.defaultFont.pointSize
                color: Kirigami.Theme.textColor
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                visible: manager.setting("showId", false)

                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
            }
            Text {
                text: qsTr("日期：") + (modelData.date || qsTr("无数据"))
                font.pointSize: Kirigami.Theme.defaultFont.pointSize
                color: Kirigami.Theme.textColor
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
            }
            Text {
                text: qsTr("天数：") + (modelData.daysText || qsTr("无数据"))
                font.pointSize: Kirigami.Theme.defaultFont.pointSize
                color: Kirigami.Theme.textColor
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
            }
            Text {
                text: qsTr("重复：") + (modelData.repeatText || qsTr("无数据"))
                font.pointSize: Kirigami.Theme.defaultFont.pointSize
                color: Kirigami.Theme.textColor
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
            }
            Text {
                text: qsTr("提醒时间：") + (modelData.notificationdaysText || qsTr("无数据"))
                font.pointSize: Kirigami.Theme.defaultFont.pointSize
                color: Kirigami.Theme.textColor
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
            }
        }
    }
}