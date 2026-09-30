import QtQuick

Window {
    id: root
    width: 320
    height: 200
    visible: true

    Item {
        id: btn
        x: 10
        y: 10
        width: 100
        height: 30
    }

    FocusScope {
        id: comboLike
        x: 10
        y: 50
        width: 100
        height: 30

        Rectangle {
            anchors.fill: parent
            border.width: 1
            border.color: comboLike.activeFocus ? "#00a2ff" : "#888888"
            color: "transparent"
        }
    }

    Timer {
        id: stepTimer
        interval: 50
        repeat: false
        onTriggered: {
            console.log("focus018-start", comboLike.focus, comboLike.activeFocus, btn.focus, btn.activeFocus)
            comboLike.focus = true
            comboLike.forceActiveFocus()
            console.log("focus018-combo", comboLike.focus, comboLike.activeFocus, btn.focus, btn.activeFocus)
            btn.forceActiveFocus()
            console.log("focus018-btn", comboLike.focus, comboLike.activeFocus, btn.focus, btn.activeFocus)
            comboLike.forceActiveFocus()
            console.log("focus018-combo-again", comboLike.focus, comboLike.activeFocus, btn.focus, btn.activeFocus)
            Qt.quit()
        }
    }

    Component.onCompleted: {
        stepTimer.start()
    }
}
