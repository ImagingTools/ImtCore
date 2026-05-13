import QtQuick

Item {
    id: root
    width: 500
    height: 500

    Timer {
        id: doneTimer
        interval: 1
        repeat: false
        onTriggered: {
            console.log("sizes", loader.width, loader.height, loader.item.width, loader.item.height)
            Qt.quit()
        }
    }

    Loader {
        id: loader
        onWidthChanged: console.log("loader width", width)
        onHeightChanged: console.log("loader height", height)

        sourceComponent: Rectangle {
            onWidthChanged: console.log("item width", width)
            onHeightChanged: console.log("item height", height)
        }

        onLoaded: {
            item.width = root.width
            item.height = root.height
            doneTimer.start()
        }
    }
}
