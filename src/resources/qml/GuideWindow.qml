import com.countdown
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import org.kde.kirigami as Kirigami

Window {
    id: guideWindow
    title: qsTr("使用引导")
    width: 640
    height: 480
    visible: false
    color: Kirigami.Theme.backgroundColor

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        TextArea {
            text: CountdownManager.getGuideMarkdown()
            textFormat: TextArea.MarkdownText
            readOnly: true
            wrapMode: TextArea.Wrap
            color: Kirigami.Theme.textColor
            background: null   // 去掉默认背景
        }
    }
}
