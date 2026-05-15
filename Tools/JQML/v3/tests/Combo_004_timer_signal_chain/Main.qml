import QtQuick

MidTimer {
    id: root

    property int finalDelay: midDelay + 1

    onTicked: {
        console.log("combo4-ticked", intervalValue, finalDelay)
        Qt.quit()
    }

    Component.onCompleted: {
        baseDelay = 2
        console.log("combo4-start", baseDelay, midDelay, finalDelay)
        startMidTimer()
    }
}
