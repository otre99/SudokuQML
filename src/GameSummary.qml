import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: summaryPanel
    anchors.fill: parent
    color: "#000000AA" // Semi-transparent background
    visible: false
    z: 100

    function scoreCountForErrors(e) {
        return Math.floor(0.5 * e * (e + 1))
    }

    property alias title: titleText.text
    property string difficulty: "Easy"
    property int invalidAttempts: 0
    property int errorCount: 0
    property int usedHints: 0
    property string totalTime: "00:00"
    property real pixelSize: 16
    signal dismissed

    Rectangle {
        id: card
        width: parent.width * 0.8
        height: parent.height * 0.5
        anchors.centerIn: parent
        color: "white"
        radius: 16
        border.color: "#cccccc"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16
            Layout.alignment: Qt.AlignCenter

            // Title
            Text {
                id: titleText
                text: "You Win!"
                font.pixelSize: summaryPanel.pixelSize * 1.2
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }

            // Summary rows
            Repeater {
                model: [{
                        "label": "Score:",
                        "value": 1000 - summaryPanel.scoreCountForErrors(
                                     summaryPanel.usedHints) * 10
                                 - summaryPanel.scoreCountForErrors(
                                     summaryPanel.errorCount) * 100 - 5
                                 * summaryPanel.invalidAttempts
                    }, {
                        "label": "Difficulty:",
                        "value": summaryPanel.difficulty
                    }, {
                        "label": "Total Time:",
                        "value": summaryPanel.totalTime
                    }, {
                        "label": "Errors:",
                        "value": summaryPanel.errorCount
                    }, {
                        "label": "Used hints:",
                        "value": summaryPanel.usedHints
                    }, {
                        "label": "Invalid Attempts:",
                        "value": summaryPanel.invalidAttempts
                    }]

                delegate: RowLayout {
                    spacing: 8
                    width: parent.width

                    Text {
                        text: modelData.label
                        font.pixelSize: summaryPanel.pixelSize
                        Layout.fillWidth: true
                    }

                    Text {
                        text: modelData.value
                        font.pixelSize: summaryPanel.pixelSize
                        font.bold: true
                    }
                }
            }

            // Dismiss button
            Button {
                text: "OK"
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    summaryPanel.visible = false
                    summaryPanel.dismissed()
                }
            }
        }
    }
}
