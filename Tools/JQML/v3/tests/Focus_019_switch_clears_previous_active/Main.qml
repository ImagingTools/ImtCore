import QtQuick

Window {
    id: root
    width: 320
    height: 220
    visible: true

    FocusScope {
        id: fieldLike
        x: 10
        y: 10
        width: 120
        height: 30

        Item {
            id: inner
            anchors.fill: parent
            focus: true
        }
    }

    FocusScope {
        id: comboLike
        x: 10
        y: 50
        width: 120
        height: 30
    }

    FocusScope {
        id: comboLike2
        x: 10
        y: 90
        width: 120
        height: 30
    }

    Timer {
        id: stepTimer
        interval: 50
        repeat: false
        onTriggered: {
            inner.forceActiveFocus()
            console.log("focus019-inner", inner.activeFocus, fieldLike.activeFocus, comboLike.activeFocus, comboLike2.activeFocus)
            comboLike.forceActiveFocus()
            console.log("focus019-combo", inner.activeFocus, fieldLike.activeFocus, comboLike.activeFocus, comboLike2.activeFocus)
            comboLike2.forceActiveFocus()
            console.log("focus019-combo2", inner.activeFocus, fieldLike.activeFocus, comboLike.activeFocus, comboLike2.activeFocus)
            Qt.quit()
        }
    }

    Component.onCompleted: {
        stepTimer.start()
    }
}
