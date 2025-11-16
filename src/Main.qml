import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtCore

Window {
    id: window
    width: 600
    height: width * 17 / 9
    visible: true

    GlobalsUtil {
        baseSize: window.width
        id: globalFontStyles
    }

    Rectangle {
        id: sudokuControler
        color: "#f0f0f0"
        property int currentCellIndex: -1
        property list<bool> currentCandidates: []
        property int currentValue: 0
        property bool currentCellIsFrozen: false

        TopBar {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: globalFontStyles.horizontalBarHeight
            pixelSize: globalFontStyles.normal
            id: topBar

            function endNewGame() {
                timer.running = false
            }
            function startNewGame(lev_difficulty) {
                topBar.difficulty = lev_difficulty
                timer.elapsedSeconds = 0
                topBar.elapsedTime = "00:00"
                timer.running = true
                hintButton.usedHints = 0
                sudokuBoardRepeater.itemAt(0).updateGlobal()
            }

            Timer {
                id: timer
                interval: 1000
                repeat: true
                property int elapsedSeconds: 0
                onTriggered: {
                    elapsedSeconds += 1
                    topBar.elapsedTime = topBar.formatTime(elapsedSeconds)
                }
            }

            function formatTime(seconds) {
                let min = Math.floor(seconds / 60)
                let sec = seconds % 60
                return (min < 10 ? "0" + min : min) + ":" + (sec < 10 ? "0" + sec : sec)
            }
        }
        anchors.fill: parent
        GridLayout {
            id: grid
            anchors.top: topBar.bottom
            anchors.margins: 3
            anchors.left: parent.left
            anchors.right: parent.right
            height: width
            columns: 9
            columnSpacing: 1
            rowSpacing: 1
            Repeater {
                id: sudokuBoardRepeater
                model: SudokuModel
                delegate: SudokuCell {
                    id: cell
                    textColor: getTextColor(model.isFrozen, model.isHint,
                                            model.isError)
                    cellColor: model.cellColor
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    candidates: model.candidates
                    value: model.value
                    isSelected: index == sudokuControler.currentCellIndex
                    MouseArea {
                        anchors.fill: parent
                        onClicked: cell.updateGlobal()
                    }

                    function updateGlobal() {
                        sudokuControler.currentCellIndex = index
                        sudokuControler.currentCandidates = cell.candidates
                        sudokuControler.currentValue = cell.value
                        sudokuControler.currentCellIsFrozen = model.isFrozen
                    }

                    function getTextColor(frozen, hint, error) {
                        if (hint) {
                            return "darkGreen"
                        }
                        if (frozen) {
                            return "#333333"
                        }

                        if (error) {
                            return "red"
                        }

                        return "#1565c0"
                    }
                }
            }
        }

        Column {
            anchors.top: grid.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 4

            Item {
                width: parent.width
                height: globalFontStyles.horizontalBarHeight / 4
            }

            Row {
                height: globalFontStyles.horizontalBarHeight
                width: parent.width
                Text {
                    text: "Values \u{1F522}"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: globalFontStyles.normal
                    height: parent.height
                    width: parent.width / 2
                }

                Button {
                    id: hintButton
                    text: "\u{1F4A1}"
                    height: parent.height
                    width: parent.height
                    font.pixelSize: globalFontStyles.normal
                    background: Item {}
                    readonly property int maxAllowedHints: settings.maxAllowedHints
                    property int usedHints: 0
                    visible: usedHints < maxAllowedHints
                    padding: 0
                    leftPadding: 0
                    rightPadding: 0
                    topPadding: 0
                    bottomPadding: 0

                    contentItem: Text {
                        text: hintButton.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font: hintButton.font
                        scale: hintButton.pressed ? 1.2 : 1.0
                        anchors.centerIn: parent
                    }
                    onClicked: {
                        let v = SudokuModel.hint()
                        if (v > 0)
                            hintButton.usedHints += 1

                        console.log(maxAllowedHints, usedHints, v)
                    }
                }
            }

            RowLayout {
                id: rowValuesSetter
                width: parent.width
                height: globalFontStyles.horizontalBarHeight
                Repeater {
                    id: valuesNumbers
                    model: 10
                    delegate: SetterButton {
                        id: root
                        number: index
                        selected: index === sudokuControler.currentValue
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        pixelSize: globalFontStyles.normal
                        enabled: sudokuControler.currentCellIsFrozen === false
                        onClicked: {

                            if (number === sudokuControler.currentValue) {
                                sudokuControler.currentValue = 0
                                SudokuModel.setCell(
                                            sudokuControler.currentCellIndex,
                                            0,
                                            sudokuControler.currentCandidates,
                                            false)
                                return
                            }

                            let conflicts = SudokuModel.getCellsInConflict(
                                    sudokuControler.currentCellIndex, number)
                            if (conflicts.length !== 0) {
                                sudokuBoardRepeater.itemAt(
                                            sudokuControler.currentCellIndex).invalidAnimate(
                                            number)
                                for (var i = 0; i < conflicts.length; ++i) {
                                    let item = sudokuBoardRepeater.itemAt(
                                            conflicts[i])
                                    item.conflictAnimate()
                                }
                            } else {
                                sudokuControler.currentValue = number
                                SudokuModel.setCell(
                                            sudokuControler.currentCellIndex,
                                            number,
                                            sudokuControler.currentCandidates,
                                            false)
                            }
                        }
                    }
                }
            }

            Item {
                width: parent.width
                height: globalFontStyles.horizontalBarHeight / 4
            }

            Text {
                text: "Annotations \u270D\ufe0f"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: globalFontStyles.normal
                width: parent.width
                height: globalFontStyles.horizontalBarHeight
            }
            RowLayout {
                width: parent.width
                height: globalFontStyles.horizontalBarHeight * 0.8
                id: rowCandidatesSetter
                Repeater {
                    id: candidatesNumbers
                    model: 10
                    delegate: SetterButton {
                        number: index
                        selected: sudokuControler.currentCandidates[index]
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        pixelSize: globalFontStyles.normal * 0.8
                        enabled: sudokuControler.currentValue < 1
                        onClicked: {

                            let conflicts = SudokuModel.getCellsInConflict(
                                    sudokuControler.currentCellIndex, number)
                            if (conflicts.length !== 0) {
                                sudokuBoardRepeater.itemAt(
                                            sudokuControler.currentCellIndex).invalidAnimate(
                                            number)
                                for (var i = 0; i < conflicts.length; ++i) {
                                    let item = sudokuBoardRepeater.itemAt(
                                            conflicts[i])
                                    item.conflictAnimate()
                                }
                            } else {
                                sudokuControler.currentCandidates[index]
                                        = !sudokuControler.currentCandidates[index]
                                SudokuModel.setCell(
                                            sudokuControler.currentCellIndex,
                                            0,
                                            sudokuControler.currentCandidates,
                                            false)
                            }
                        }
                    }
                }
            }

            Item {
                width: parent.width
                height: globalFontStyles.horizontalBarHeight / 2
            }

            RowLayout {
                width: parent.width
                height: globalFontStyles.horizontalBarHeight * 1.5
                spacing: 10
                Button {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    id: btNewGame
                    text: "New Game"
                    font.pixelSize: globalFontStyles.normal
                    onClicked: {
                        let lev = SudokuModel.generateNewGame(
                                settings.difficulty)
                        topBar.startNewGame(lev)
                    }
                    background: Rectangle {
                        radius: 8
                        color: btNewGame.pressed ? "#2980b9" : "#ecf0f1"
                        border.color: btNewGame.pressed ? "#1c5980" : "#bdc3c7"
                        border.width: 2
                    }
                }

                Button {
                    text: "⚙️"
                    font.pixelSize: globalFontStyles.normal * 1.5
                    Layout.fillHeight: true
                    background: Item {} // make it symbol-only if desired
                    onClicked: settings.open()
                }
            }
        }
    }

    SettingsUI {
        id: settings
        width: Math.min(parent.width, parent.height) * 0.8
        height: globalFontStyles.horizontalBarHeight * 6
        anchors.centerIn: parent
        visible: false
        pixelSize: globalFontStyles.normal

        function open() {
            settings.scale = 0.1
            settings.visible = true
            sudokuControler.enabled = false
            animset.start()
        }

        Settings {
            id: settingStorage
            property int difficulty: 0
            property bool showPrecalculatedAnns: false
            property int maxAllowedHints: 0
        }

        ScaleAnimator on scale {
            id: animset
            from: 0.1
            to: 1.0
            duration: 300
        }

        onSave: {
            settingStorage.difficulty = settings.difficulty
            settingStorage.showPrecalculatedAnns = settings.showPrecalculatedAnns
            settingStorage.maxAllowedHints = settings.maxAllowedHints
            settings.visible = false
            sudokuControler.enabled = true
            SudokuModel.precalculateAnnotations(
                        settingStorage.showPrecalculatedAnns)
        }

        Component.onCompleted: {
            settings.difficulty = settingStorage.difficulty
            settings.showPrecalculatedAnns = settingStorage.showPrecalculatedAnns
            settings.maxAllowedHints = settingStorage.maxAllowedHints
            SudokuModel.precalculateAnnotations(
                        settingStorage.showPrecalculatedAnns)
        }
    }

    GameSummary {
        id: gameSummary
        anchors.centerIn: parent
        pixelSize: globalFontStyles.normal
        opacity: 0.95
        width: parent.width * 0.6
        height: parent.height * 0.6
        difficulty: topBar.difficulty
        errorCount: topBar.errorCount
        usedHints: hintButton.usedHints

        function open() {
            gameSummary.scale = 0.1
            gameSummary.visible = true
            sudokuControler.enabled = false
            anim.start()
        }

        ScaleAnimator on scale {
            id: anim
            from: 0.1
            to: 1.0
            duration: 300
        }
        onDismissed: {
            sudokuControler.enabled = true
        }
    }

    Connections {
        target: SudokuModel
        function onFailCountChanged() {
            gameSummary.invalidAttempts = SudokuModel.failCount
        }

        function onErrorCountChanged() {
            topBar.errorCount = SudokuModel.errorCount
            if (SudokuModel.errorCount === 3) {
                gameSummary.totalTime = topBar.elapsedTime
                gameSummary.title = "You Loss!"
                gameSummary.errorCount = SudokuModel.errorCount
                gameSummary.open()
            }
        }

        function onSuccessChanged() {
            if (SudokuModel.success) {
                gameSummary.totalTime = topBar.elapsedTime
                gameSummary.title = "You Win!"
                gameSummary.errorCount = SudokuModel.errorCount
                gameSummary.open()
            }
        }
    }

    Connections {
        target: gameSummary
        function onDismissed() {
            btNewGame.clicked()
        }
    }
}
