import QtQuick

Item {
    Item {
        id: itemObject
        property int counter: 0
    }

    Timer {
        id: doneTimer
        interval: 1
        repeat: false
        onTriggered: Qt.quit()
    }

    Connections {
        target: itemObject
        function onCounterChanged() {
            console.log("counter changed")
        }
    }

    Component.onCompleted: {
        itemObject.counter = 5
        console.log("counter", itemObject.counter)
        doneTimer.start()
    }
}
