import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.kde.kirigami as Kirigami

Window {
    id: updaterWindow
    title: qsTr("更新")
    width: 640
    height: 480
    visible: false
    color: Kirigami.Theme.backgroundColor

    Connections {
        target: updater

        // 检查更新
        function onNewVersion(latestVersion, version , updateLog)
        {
            if ( latestVersion ) versionShow.text = version
            else versionShow.text = qsTr("当前已是最新版本：") + version
            versionShow.visible = true

            versionUpdateLog.text = updateLog
            versionUpdateLogScroll.visible = true

            downloadUpdate.visible = latestVersion

            loadingOverlay.visible = false // 转圈圈
        }

        // 下载进度
        function onDownloadProgress(bytesReceived, bytesTotal)
        {
            downloadProgressBar.indeterminate = false
            downloadProgressBar.value = bytesReceived / bytesTotal
        }

        // 下载完成
        function onDownloadFinished()
        {
            downloadProgressBar.indeterminate = true
            downloadText.text = qsTr("正在安装更新...")
        }

        // 下载失败
        function onDownloadError(errorString)
        {
            downloadOverlay.visible = false
            inlineMessage.text = qsTr("下载失败：") + errorString
            inlineMessage.type = Kirigami.MessageType.Error
            inlineMessage.visible = true
        }

        // 安装成功
        function onInstallSuccess()
        {
            downloadOverlay.visible = false
            inlineMessage.text = qsTr("安装成功，请手动重启程序")
            inlineMessage.type = Kirigami.MessageType.Positive
            inlineMessage.visible = true
        }
    }

    Text {
        id: versionShow
        visible: false
        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 2
        color: Kirigami.Theme.textColor
        font.weight: Font.Bold
        text: qsTr("未知版本")

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
                text: qsTr("未知更新日志")
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
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter

        Button
        {
            id: getReleaseInfo
            text: qsTr("检查更新")
            icon.name: "update-none"
            onClicked: {
                loadingOverlay.visible = true
                updater.getReleaseInfo()
            }
        }
        Button
        {
            id: downloadUpdate
            visible: false
            text: qsTr("下载更新")
            icon.name: "download"
            onClicked: {
                downloadOverlay.visible = true
                updater.downloadNewVersion()
            }
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
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
        }
        BusyIndicator {
            id: loadingBusyIndicator
            anchors.centerIn: parent
            running: parent.visible
            width: 80
            height: 80
        }
        Label {
            anchors.top: loadingBusyIndicator.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 10
            text: qsTr("正在检查更新...")
            color: "white"
            font.pointSize: 12
        }
    }

    Item {
        id: downloadOverlay
        anchors.fill: parent
        visible: false
        z: 100
        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 0.5
        }
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
        }
        ProgressBar {
            id: downloadProgressBar
            value: 0.0
            indeterminate: true // 不确定模式
            anchors.centerIn: parent
        }
        Label {
            id: downloadText
            anchors.top: downloadProgressBar.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 10
            text: qsTr("正在下载更新...")
            color: "white"
            font.pointSize: 12
        }
    }

    //提醒土司
    Kirigami.InlineMessage {
        id: inlineMessage
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        visible: false
        showCloseButton: true
        onVisibleChanged: {
            if (visible) {
                hideMessageTimer.restart()
            }
        }
    }

    // 两秒自动关闭
    Timer {
        id: hideMessageTimer
        interval: 2000
        onTriggered: {
            inlineMessage.visible = false
        }
    }
}