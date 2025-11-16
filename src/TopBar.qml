import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: topBar
    width: parent.width
    anchors.top: parent.top

    property string difficulty: "Easy"
    property int errorCount: 0
    property string elapsedTime: "00:00"
    property int pixelSize: 12

    GlobalsUtil {
        id: globalFontStyles
    }

    Row {
        id: infoRow
        anchors.fill: parent
        anchors.margins: 3
        width: parent.width
        height: parent.height
        anchors.verticalCenter: parent.verticalCenter

        // Difficulty
        Item {
            width: infoRow.width / 3
            height: parent.height
            Text {
                anchors.centerIn: parent
                text: "🎯 " + topBar.difficulty
                font.pixelSize: topBar.pixelSize
            }
        }

        // Fails
        Item {
            width: infoRow.width / 3
            height: parent.height
            Text {
                anchors.centerIn: parent
                text: "❌ " + topBar.errorCount
                font.pixelSize: topBar.pixelSize
            }
        }

        // Timer
        Item {
            width: infoRow.width / 3
            height: parent.height
            Text {
                anchors.centerIn: parent
                text: "⏱ " + topBar.elapsedTime
                font.pixelSize: topBar.pixelSize
            }
        }
    }
}
