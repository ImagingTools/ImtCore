import QtQuick

Item {
    Timer {
        interval: 100
        running: true
        repeat: false
        onTriggered: {
            console.log("timer triggered")
            Qt.quit()
        }
    }

    Component.onCompleted: {
        console.log("component created")
    }
}
