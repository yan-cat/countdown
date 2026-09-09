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

            // Debug
            Text {
                text: "\nDebug信息"
                font.pointSize: Kirigami.Theme.defaultFont.pointSize
                color: Kirigami.Theme.textColor
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                visible: manager.setting("showDebugDetails", false)

                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
            }
            Repeater {
                model: Object.keys(detailsWindow.modelData)
                delegate: RowLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    spacing: 5
                    visible: manager.setting("showDebugDetails", false)

                    Label {
                        text: modelData + "："
                        font.bold: true
                        color: Kirigami.Theme.textColor
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        Layout.preferredWidth: detailsWindow.width * 0.5 - 20
                        Layout.maximumWidth: detailsWindow.width * 0.5 - 20
                    }
                    Label {
                        text: {
                            var val = detailsWindow.modelData[modelData]
                            if (val === undefined || val === null) return qsTr("无数据")
                            if (typeof val === 'object') return JSON.stringify(val)
                            return String(val)
                        }
                        color: Kirigami.Theme.textColor
                        Layout.fillWidth: true
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }
                }
            }
        }
    }
}