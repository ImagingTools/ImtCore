import QtQuick

Item {
    id: root

    property int baseTicks: 0

    signal baseReady()

    Timer {
        id: timer
        interval: 0
        repeat: false
        running: false

        onTriggered: root.baseReady()
    }

    function startBase() {
        timer.start()
    }
}