import QtQuick

BaseTimer {
    id: root

    property int midDelay: baseDelay + 1

    function startMidTimer() {
        startTimer(midDelay)
    }

    Component.onCompleted: {
        console.log("timer-mid", midDelay)
    }
}
