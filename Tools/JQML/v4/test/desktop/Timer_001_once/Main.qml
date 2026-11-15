import QtQuick

Item {
    id: root
    property int value: 3

    Timer {
        interval: 30
        running: true
        onTriggered: {
            console.log("tick", root.value)
            Qt.quit()
        }
    }
}
