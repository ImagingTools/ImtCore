import QtQuick

Item {
    id: root

    signal loadedReady()

    Loader {
        id: loader
        width: 60
        height: 40
        source: "LoadedPart.qml"

        onLoaded: root.loadedReady()
    }

    function loadedSize() {
        return "loaded"
    }
}