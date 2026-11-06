import QtQuick

Item {
    width: 400
    height: 300

    Loader {
        id: loader
        sourceComponent: Rectangle { width: 32; height: 40 }

        onLoaded: {
            loader.width = item.width
            loader.height = item.height
            item.width = 200
        }
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("after load", loader.item.width, loader.item.height, "loader", loader.width, loader.height)
            loader.item.width = 120
            console.log("script", loader.item.width, "loader", loader.width)
            loader.width = 60
            console.log("loader resized", loader.item.width, "loader", loader.width)
            Qt.quit()
        }
    }
}
