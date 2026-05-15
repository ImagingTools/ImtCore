import QtQuick

Item {
    id: root

    signal loadedReady()

    function loadedMetrics() {
        return loader.item.width + ":" + loader.item.height
    }

    Loader {
        id: loader
        width: 80
        height: 50
        source: "LoadedItem.qml"

        onLoaded: root.loadedReady()
    }
}
