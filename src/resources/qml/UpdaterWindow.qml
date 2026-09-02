import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.kde.kirigami as Kirigami

Window {
    id: updaterWindow
    title: "检查更新"
    width: 640
    height: 480
    visible: false
    color: Kirigami.Theme.backgroundColor

    Connections {
        target: updater
        function onNewVersion(latestVersion, version , updateLog)
        {
            if ( latestVersion ) versionShow.text = version
            else versionShow.text = "当前已是最新版本：" + version
            versionShow.visible = true

            versionUpdateLog.text = updateLog
            versionUpdateLogScroll.visible = true

            downloadUpdate.visible = latestVersion

            loadingOverlay.visible = false // 转圈圈
        }
    }

    Text {
        id: versionShow
        visible: false
        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 2
        color: Kirigami.Theme.textColor
        font.weight: Font.Bold

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 10
        anchors.topMargin: 10
    }

    ScrollView {
            id: versionUpdateLogScroll
            visible: false

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: versionShow.bottom
            anchors.bottom: buttonRow.top

            anchors.leftMargin: 10
            anchors.rightMargin: 10
            anchors.topMargin: 10
            anchors.bottomMargin: 10
            TextArea {
                id: versionUpdateLog
                text: "未知更新日志"
                wrapMode: TextArea.Wrap
                textFormat: TextArea.MarkdownText
                font.pointSize: Kirigami.Theme.defaultFont.pointSize
                color: Kirigami.Theme.textColor
                readOnly: true
            }
        }

    Row {
        id: buttonRow
        spacing: 10
        Layout.alignment: Qt.AlignCenter
        // anchors.centerIn: parent
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter

        Button
        {
            id: getReleaseInfo
            text: "检查更新"
            icon.name: "system-software-update"
            onClicked: {
                loadingOverlay.visible = true
                updater.getReleaseInfo()
            }
        }
        Button
        {
            id: downloadUpdate
            visible: false
            text: "下载更新"
            icon.name: "download"
            onClicked: a
        }
    }

    // 转圈圈
    Item {
        id: loadingOverlay
        anchors.fill: parent
        visible: false
        z: 100

        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 0.5
        }

        BusyIndicator {
            id: busyIndicator
            anchors.centerIn: parent
            running: parent.visible
            width: 80
            height: 80
        }

        Label {
            anchors.top: busyIndicator.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 10
            text: "正在检查更新..."
            color: "white"
            font.pointSize: 12
        }
    }
}