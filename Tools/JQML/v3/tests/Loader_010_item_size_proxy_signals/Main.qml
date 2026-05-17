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
            console.log("proxy", loader.itemWidth, loader.itemHeight)
            Qt.quit()
        }
    }

    Loader {
        id: loader
        property real itemWidth: item ? item.width : 0
        property real itemHeight: item ? item.height : 0

        sourceComponent: Rectangle {}

        onLoaded: {
            item.width = root.width
            item.height = root.height
            doneTimer.start()
        }
    }
}
