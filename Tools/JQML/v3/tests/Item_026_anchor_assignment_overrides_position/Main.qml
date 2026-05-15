import QtQuick

Item {
    id: root
    width: 500
    height: 400

    Rectangle {
        id: rect
        x: 100
        y: 50
        width: 20
        height: 20

        onXChanged: console.log("xChanged", x)
        onYChanged: console.log("yChanged", y)
    }

    Timer {
        id: doneTimer
        interval: 1
        repeat: false
        onTriggered: {
            console.log("final", rect.x, rect.y)
            Qt.quit()
        }
    }

    Component.onCompleted: {
        rect.anchors.left = root.left
        rect.anchors.top = root.top
        doneTimer.start()
    }
}
