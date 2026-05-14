import QtQuick

Item {
    id: root

    property int baseDelay: 1
    property int intervalValue: 0

    signal ticked()

    Timer {
        id: timer
        interval: 0
        repeat: false
        running: false

        onTriggered: root.ticked()
    }

    function startTimer(ms) {
        intervalValue = ms
        timer.interval = ms
        timer.start()
    }
}
