import QtQuick

BaseLoaderTimer {
    id: root

    property int midState: 2

    Timer {
        id: timer
        interval: 0
        repeat: false
        running: false

        onTriggered: root.midReady()
    }

    signal midReady()

    function summary() {
        return baseLabel() + ":" + midState
    }

    function startAll() {
        timer.start()
    }
}